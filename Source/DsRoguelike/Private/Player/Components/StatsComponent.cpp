// Fill out your copyright notice in the Description page of Project Settings.

#include "StatsComponent.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	StatsParameters.Add(EStatsParameterType::SP_Health, FStatsParameter());
	StatsParameters.Add(EStatsParameterType::SP_Poise, FStatsParameter());
	StatsParameters.Add(EStatsParameterType::SP_Stamina, FStatsParameter());
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float WorldTime = GetWorld()->TimeSeconds;

	for (auto& StatsParameterPair : StatsParameters) {
		auto& StatsParameter = StatsParameterPair.Value;
		if (StatsParameter.Value < StatsParameter.MaxValue) {
			if (StatsParameter.LastDamageTime + StatsParameter.RegenerationDelay > WorldTime) {
				continue;
			}
			float RegeneratedValue = StatsParameter.Value + StatsParameter.RegenerationSpeed * DeltaTime;
			StatsParameter.Value = FMath::Clamp(RegeneratedValue, StatsParameter.MinValue, StatsParameter.MaxValue);
		}
	}
}

void UStatsComponent::GetStatsParameter(EStatsParameterType Type, FStatsParameter& Parameter)
{
	auto ParameterFound = StatsParameters.Find(Type);
	if (ParameterFound) {
		Parameter = *ParameterFound;
	}
}

void UStatsComponent::ApplyDamage(EStatsParameterType Type, float Damage)
{
	auto ParameterFound = StatsParameters.Find(Type);
	if (ParameterFound) {
		ParameterFound->Value -= Damage;
		ParameterFound->Value = FMath::Clamp(ParameterFound->Value, ParameterFound->MinValue, ParameterFound->MaxValue);
		ParameterFound->LastDamageTime = GetWorld()->TimeSeconds;
	}
}

void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

}
