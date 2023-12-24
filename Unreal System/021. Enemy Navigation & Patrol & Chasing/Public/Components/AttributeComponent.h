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

	// Health에 Damage만큼 뺌.
	void ReceiveDamage(float Damage);

	// HealthBar에 적용될 Health의 Percent를 구함
	float GetHealthPercent();

	bool IsAlive();

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
		
};
