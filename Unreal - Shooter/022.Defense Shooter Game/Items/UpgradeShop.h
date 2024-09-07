#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UpgradeShop.generated.h"

class AShooterCharacter;
class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class SHOOTER_API AUpgradeShop : public AActor
{
	GENERATED_BODY()
	
public:	
	AUpgradeShop();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ShopAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ShopAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop | Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MarketOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop | Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Market;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop | Area", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ShopArea;

public:	
	virtual void Tick(float DeltaTime) override;

};
