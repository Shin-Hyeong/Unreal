#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	// FMath::Clamp (X, X의 최소 제한값, X의 최대 제한값)
	Health = FMath::Clamp(Health - Damage, 0.f, Maxhealth);
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

bool UAttributeComponent::IsAlive()
{	
	// Health가 0보다 크면 true
	return Health > 0.0f;
}

