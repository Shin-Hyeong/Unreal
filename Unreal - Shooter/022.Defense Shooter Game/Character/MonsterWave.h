#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterWave.generated.h"

class AEnemy;
class UDataTable;
class AExplosive;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameClear, bool, Clear);

USTRUCT(BlueprintType)
struct FWaveLevel : public FTableRowBase
{
	GENERATED_BODY()

	// Level당 기본 Grux 스폰 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DefaultGruxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HallowneenGruxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WarChiefGruxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BeetleGruxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MoltenGruxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QilinGruxNumber;
};

UCLASS()
class SHOOTER_API AMonsterWave : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterWave();

	virtual void Tick(float DeltaTime) override;

	void NextLevel();

protected:
	virtual void BeginPlay() override;

	void SetMonsterWave();

	UFUNCTION()
	void SelectMonsterType();

	void SpawnMonster(int MonsterType);

private:
	// 몬스터 웨이브에서 소환할 몬스터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave | Monter", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AEnemy>> Monsters;

	// 몬스터 웨이브 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave | Spawn", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> SpawnTransform;

	// 몬스터 일정 스폰 하면 다음 스폰 Idx로 옮김.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave| Spawn", meta = (AllowPrivateAccess = "true"))
	int32 SpawnCount{ 0 };

	// 몬스터 스폰 위치 Idx
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave| Spawn", meta = (AllowPrivateAccess = "true"))
	int32 SpawnIdx{ 0 };

	// 현재 Wave Level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave | Level", meta = (AllowPrivateAccess  = "true"))
	int32 Level{ 0 };

	// SpawnMonsterCount
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave | Level", meta = (AllowPrivateAccess = "true"))
	int32 WaveSpawnCount{ 0 };

	// Monster Wave Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave | DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* MonsterLevelData;

	/**
		소환할 몬스터 수
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave | Spawn", meta = (AllowPrivateAccess = "true"))
	TArray<int32> GruxNumber; 

	/**
		Explosive
	*/
	// Level이 시작되면 스폰될 폭탄들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave | Explosive", meta  = (AllowPrivateAccess = "true"))
	TSubclassOf<AExplosive> Explosives;

	// 폭탄이 스폰될 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave | Explosive", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ExplosiveSpawnTransform;

	// 스폰한 폭탄 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave | Explosive", meta = (AllowPrivateAccess = "true"))
	TArray<AExplosive*> SpawnedExplosives;

	// Game Clear
	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FGameClear GameClear;

public:	
	FORCEINLINE void SubSpawnCount(int32 Amount) { WaveSpawnCount -= Amount; }
	FORCEINLINE TArray<AExplosive*> GetSpawnedExplosives() { return SpawnedExplosives; }

};
