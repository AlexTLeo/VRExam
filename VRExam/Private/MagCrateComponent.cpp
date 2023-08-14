// Fill out your copyright notice in the Description page of Project Settings.


#include "MagCrateComponent.h"

// Sets default values for this component's properties
UMagCrateComponent::UMagCrateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsHooked = false;
}


// Called when the game starts
void UMagCrateComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Crate Spawned!"));
}


// Called every frame
void UMagCrateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMagCrateComponent::SetHooked(bool Status) {
	IsHooked = Status;
}

bool UMagCrateComponent::GetHooked() {
	return IsHooked;
}
