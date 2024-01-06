#pragma once

#include "CoreMinimal.h"
// UE ���� ������ Components
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UDEMYCPP_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Health�� Damage��ŭ ��.
	void ReceiveDamage(float Damage);

	// HealthBar�� ����� Health�� Percent�� ����
	float GetHealthPercent();

	bool IsAlive();

protected:
	virtual void BeginPlay() override;

public:	

private:
	// ���� ü��
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	// �ִ� ü��
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Maxhealth;
		
};
