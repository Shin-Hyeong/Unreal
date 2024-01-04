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
	// FMath::Clamp (X, X�� �ּ� ���Ѱ�, X�� �ִ� ���Ѱ�)
	Health = FMath::Clamp(Health - Damage, 0.f, Maxhealth);
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

bool UAttributeComponent::IsAlive()
{	
	// Health�� 0���� ũ�� true
	return Health > 0.0f;
}

