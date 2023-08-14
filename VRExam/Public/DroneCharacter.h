// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h" 
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h" 
#include "MagCrateComponent.h"

#include "DroneCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class VREXAM_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/// Sets default values for this character's properties
	ADroneCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// Cleanup when game ends
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
	* HUD
	*/
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UMyUI> PlayerUIClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UMyEndingScreenUI> EndingScreenUIClass;

	UPROPERTY()
		class UMyUI* PlayerUI;

	UPROPERTY()
		class UMyEndingScreenUI* EndingScreenUI;

	/*
	* Everything else
	*/

	UPROPERTY()
		UPrimitiveComponent* GrabbedObject;
	UPROPERTY()
		UMagCrateComponent* GrabbedObjectCrateHandler;
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
	UPROPERTY()
		AActor* MainPlayerActor; /// The actual player actor
	UPROPERTY()
		UPhysicsConstraintComponent* PhysicsConstraint;

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

	/// <summary>
	/// Ends the game and shows an ending screen
	/// </summary>
	void EndSimulation();

	/// Handles input for moving forward and backward.
	UFUNCTION()
		void MoveForward(float Value);

	/// Handles input for moving right and left.
	UFUNCTION()
		void MoveRight(float Value);

	/// Handles input for moving up and down.
	UFUNCTION()
		void MoveUp(float Value);

private:
	int MaxProgressTransport; /// Number of things to transport
	int CurrentProgressTransport; /// How many transported so far

	/// Used to interpolate end velocity to smoothly stop player
	float TimeElapsed;
	bool bEndMovement;

	UPROPERTY(EditAnywhere)
	float LerpDuration;
};
