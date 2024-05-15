#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "Exp.generated.h"

class UNiagaraSystem;

UCLASS()
class SOULSLIKE__API AExp : public ABaseItem
{
	GENERATED_BODY()

public:
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	FORCEINLINE int32 GetExp() const { return Exp; }
	FORCEINLINE void SetExp(int32 AmountOfExp) { Exp = AmountOfExp; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	int32 Exp;

	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	bool bShowLineTrace = false;

	// 땅으로 부터 떠 있을 높이
	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	float ExpLocationZ = 50.f;

	// 떨어지는 도착 지점 = 땅 높이 + ExpLocationZ
	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	double DesiredZ;

	// 떨어지는 속도
	UPROPERTY(EditAnywhere, Category = "Exp Properties")
	float DriftRate = -30.f;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* GetExpEffect;
};
