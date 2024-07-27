#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/BulletHitInterface.h"
#include "Explosive.generated.h"

class USoundBase;
class UParticleSystem;

UCLASS()
class SHOOTER_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	AExplosive();

	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(const FHitResult& HitResult) override;

protected:
	virtual void BeginPlay() override;

public:	

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit | Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* ExplosiveSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplosiveEffect;
};
