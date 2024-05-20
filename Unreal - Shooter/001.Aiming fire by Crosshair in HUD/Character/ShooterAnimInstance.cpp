#include "Character/ShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Character/ShooterCharacter.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// 해당 AnimInstacne를 가지고 있는 Pawn를 가져와서 ShooterCharacter로 캐스팅
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	if (ShooterCharacter)
		ShooterMovement = ShooterCharacter->GetCharacterMovement();
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
		if (ShooterCharacter)
			ShooterMovement = ShooterCharacter->GetCharacterMovement();
	}

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
