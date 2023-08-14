// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagneticHookComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VREXAM_API UMagneticHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY()
		bool IsHoldingProp; /// Is hook currently holding a prop or not?

	// Sets default values for this component's properties
	UMagneticHookComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	///  Sets IsHoldingProp to Status
	/// </summary>
	UFUNCTION()
	void SetHoldingStatus(bool Status);

	/// <summary>
	///  Get IsHoldingProp (is the hook currently holding a prop?)
	/// </summary>
	UFUNCTION()
	bool GetHoldingStatus();
};
