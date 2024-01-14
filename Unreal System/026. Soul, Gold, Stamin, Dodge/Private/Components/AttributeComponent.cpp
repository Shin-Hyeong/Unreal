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
	// FMath::Clamp (X, X�� �ּ� ���Ѱ�, X�� �ִ� ���Ѱ�)
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
	// Health�� 0�̸� 0�� ��ȯ
	return Percent;
}

float UAttributeComponent::GetStaminaPercent()
{
	float Percent = 0;
	if (Stamina != 0)
	{
		Percent = Stamina / MaxStamina;
	}
	// Health�� 0�̸� 0�� ��ȯ
	return Percent;
}

bool UAttributeComponent::IsAlive()
{	
	// Health�� 0���� ũ�� true
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

