#pragma once

#include "CoreMinimal.h"
// UE 엔진 폴더의 Components
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

	// Health에 Damage만큼 뺌.
	void ReceiveDamage(float Damage);

	void UseStamina(float StaminaCost);

	// HealthBar에 적용될 Health의 Percent를 구함
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
	// 현재 체력
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	// 최대 체력
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Maxhealth;

	// 현재 Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	// 최대 Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	// Dodge에 소모될 Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 25.f;

	// 초당 회복될 Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 5.0f;

	// 초당 회복될 HP
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float HealthRegenRate = 1.0f;
		
	// 소지 골드
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	// 획득한 경험치
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;
};
