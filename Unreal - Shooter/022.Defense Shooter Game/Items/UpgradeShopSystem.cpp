#include "Items/UpgradeShopSystem.h"

// Player
#include "Character/ShooterCharacter.h"

// BlueprintLibrary
#include "Blueprint/WidgetBlueprintLibrary.h"

// Text
#include "Components/TextBlock.h"

// Button
#include "Components/Button.h"

// Weapon
#include "Items/Weapon.h"

// MonsterWave
#include "Character/MonsterWave.h"

void UUpgradeShopSystem::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = World->GetFirstPlayerController();
		Player = Cast<AShooterCharacter>(PlayerController->GetPawn());
		MonsterWave = Player->GetMonsterWave();

		// PlayerController로 게임에 대한 입력만 가능 UI요소는 무시
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	}

	WarningText->SetRenderOpacity(0.0f);

	// Set Level
	AttackLevel->SetText(FText::FromString(FString::FromInt(AttackLevelInt)));
	DefenseLevel->SetText(FText::FromString(FString::FromInt(DefenseLevelInt)));
	HealthLevel->SetText(FText::FromString(FString::FromInt(HealthLevelInt)));
	// Set Cost
	AttackCost->SetText(FText::FromString(FString::FromInt(NeedAttackCost)));
	DefenseCost->SetText(FText::FromString(FString::FromInt(NeedDefenseCost)));
	HealthCost->SetText(FText::FromString(FString::FromInt(NeedHealthCost)));

	// Delegate
	AttackUpGradeButton->OnClicked.AddDynamic(this, &UUpgradeShopSystem::OnClickAttackUpdateButton);
	DefeseUpGradeButton->OnClicked.AddDynamic(this, &UUpgradeShopSystem::OnClickDefenseUpdateButton);
	HealthUpGradeButton->OnClicked.AddDynamic(this, &UUpgradeShopSystem::OnClickHealthUpdateButton);
	NextLevelButton->OnClicked.AddDynamic(this, &UUpgradeShopSystem::OnClickNextLevel);
}

void UUpgradeShopSystem::OnClickAttackUpdateButton()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickAttackUpdateButton"));
	if (Player)
	{
		int32 PlayerCoin = Player->GetCoin();

		if (PlayerCoin <= NeedAttackCost)
		{
			NotEnoughCostDelegate.Broadcast(0);
			return;
		}
		
		// Coin 지불
		Player->SetCoin(PlayerCoin - NeedAttackCost);

		// 다음 Cost로 변경
		NeedAttackCost = static_cast<int32>(NeedAttackCost * 2.0f);
		AttackCost->SetText(FText::FromString(FString::FromInt(NeedAttackCost)));

		// AttackLevel TEXT +1
		++AttackLevelInt;
		AttackLevel->SetText(FText::FromString(FString::FromInt(AttackLevelInt)));

		// Player의 공격력 증가
		Player->AddUpgradeDamage(4.0f);
	}
}

void UUpgradeShopSystem::OnClickDefenseUpdateButton()
{
	int32 PlayerCoin = Player->GetCoin();

	if (PlayerCoin <= NeedDefenseCost)
	{
		NotEnoughCostDelegate.Broadcast(0);
		return;
	}

	// Coin 지불
	Player->SetCoin(PlayerCoin - NeedDefenseCost);

	// 다음 Cost로 변경
	NeedDefenseCost = static_cast<int32>(NeedDefenseCost * 2.0f);
	DefenseCost->SetText(FText::FromString(FString::FromInt(NeedDefenseCost)));

	// DefenseLevel TEXT +1
	++DefenseLevelInt;
	DefenseLevel->SetText(FText::FromString(FString::FromInt(DefenseLevelInt)));

	// Player의 방어력 증가
	Player->AddDefensive(1.0f);
}

void UUpgradeShopSystem::OnClickHealthUpdateButton()
{
	int32 PlayerCoin = Player->GetCoin();

	if (PlayerCoin <= NeedHealthCost)
	{
		NotEnoughCostDelegate.Broadcast(0);
		return;
	}

	// Coin 지불
	Player->SetCoin(PlayerCoin - NeedHealthCost);

	// 다음 Cost로 변경
	NeedHealthCost = static_cast<int32>(NeedHealthCost * 2.0f);
	HealthCost->SetText(FText::FromString(FString::FromInt(NeedHealthCost)));

	// HealthLevel TEXT +1
	++HealthLevelInt;
	HealthLevel->SetText(FText::FromString(FString::FromInt(HealthLevelInt)));

	// Player의 최대체력 증가
	Player->AddMaxHealth(25.f);
}

void UUpgradeShopSystem::OnClickNextLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Call Start Button"));
	if (MonsterWave)
	{
		MonsterWave->NextLevel();
		UE_LOG(LogTemp, Warning, TEXT("Spawn Monster"));
	}
	if (Player)
	{
		// 체력 회복
		float PlayerMaxHealth = Player->GetMaxHealth();
		float PlayerHealth = Player->GetHealth();
		// HP 100 회복 / 최대 HP 이상으로 회복되지 않도록 함.
		if (PlayerMaxHealth <= PlayerHealth + 100.f)
		{
			Player->SetHealth(PlayerMaxHealth);
		}
		else
		{
			Player->AddHealth(100.f);
		}
		// 총알 보충
		Player->InitializeAmmoMap();

		// Shop 닫기
		Player->SelectButtonPressed();
	}
}
