#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterType.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class SOULSLIKE__API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime)  override;

public:
	UPROPERTY(BlueprintReadOnly)
	class APlayableCharacter* PlayableCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerMovement;

	// 플레이어 입력 방향
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector2D MovementValue;

	// X, Y 합친 속도
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	// X 속도
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeedX;

	// Y 속도
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeedY;

	// 점프(땅에 발이 떨어져 있는지 유무)
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = State)
	ECharacterState CharacterWeaponState;

	UPROPERTY(BlueprintReadOnly, Category = State)
	EPlayerState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = State)
	TEnumAsByte<EDeathPose> DeathPose;
};
