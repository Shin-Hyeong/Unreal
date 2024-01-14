#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerOverlay;

UCLASS()
class UDEMYCPP_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() const { return PlayerOverlay; }

protected:
	/** <AAcotr> */
	virtual void BeginPlay() override;
	/** </AAcotr> */


private:
	UPROPERTY(EditDefaultsOnly, Category = Player)
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;
};
