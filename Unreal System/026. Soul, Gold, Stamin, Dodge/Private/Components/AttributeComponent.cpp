#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	// FMath::Clamp (X, X의 최소 제한값, X의 최대 제한값)
	Health = FMath::Clamp(Health - Damage, 0.f, Maxhealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	float Percent = 0;
	if (Health != 0) 
	{
		Percent = Health / Maxhealth;
	}
	// Health가 0이면 0을 반환
	return Percent;
}

float UAttributeComponent::GetStaminaPercent()
{
	float Percent = 0;
	if (Stamina != 0)
	{
		Percent = Stamina / MaxStamina;
	}
	// Health가 0이면 0을 반환
	return Percent;
}

bool UAttributeComponent::IsAlive()
{	
	// Health가 0보다 크면 true
	return Health > 0.0f;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}

