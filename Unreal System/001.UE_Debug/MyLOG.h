#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyLOG.generated.h"

UCLASS()
class FORENHANCEDINPUT_API AMyLOG : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyLOG();

UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
