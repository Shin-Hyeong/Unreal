#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeShopSystem.generated.h"

class AShooterCharacter;
class APlayerController;
class UTextBlock;
class UButton;
class AMonsterWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotEnoughCostDelegate, int32, test);

UCLASS()
class SHOOTER_API UUpgradeShopSystem : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnClickAttackUpdateButton();

	UFUNCTION()
	void OnClickDefenseUpdateButton();

	UFUNCTION()
	void OnClickHealthUpdateButton();

	UFUNCTION()
	void OnClickNextLevel();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AMonsterWave* MonsterWave;

	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FNotEnoughCostDelegate NotEnoughCostDelegate;



	/**
		Text Variable
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Cost", meta = (AllowPrivateAccess = "true"))
	int32 NeedAttackCost{ 10 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Cost", meta = (AllowPrivateAccess = "true"))
	int32 NeedDefenseCost{ 10 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Cost", meta = (AllowPrivateAccess = "true"))
	int32 NeedHealthCost{ 10 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Level", meta = (AllowPrivateAccess = "true"))
	int32 AttackLevelInt{ 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Level", meta = (AllowPrivateAccess = "true"))
	int32 DefenseLevelInt{ 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Update | Level", meta = (AllowPrivateAccess = "true"))
	int32 HealthLevelInt{ 0 };

	/**
		Bind Widget
	*/
	// Level Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackLevel;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefenseLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthLevel;

	// Upgrade Cost Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefenseCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthCost;

	// Warning Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarningText;

	// Upgrade Button
	UPROPERTY(meta = (BindWidget))
	UButton* AttackUpGradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DefeseUpGradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HealthUpGradeButton;

	// NextLevel Button
	UPROPERTY(meta = (BindWidget))
	UButton* NextLevelButton;
};
