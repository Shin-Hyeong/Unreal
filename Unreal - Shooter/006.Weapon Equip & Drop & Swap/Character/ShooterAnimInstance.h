#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterMovementComponent;

UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/* <AnimInstance> */
	virtual void NativeInitializeAnimation() override;
	/* </AnimInstance> */
	
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	void SetShooterCharacter();
	void SetAnimProperties();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* ShooterMovement;
	
	// 캐릭터 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	// 캐릭터가 점프 중 인지 (공중에 있는지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping;

	// 캐릭터가 이동중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// 캐릭터가 바라보는 방향 구하기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// 캐릭터가 멈출 때 모션을 얻기 위해 저장용 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	// 조준 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming{false};
};
