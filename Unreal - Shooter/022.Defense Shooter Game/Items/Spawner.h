#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

class UBoxComponent;
class AEnemy;
class AExplosive;

UCLASS()
class SHOOTER_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawner();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SpawnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SpawnTargetActor();

protected:
	virtual void BeginPlay() override;

private:
	// Root
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Root", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Root;

	/**
		Spawn
	*/
	// 스폰 허용 Area
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SpawnOrderArea;

	// 스폰 시킬 Enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn | Enemy", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AEnemy>> SpawnActors;

	// Enemy 스폰 시킬 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn | Enemy", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> EnemySpawnTransform;

	// 스폰 시킬 Explosive
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn | Explosive", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AExplosive> SpawnExplosive;

	// Explosive 스폰 시킬 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn | Explosive", meta = (AllowPrivateAccess = "true"))
	AActor* ExplosiveSpawnTransform;

public:	

};
