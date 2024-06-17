#include "Character/ShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Character/ShooterCharacter.h"

UShooterAnimInstance::UShooterAnimInstance()
{

}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 해당 AnimInstacne를 가지고 있는 Pawn를 가져와서 ShooterCharacter로 캐스팅
	SetShooterCharacter(); 
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// Shooter Character 인식
	SetShooterCharacter(); 

	// 이동중, 점프중, 조준중 확인
	SetAnimProperties();

	// 캐릭터의 전방 기준으로 좌/우를 알아냄.
	SetCharacterMovementOffset();

	// 제자리에서 화면이동에 따라 캐릭터 제자리 회전
	TurnInPlace();

	// 이동 중 캐릭터의 화면 회전 속도
	Lean(DeltaTime);
}


void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsJumping)
	{
		// 제자리 이동할 필요가 없음
		RootYawOffset = 0;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveValue = 0.f;
		RotationCurveValueLastFrame = 0.f;
	}
	else
	{
		// 이전 프레임에 가지고 있던 Yaw값을 넘기고
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		// 새로 갱신된 Yaw값을 받음
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		// 이전 프레임과의 Yaw 차이
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		// RootYawOffset를 -180 ~ 180으로 고정시킴
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// Truning Curve는 Animation 진행중엔 계속 1의 값을 가짐.
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		// Turn Animation이 진행중이면 1, 진행 중이지 않으면 0
		if (Turning > 0)
		{
			RotationCurveValueLastFrame = RotationCurveValue;
			RotationCurveValue = GetCurveValue(TEXT("Rotation")); // -90 -> 0
			const float DeltaRotation{ RotationCurveValue - RotationCurveValueLastFrame };

			// RootYawOffset > 0 = 좌회전
			// RootYawOffset < 0 = 우회전
			// 회전하는 Animation를 통해서 부드럽게 Root Bone Yaw를 회전하여 RootYawOffset를 0으로 만듦.
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation /* Turning Left */ : RootYawOffset += DeltaRotation /* Turning Right */;

			// Abs = 절대값
			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			// RootYawOffset이 절대값 90을 넘는 값을 갖게 되면 절대값 90이 되도록 함
			if (ABSRootYawOffset > 90.f)
			{
				// 90을 넘는 여분, ex) 120 - 90 = 30
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

	// DeltaTime은 작은 값(1보다 작은 값)이기 때문에 곱하면 더 작은 수가 나오기 떄문에 나눈다.
	const double Target{ Delta.Yaw / DeltaTime };

	// DeltaYaw에서 Target까지 DeltaTime마다 6.0f 속도로 보간.
	const double Interp{ FMath::FInterpTo(DeltaYaw, Target, DeltaTime, 6.0f) };

	DeltaYaw = FMath::Clamp(Interp, -90.f, 90.f);

	if (GEngine) GEngine->AddOnScreenDebugMessage(
		1,
		-1,
		FColor::Cyan,
		FString::Printf(TEXT("DeltaYaw : %f"), DeltaYaw)
	);
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
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

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

void UShooterAnimInstance::SetCharacterMovementOffset()
{
	if (ShooterCharacter)
	{
		// 캐릭터가 바라는 보는 방향
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

		// 월드 좌표 X방향을 기준으로 캐릭터의 이동할려는 Rotation값 만듦
		FRotator MovementRoation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		// 캐릭터의 전방 FRotator와 캐릭터가 이동할려는 방향 FRotator의 차이 구함.
		// NormalizedDeltaRotator : Normalized (A - B)
		// 0 : Forward / 90 : Right / -90 : Left / -180(180) : Back
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRoation, AimRotation).Yaw;

		// 캐릭터의 마지막 이동 방향(캐릭터가 멈추기 전 이동하고 있던 방향)
		if (ShooterCharacter->GetVelocity().Size())
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		// 조준 중인지
		bAiming = ShooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsJumping)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (bAiming)
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}
}