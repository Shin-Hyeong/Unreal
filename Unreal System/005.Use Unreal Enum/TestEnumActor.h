#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// 직접 만듬 Enum를 사용하기 위해 헤더 파일 전처리
#include "CustomEnum.h"
#include "TestEnumActor.generated.h"

UCLASS()
class FORENHANCEDINPUT_API ATestEnumActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnumActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// UENUM 사용하기
	UPROPERTY(EditAnywhere)
	EMyColor MyColorTestEnum;
};
