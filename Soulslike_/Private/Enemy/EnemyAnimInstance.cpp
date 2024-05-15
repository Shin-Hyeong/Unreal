#include "Enemy/EnemyAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Enemy/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());

	if (EnemyCharacter)
	{
		EnemyMovement = EnemyCharacter->GetCharacterMovement();
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EnemyMovement)
	{
		EnemySpeed = UKismetMathLibrary::VSizeXY(EnemyMovement->Velocity);

		EnemyState = EnemyCharacter->GetEnemyState();
		DeathPose = EnemyCharacter->GetDeathPose();
	}
}
