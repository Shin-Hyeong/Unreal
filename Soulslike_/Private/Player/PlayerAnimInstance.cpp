#include "Player/PlayerAnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Player/PlayableCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayableCharacter = Cast<APlayableCharacter>(TryGetPawnOwner());

	if (PlayableCharacter)
	{
		PlayerMovement = PlayableCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PlayerMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerMovement->Velocity);
		MovementValue = PlayableCharacter->GetMovementValue();
		GroundSpeedX = GroundSpeed * MovementValue.X;
		GroundSpeedY = GroundSpeed * MovementValue.Y;

		IsFalling = PlayerMovement->IsFalling();
		CharacterWeaponState = PlayableCharacter->GetCharacterState();
		CharacterState = PlayableCharacter->GetPlayerState();
		DeathPose = PlayableCharacter->GetDeathPose();
	}
}

