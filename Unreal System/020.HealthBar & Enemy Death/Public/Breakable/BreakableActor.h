#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// HitInterface를 상속시키기 위함
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

// 기하학적 기능
class UGeometryCollectionComponent;

UCLASS()
class UDEMYCPP_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	// HitInterface
	// _Implementation 붙여 Native Function으로 만듦.
	virtual void GetHit_Implementation(const FVector& ImpactPoint);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Break Properties")
	float TreasureLocationZ = 75.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "Break Properties")
	// UClass* -> 모든 파일이 가능해짐
	// TSubclassOf<T> -> T에 관련된 파일만 가능하도록 함.
	// TSubclassOf<class ATreasure> TreasureClass;
	// 하나의 보물만 스폰되는 것이 아닌 여러 보물이 스폰되도록 함.
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	// 부셔져 있는지
	UPROPERTY(VisibleAnywhere)
	bool bBroken = false;
};
