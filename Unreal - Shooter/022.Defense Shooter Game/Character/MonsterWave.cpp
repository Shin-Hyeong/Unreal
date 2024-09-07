#include "Character/MonsterWave.h"

// Enemy
#include "Character/Enemy.h"

// BlackboardComponent
#include "BehaviorTree/BlackboardComponent.h"

// AI Controller
#include "Character/EnemyController.h"

// Player
#include "Character/ShooterCharacter.h"

// Explosive
#include "Items/Explosive.h"

// KismetSystemLibrary
#include "Kismet/KismetSystemLibrary.h"

AMonsterWave::AMonsterWave()
{
	PrimaryActorTick.bCanEverTick = true;

	// 배열 0으로 6개 채움.
	GruxNumber.Init(0, 6);
}

void AMonsterWave::BeginPlay()
{
	Super::BeginPlay();
	
	AShooterCharacter* Player = Cast<AShooterCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player)
	{
		Player->SetMonsterWave(this);
	}

	
}

void AMonsterWave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterWave::NextLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Call Next Level"));

	if (WaveSpawnCount == 0)
	{
		// 스테이중 안 터져진 폭탄 제거
		for (int32 Num = 0; Num < SpawnedExplosives.Num(); ++Num)
		{
			if (SpawnedExplosives[Num] != nullptr)
			{
				SpawnedExplosives[Num]->Destroy();
				SpawnedExplosives[Num] = nullptr;
			}
		}

		// 다음 스테이지 정보 가져와서 몬스터 스폰
		++Level;
		SetMonsterWave();
		SelectMonsterType();

		if (Explosives && ExplosiveSpawnTransform.Num() > 0)
		{
			// ExplosiveSpawnTransform 위치에 Explosive 설치
			for (int32 ExplosiveTransform = 0; ExplosiveTransform < ExplosiveSpawnTransform.Num(); ++ExplosiveTransform)
			{
				AExplosive* Explosive = GetWorld()->SpawnActor<AExplosive>(
					Explosives,
					ExplosiveSpawnTransform[ExplosiveTransform]->GetActorLocation(),
					ExplosiveSpawnTransform[ExplosiveTransform]->GetActorRotation()
				);

				SpawnedExplosives.Add(Explosive);
			}
		}
	}
}

void AMonsterWave::SetMonsterWave()
{
	if (MonsterLevelData)
	{
		// Level에 맞는 DataTabel의 Row를 가져옴
		FWaveLevel* WaveLevelRow = nullptr;
		switch (Level)
		{
		case 1:
			UE_LOG(LogTemp, Warning, TEXT("Level1"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level1"), TEXT(""));
			break;

		case 2:
			UE_LOG(LogTemp, Warning, TEXT("Level2"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level2"), TEXT(""));
			break;

		case 3:
			UE_LOG(LogTemp, Warning, TEXT("Level3"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level3"), TEXT(""));
			break;

		case 4:
			UE_LOG(LogTemp, Warning, TEXT("Level4"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level4"), TEXT(""));
			break;

		case 5:
			UE_LOG(LogTemp, Warning, TEXT("Level5"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level5"), TEXT(""));
			break;

		case 6:
			UE_LOG(LogTemp, Warning, TEXT("Level6"));
			WaveLevelRow = MonsterLevelData->FindRow<FWaveLevel>(FName("Level6"), TEXT(""));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Null"));
			WaveLevelRow = nullptr;
			break;
		}

		// DataTable Row에서 Data를 가져옴.
		if (WaveLevelRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("Get DataTable"));
			GruxNumber[0] = WaveLevelRow->DefaultGruxNumber;
			GruxNumber[1] = WaveLevelRow->HallowneenGruxNumber;
			GruxNumber[2] = WaveLevelRow->WarChiefGruxNumber;
			GruxNumber[3] = WaveLevelRow->BeetleGruxNumber;
			GruxNumber[4] = WaveLevelRow->MoltenGruxNumber;
			GruxNumber[5] = WaveLevelRow->QilinGruxNumber;
		}
	}
}

// 몬스터 타입 별로 소환함
void AMonsterWave::SelectMonsterType()
{
	// 몬스터 타입별로 순환
	for (int MonsterType = 0; MonsterType < Monsters.Num(); ++MonsterType)
	{
		// Null체크, 한 종류 Type씩 소환
		if (Monsters[MonsterType])
		{
			SpawnMonster(MonsterType);
		}
	}
}

void AMonsterWave::SpawnMonster(int MonsterType)
{
	// 입력된 횟수만큼 소환
	for (int MonsterNumber = 0; MonsterNumber < GruxNumber[MonsterType]; ++MonsterNumber)
	{
		// 일정 횟수 소환하면 다음 위치에서 소환되도록 함.
		// 소환되는 Enemy의 Collision이 서로 겹쳐서 소환이 안되는걸 막음
		if (SpawnCount > 2)
		{
			if (SpawnIdx + 1 > SpawnTransform.Num() - 1)
			{
				SpawnIdx = 0;
			}
			else
			{
				++SpawnIdx;
			}

			SpawnCount = 0;
		}
		AEnemy* Monster = GetWorld()->SpawnActor<AEnemy>(Monsters[MonsterType],
			SpawnTransform[SpawnIdx]->GetActorLocation(),
			SpawnTransform[SpawnIdx]->GetActorRotation()
		);

		++SpawnCount;
		// 현재 Level에 소환된 몬스터 수
		++WaveSpawnCount;
		// 각 몬스터에 죽었을 떄 죽음을 알릴 수 있도록 함.
		if (Monster)
		{
			Monster->SetMonsterWave(this);
		}
	}
}
