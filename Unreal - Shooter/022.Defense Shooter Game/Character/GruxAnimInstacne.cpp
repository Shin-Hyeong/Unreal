#include "Character/GruxAnimInstacne.h"

#include "Character/Enemy.h"

UGruxAnimInstacne::UGruxAnimInstacne()
{

}

void UGruxAnimInstacne::NativeInitializeAnimation()
{

}

void UGruxAnimInstacne::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
