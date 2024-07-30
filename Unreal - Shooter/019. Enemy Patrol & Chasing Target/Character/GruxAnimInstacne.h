#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GruxAnimInstacne.generated.h"

class AEnemy;

UCLASS()
class SHOOTER_API UGruxAnimInstacne : public UAnimInstance
{
	GENERATED_BODY()
public:
	UGruxAnimInstacne();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AEnemy* Enemy;

	// 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
};
