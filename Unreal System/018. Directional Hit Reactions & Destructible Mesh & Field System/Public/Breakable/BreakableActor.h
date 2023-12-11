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

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

};
