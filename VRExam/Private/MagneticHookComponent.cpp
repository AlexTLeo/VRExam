// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticHookComponent.h"

// Sets default values for this component's properties
UMagneticHookComponent::UMagneticHookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsHoldingProp = false;
}


// Called when the game starts
void UMagneticHookComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMagneticHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMagneticHookComponent::SetHoldingStatus(bool Status) {
	IsHoldingProp = Status;
}

bool UMagneticHookComponent::GetHoldingStatus() {
	return IsHoldingProp;
}