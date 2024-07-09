#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	// 조준
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	// 비 조준
	EOS_Hip UMETA(DisplayName = "Hip"),
	// 재 장전
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	// 공중
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefautMax"),
};

UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UShooterAnimInstance();

	/* <AnimInstance> */
	virtual void NativeInitializeAnimation() override;
	/* </AnimInstance> */
	
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	

protected:
	// 제자리 회전을 위한 값 계산
	void TurnInPlace();

	// 이동 중 화면 회전 속도
	void Lean(float DeltaTime);

private:
	void SetShooterCharacter();
	void SetAnimProperties();
	void SetCharacterMovementOffset();

private:
	/**
		ShooterCharacter Binding
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* ShooterMovement;
	
	/**
		Proprties
	*/
	// 캐릭터 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed{ 0.f };

	// 캐릭터가 점프 중 인지 (공중에 있는지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping{ false };

	// 캐릭터가 이동중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating{ false };

	// 조준 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming{ false };

	// 재장전 중일 동안 true, 재장전 모션 중에는 AimOffset이 되지 않도록 함.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading{ false };

	// 웅크리고 있을 때 true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Crouching", meta = (AllowPrivateAccess = "true"))
	bool bCrouching{ false };

	// 무기 교체주이면 true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipping", meta = (AllowPrivateAccess = "true"))
	bool bEquipping{ false };

	// 상체 Blend Alpha값을 제어 - 제자리 회전 중 총을 못쏘도록 함. 등
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight{ 1.0f };

	/**
		Character Movement Yaw
	*/
	// 캐릭터가 바라보는 방향 구하기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw{ 0.f };

	// 캐릭터가 멈출 때 모션을 얻기 위해 저장용 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw{ 0.f };

	/**
		Character Turn in place
	*/
	// 제자리에서 회전 중인지 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Crouching", meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace{ false };

	// 가만히 있을 때, 캐릭터의 현재 프레임의 Yaw
	float TIPCharacterYaw{ 0.f };

	// 가만히 있을 때, 캐릭터의 이전(직전) 프레임의 Yaw
	float TIPCharacterYawLastFrame{ 0.f };

	// Root Bone의 Yaw
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset{ 0.f };

	// 현재 Frame의 RatationCurve 값
	float RotationCurveValue{ 0.0f };
	
	// 이전(직전) Frame의 RotationCurve 값
	float RotationCurveValueLastFrame{ 0.f };

	// Aim이 바라보고 있는 Pitch값(위 아래), AimOffset에서 사용함.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch{ 0.f };

	// AimOffset 상태를 결정하는데 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState{ EOffsetState::EOS_Hip };

	/**
		Lean in Running
	*/
	// 움직이고 있을 때 캐릭터 현재 프레임의 Rotator
	FRotator CharacterRotation{ FRotator(0.f) };

	// 움직이고 있을 때 캐릭터 이전(직전) 프레임의 Rotator
	FRotator CharacterRotationLastFrame{ FRotator(0.f) };

	// BlendSpace에서 사용할 수 있는 이동 중 화면 좌우회전 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Lean", meta = (AllowPrivateAccess = "true"))
	float DeltaYaw{ 0.f };
};
