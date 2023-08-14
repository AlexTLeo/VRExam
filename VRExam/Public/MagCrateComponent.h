// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagCrateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VREXAM_API UMagCrateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY()
		bool IsHooked; /// Is hooked to a mag hook?

	// Sets default values for this component's properties
	UMagCrateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/// <summary>
	///  Sets IsHooked to Status
	/// </summary>
	UFUNCTION()
	void SetHooked(bool Status);

	/// <summary>
	///  Get IsHooked (is crate currently hooked to a maghook?)
	/// </summary>
	UFUNCTION()
	bool GetHooked();
		
};
