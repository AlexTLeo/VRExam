// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCharacter.h"
#include "Kismet/KismetMathLibrary.h"

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
}

// Called when the game starts or when spawned
void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
}

// Called every frame
void ADroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// Ray trace
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, QueryParams)) {
		if (UPrimitiveComponent* Prim = Hit.GetComponent()) {
			if (Prim->IsSimulatingPhysics()) {
				SetGrabbedObject(Prim);
			}
		}
	}
}

void ADroneCharacter::EndGrab() {
	// Check if we actually have an object grabbed
	if (GrabbedObject) {
		const float ThrowStrength = 50.0f;
		const FVector ThrowVelocity = FPSCameraComponent->GetForwardVector() * ThrowStrength;

		GrabbedObject->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedObject->SetSimulatePhysics(true); // Turn physics back on for grabbed object
		GrabbedObject->AddImpulse(ThrowVelocity, NAME_None, true);

		SetGrabbedObject(nullptr);
	}
}

void ADroneCharacter::SetGrabbedObject(UPrimitiveComponent* ObjectToGrab) {
	GrabbedObject = ObjectToGrab;

	if (GrabbedObject) {
		GrabbedObject->SetSimulatePhysics(false); // Momentarily turn physics off for grabbed object
		// Align the object to the centre of the screen
		FTransform wTo = GrabbedObject->GetComponentTransform();
		FVector CoM = UKismetMathLibrary::InverseTransformLocation(wTo, GrabbedObject->GetCenterOfMass());
		CoM.Z = 0;
		GrabbedObjectLocation->SetRelativeLocation(CoM);
		GrabbedObject->AttachToComponent(GrabbedObjectLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ADroneCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ADroneCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void ADroneCharacter::MoveUp(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FVector(0, 0, 1);
	AddMovementInput(Direction, Value);
}