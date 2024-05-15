#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterType.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class SOULSLIKE__API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	/** BeginPlay */
	virtual void NativeInitializeAnimation() override;
	/** TickEvent */
	virtual void NativeUpdateAnimation(float DeltaTime)  override;

public:
	UPROPERTY(BlueprintReadOnly)
	class AEnemy* EnemyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* EnemyMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float EnemySpeed;

	UPROPERTY(BlueprintReadOnly, Category = State)
	EEnemyState EnemyState;

	UPROPERTY(BlueprintReadOnly, Category = State)
	TEnumAsByte<EDeathPose> DeathPose;
};
