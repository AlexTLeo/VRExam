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

	UPROPERTY()
		UPrimitiveComponent* GrabbedObject;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* FPSCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = Components)
		class USceneComponent* GrabbedObjectLocation;

	void SetGrabbedObject(UPrimitiveComponent* ObjectToGrab);

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
