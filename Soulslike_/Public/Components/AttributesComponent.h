#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE__API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributesComponent();

	/** <Component> */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** </Component> */

	// 체력 회복 속도
	void RegenHealth(float DeltaTime);
	// 스테미나 회복 속도
	void RegenStamina(float DeltaTime);

	// 데미지 피해 받음
	void ReceiveDamage(float Damage);
	// 스테미나 소모
	void UseStamina(float StaminaCost);

	// 체력 퍼센트 가져옴
	float GetHealthPercent();
	// 스테미나 퍼센트 가져옴
	float GetStaminaPercent();

	bool IsAlive();
	bool IsStaminaCost(float Cost);

	void AddExp(int32 AmountOfExp);

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetAttackCost() const { return AttackCost; }
	FORCEINLINE float GetPwAttackCost() const { return PowerAttackCost; }
	FORCEINLINE float GetComboAttackCost() const { return ComboAttackCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE int32 GetExp() const { return Exp; }

protected:
	/** <Component> */
	virtual void BeginPlay() override;
	/** </Component> */

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float AttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float PowerAttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float ComboAttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float HealthRegenRate;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Exp = 0;
};
