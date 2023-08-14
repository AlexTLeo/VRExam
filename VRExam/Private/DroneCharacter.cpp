// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathVectorCommon.h" 
#include "Kismet/GameplayStatics.h" 
#include "DrawDebugHelpers.h" 
#include "MyUI.h"
#include "MyEndingScreenUI.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PawnMovementComponent.h" 
#include "GameFramework/MovementComponent.h" 

// Sets default values
ADroneCharacter::ADroneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);
	FPSCameraComponent->SetupAttachment(GetMesh());	
	FPSCameraComponent->SetRelativeLocation(FVector(0, 0, 0));
	FPSCameraComponent->bUsePawnControlRotation = true;

	GrabbedObjectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("GrabbedObjectLocation"));
	GrabbedObjectLocation->SetupAttachment(FPSCameraComponent);

	MinDistanceToMagHook = 200;
	DistanceToClosestHook = INT_MAX;

	// UI
	PlayerUIClass = nullptr;
	PlayerUI = nullptr;
	EndingScreenUIClass = nullptr;
	EndingScreenUI = nullptr;

	// Game Progress
	MaxProgressTransport = 2;
	CurrentProgressTransport = 0;

	// Ending velocity lerp
	TimeElapsed = 0;
	LerpDuration = 3;
	bEndMovement = false;
}

// Called when the game starts or when spawned
void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));

	// UI
	if (IsLocallyControlled() && PlayerUIClass) {
		APlayerController * MyController = Cast<APlayerController>(GetController());
		check(MyController);
		PlayerUI = CreateWidget<UMyUI>(MyController, PlayerUIClass);
		check(PlayerUI);
		PlayerUI->AddToPlayerScreen();
		PlayerUI->SetProgressTransport(0, 1); // Reset progress
	}

	MainPlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// The grabbed object should maintain its collision with the environment
	if (MainPlayerActor != nullptr) {
		PhysicsConstraint = Cast<UPhysicsConstraintComponent>(NewObject<UPhysicsConstraintComponent>(MainPlayerActor,
			UPhysicsConstraintComponent::StaticClass(), FName(TEXT("PhysicsConstraint"))));
		PhysicsConstraint->RegisterComponent();
		PhysicsConstraint->AttachToComponent(Cast<USceneComponent>(MainPlayerActor->GetComponentByClass(USceneComponent::StaticClass())),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabbedObjectSocket"));

		PhysicsConstraint->ConstraintInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked);
		PhysicsConstraint->ConstraintInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked);
		PhysicsConstraint->ConstraintInstance.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Locked);
		PhysicsConstraint->ConstraintInstance.SetLinearXMotion(ELinearConstraintMotion::LCM_Locked);
		PhysicsConstraint->ConstraintInstance.SetLinearYMotion(ELinearConstraintMotion::LCM_Locked);
		PhysicsConstraint->ConstraintInstance.SetLinearZMotion(ELinearConstraintMotion::LCM_Locked);
		PhysicsConstraint->ConstraintInstance.ProfileInstance.bDisableCollision = true;
	}
}

// Called every frame
void ADroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Find closest magnetic hook
	CheckNearbyMagneticHooks();

	if (bEndMovement) {
		// Get the Pawn of the main player and stop its movement slowly, interpolating via FMath::Lerp
		APlayerController* MyController = Cast<APlayerController>(GetController());
		APawn* PlayerPawn = MyController->GetPawn();
		if (PlayerPawn) {
			UMovementComponent* PlayerPawnMovement = Cast<UMovementComponent>(PlayerPawn->GetMovementComponent());
			if (PlayerPawnMovement) {
				PlayerPawnMovement->Velocity = FMath::Lerp(PlayerPawnMovement->Velocity, FVector(0, 0, 0), TimeElapsed / LerpDuration);
				TimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(this);

				double ErrorFromZero = FVector3d::Distance(PlayerPawnMovement->Velocity, FVector(0, 0, 0));

				if (ErrorFromZero == 0) {
					bEndMovement = false;
				}
			}
		}
	}
}

// Called to bind functionality to input
void ADroneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "grab"
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &ADroneCharacter::OnGrab);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &ADroneCharacter::EndGrab);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &ADroneCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADroneCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ADroneCharacter::MoveUp);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &ADroneCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ADroneCharacter::AddControllerPitchInput);
}

void ADroneCharacter::OnGrab() {
	const FCollisionQueryParams QueryParams("GrabRayTrace", false, this); // Ignore player
	const float TraceRange = 5000.0f;
	const FVector StartTrace = FPSCameraComponent->GetComponentLocation();
	const FVector EndTrace = (FPSCameraComponent->GetForwardVector() * TraceRange) + StartTrace;
	FHitResult Hit;

	// Reset this
	ClosestMagneticHook = nullptr;
	DistanceToClosestHook = INT_MAX;

	// Ray trace
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, QueryParams)) {
		if (UPrimitiveComponent* Prim = Hit.GetComponent()) {
			if (Prim->ComponentHasTag("SpawnableProp")) {
				SetGrabbedObject(Prim);
			}
		}
	}
}

void ADroneCharacter::EndGrab() {
	// Check if we actually have an object grabbed
	if (GrabbedObject) {
		// Check if we are near a magnetic hook or not
		if (ClosestMagneticHook && DistanceToClosestHook <= MinDistanceToMagHook) {
			// If so, attach the object to the mag hook
	
			USceneComponent* MagHook = Cast<USceneComponent>(ClosestMagneticHook->GetComponentByClass(USceneComponent::StaticClass()));

			//FTransform wTo = GrabbedObject->GetComponentTransform();
			//FVector CoM = UKismetMathLibrary::InverseTransformLocation(wTo, GrabbedObject->GetCenterOfMass());
			//GrabbedObject->SetRelativeLocation(CoM);

			//GrabbedObject->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			PhysicsConstraint->BreakConstraint();
			GrabbedObject->AttachToComponent(MagHook, FAttachmentTransformRules::KeepWorldTransform, FName(TEXT("MagneticSocket")));
			GrabbedObject->SetAllPhysicsRotation(FRotator(0.0f, 0.0f, 0.0f));
			GrabbedObject->SetSimulatePhysics(false); // Turn physics off for the object

			// Update transport progress and crate status
			CurrentProgressTransport++;
			GrabbedObjectCrateHandler->SetHooked(true);

			// If met goals, end the game!
			if (CurrentProgressTransport >= MaxProgressTransport) {
				EndSimulation();
			} else if (PlayerUI) {
				PlayerUI->SetProgressTransport(CurrentProgressTransport, MaxProgressTransport);
			}

			SetGrabbedObject(nullptr);
		} else {
			// If not near a magnetic hook, simply release the object

			const float ThrowStrength = 50.0f;
			const FVector ThrowVelocity = FPSCameraComponent->GetForwardVector() * ThrowStrength;

			//GrabbedObject->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			PhysicsConstraint->BreakConstraint();
			GrabbedObject->SetSimulatePhysics(true); // Turn physics back on for grabbed object
			GrabbedObject->AddImpulse(ThrowVelocity, NAME_None, true);

			SetGrabbedObject(nullptr);
		}
	}
}

void ADroneCharacter::SetGrabbedObject(UPrimitiveComponent* ObjectToGrab) {
	GrabbedObject = ObjectToGrab;
	GrabbedObjectCrateHandler = nullptr; // Reset handler

	if (GrabbedObject) {
		// Check if this object was already magnetised somewhere
		AActor* GrabbedObjectActor = GrabbedObject->GetOwner();
		if (GrabbedObjectActor) {
			UMagCrateComponent* MagCrate = Cast<UMagCrateComponent>(GrabbedObjectActor->GetComponentByClass(UMagCrateComponent::StaticClass()));
			if (MagCrate) {
				GrabbedObjectCrateHandler = MagCrate;

				bool wasHooked = MagCrate->GetHooked();
				// Update transport progress and crate status
				if (wasHooked) {
					CurrentProgressTransport--;
					GrabbedObjectCrateHandler->SetHooked(false);
				}

				if (PlayerUI) {
					PlayerUI->SetProgressTransport(CurrentProgressTransport, MaxProgressTransport);
				}
			}
		}

		// Free the object in case it is attached to anything
		GrabbedObject->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// Attach object to the player
		GrabbedObject->SetSimulatePhysics(true); // Turn physics back on for grabbed object
		PhysicsConstraint->SetConstrainedComponents(GrabbedObject, NAME_None, Cast<UPrimitiveComponent>(MainPlayerActor->GetComponentByClass(USceneComponent::StaticClass())), NAME_None);

		UE_LOG(LogTemp, Warning, TEXT("Number of detected hooks: %d"), AllMagneticHooks.Num());
	}
}

void ADroneCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value/2);
}

void ADroneCharacter::CheckNearbyMagneticHooks() {
	// Get all magnetic hooks in the world
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MagneticHook"), AllMagneticHooks);

	// If grabbed object, calculate the distances from the grabbed object to the hooks and find the closest
	if (GrabbedObject && AllMagneticHooks.Num() > 0) {
		TArray<double> DistancesToHooks;
		DistancesToHooks.Init(-1, AllMagneticHooks.Num());
		DistanceToClosestHook = INT_MAX;
		int IndexOfClosestHook = 0;

		int i = 0;
		for (AActor* Hook : AllMagneticHooks) {
			DistancesToHooks[i] = FVector3d::Distance(Hook->GetActorLocation(), GrabbedObject->GetCenterOfMass());

			// Save the index for the closest hook
			if (DistancesToHooks[i] < DistanceToClosestHook) {
				DistanceToClosestHook = DistancesToHooks[i];
				IndexOfClosestHook = i;
			}
			
			i++;
		}

		ClosestMagneticHook = AllMagneticHooks[IndexOfClosestHook];

		// If close enough, trace a line from the grabbed object to the hook
		if (DistanceToClosestHook <= MinDistanceToMagHook) {
			DrawDebugLine(GetWorld(), GrabbedObject->GetComponentLocation(), ClosestMagneticHook->GetActorLocation(), FColor(0, 255, 0), false, -1.0f, 1, 1.0f);
		} else {
			// Otherwise show a semi-transparent grey line
			DrawDebugLine(GetWorld(), GrabbedObject->GetComponentLocation(), ClosestMagneticHook->GetActorLocation(), FColor(200, 200, 200, 125), false, -1.0f, 1, 1.0f);
		}
	}
}

void ADroneCharacter::EndSimulation() {
	if (IsLocallyControlled() && EndingScreenUIClass) {
		APlayerController* MyController = Cast<APlayerController>(GetController());
		check(MyController);
		EndingScreenUI = CreateWidget<UMyEndingScreenUI>(MyController, EndingScreenUIClass);
		check(EndingScreenUI);
		check(PlayerUI);
		PlayerUI->RemoveFromViewport();
		PlayerUI->RemoveFromParent();
		EndingScreenUI->AddToPlayerScreen();
		PlayerUI = nullptr;
		DisableInput(MyController);
		bEndMovement = true;
	}
}

void ADroneCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value/2);
}

void ADroneCharacter::MoveUp(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FVector(0, 0, 1);
	AddMovementInput(Direction, Value/2);
}