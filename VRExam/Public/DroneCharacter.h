// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h" 
#include "Components/CapsuleComponent.h"

#include "DroneCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class VREXAM_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/// Sets default values for this character's properties
	ADroneCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY()
		UPrimitiveComponent* GrabbedObject;
	UPROPERTY()
		TArray<AActor*> AllMagneticHooks; /// Contains all magnetic hooks
	UPROPERTY()
		AActor* ClosestMagneticHook;
	UPROPERTY()
		double DistanceToClosestHook;
	UPROPERTY()
		double MinDistanceToMagHook; /// Minimum distance to allow object to be magnetically hooked
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* FPSCameraComponent;
	UPROPERTY(EditDefaultsOnly, Category = Components)
		class USceneComponent* GrabbedObjectLocation;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// Grab object
	UFUNCTION()
		void OnGrab();

	/// Release object 
	UFUNCTION()
		void EndGrab();

	/// <summary>
	/// Move the targeted object to float in front of the player and highlight magnetic hooks.
	/// </summary>
	/// <param name="ObjectToGrab">Targeted object</param>
	void SetGrabbedObject(UPrimitiveComponent* ObjectToGrab);

	/// <summary>
	/// Check for any nearby magnetic hooks. A line will be drawn
	/// from the grabbed object to the closest hook specifically. 
	/// If the closest hook is close enough, the grabbed object will glow and,
	/// when released, it will attach to the magnetic hook.
	/// </summary>
	void CheckNearbyMagneticHooks();

	/// Handles input for moving forward and backward.
	UFUNCTION()
		void MoveForward(float Value);

	/// Handles input for moving right and left.
	UFUNCTION()
		void MoveRight(float Value);

	/// Handles input for moving up and down.
	UFUNCTION()
		void MoveUp(float Value);
};
