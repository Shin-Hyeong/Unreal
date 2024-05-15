#include "Components/AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributesComponent::RegenHealth(float DeltaTime)
{
	Health = FMath::Clamp(Health + HealthRegenRate * DeltaTime, 0.f, MaxHealth);
}

void UAttributesComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributesComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributesComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributesComponent::GetHealthPercent()
{
	float Percent = 0.0f;
	if (Health > 0.0f)
	{
		Percent = Health / MaxHealth;
	}
	return Percent;
}

float UAttributesComponent::GetStaminaPercent()
{
	float Percent = 0.f;
	if (Stamina > 0.0f)
	{
		Percent = Stamina / MaxStamina;
	}
	return Percent;
}

bool UAttributesComponent::IsAlive()
{
	return Health > 0.0f;
}

bool UAttributesComponent::IsStaminaCost(float Cost)
{
	return Stamina >= Cost;
}

void UAttributesComponent::AddExp(int32 AmountOfExp)
{
	Exp += AmountOfExp;
}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}