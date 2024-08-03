#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/BulletHitInterface.h"
#include "Explosive.generated.h"

class USoundBase;
class UParticleSystem;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class SHOOTER_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	AExplosive();

	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(const FHitResult& HitResult, AActor* Shooter, AController* ShooterController) override;

protected:
	virtual void BeginPlay() override;

public:	

private:
	// 폭발물 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Sound", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ExplosiveMesh;					
															
	// 폭발 범위 - Overlap된 Actor에게 데미지를 줌			  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Area", meta = (AllowPrivateAccess = "true"))
	USphereComponent* OverlapSphere;						
															
	// 폭발 사운드											
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* ExplosiveSound;						
													
	// 폭발 효과										
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplosiveEffect;					
														
	// 폭발 데미지										
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Damage", meta = (AllowPrivateAccess = "true"))
	float Damage{ 100.f };
};
