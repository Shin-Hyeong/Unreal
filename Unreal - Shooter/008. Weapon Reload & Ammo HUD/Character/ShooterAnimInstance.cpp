#include "Character/ShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Character/ShooterCharacter.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 해당 AnimInstacne를 가지고 있는 Pawn를 가져와서 ShooterCharacter로 캐스팅
	SetShooterCharacter(); 
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	SetShooterCharacter(); 

	SetAnimProperties();

	// 캐릭터의 전방 기준으로 좌/우를 알아냄.
	if (ShooterCharacter)
	{
		// 캐릭터가 바라는 보는 방향
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

		// 월드 좌표 X방향을 기준으로 캐릭터의 이동할려는 Rotation값 만듦
		FRotator MovementRoation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// 캐릭터의 전방 FVector와 캐릭터가 이동할려는 방향 FVector의 차이 구함.
		// NormalizedDeltaRotator : Normalized (A - B)
		// 0 : Forward / 90 : Right / -90 : Left / -180(180) : Back
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRoation, AimRotation).Yaw;

		if (ShooterCharacter->GetVelocity().Size())
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		// 조준 중인지
		bAiming = ShooterCharacter->GetAiming();
	}
}

void UShooterAnimInstance::SetShooterCharacter()
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
		if (ShooterCharacter)
			ShooterMovement = ShooterCharacter->GetCharacterMovement();
	}
}

void UShooterAnimInstance::SetAnimProperties()
{
	if (ShooterCharacter)
	{
		Speed = UKismetMathLibrary::VSizeXY(ShooterMovement->Velocity);
		bIsJumping = ShooterMovement->IsFalling();

		// GetCurrentAccleration() : Get 현재 가속도 
		if (ShooterMovement->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
	}
}