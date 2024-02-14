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

	void RegenStamina(float DeltaTime);

	void RegenHealth(float DeltaTime);

	// Health�� Damage��ŭ ��.
	void ReceiveDamage(float Damage);

	void UseStamina(float StaminaCost);

	// HealthBar�� ����� Health�� Percent�� ����
	float GetHealthPercent();

	float GetStaminaPercent();

	bool IsAlive();

	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }

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

	// ���� Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	// �ִ� Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	// Dodge�� �Ҹ�� Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 25.f;

	// �ʴ� ȸ���� Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 5.0f;

	// �ʴ� ȸ���� HP
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float HealthRegenRate = 1.0f;
		
	// ���� ���
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	// ȹ���� ����ġ
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;
};
