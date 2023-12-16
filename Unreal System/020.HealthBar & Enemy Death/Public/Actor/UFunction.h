// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UFunction.generated.h"

UCLASS()
class UDEMYCPP_API AUFunction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUFunction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float Amplitude = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float SineSpeed = 5.0f;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sin값에 맞게 위치를 이동시킬 값 구하는 함수 : 진폭 크기 * FMath::Sin(RunningTime * 속도)
	// BlueprintCallable : BP에서 만든 Cpp 함수를 노드로 불러올 수 있도록 만듬.
	// BP의 이벤트 그래프에서 이벤트 진행을 연결해줘야함. (파랑색 노드)

	// BlueprintPure : BP에서 특정 이벤트를 실행하는 노드가 아닌 값을 반환하는 단순한 노드로 만듬.
	// BP의 이벤트 그래프에서 이벤트 진행을 연결하지 않아도 됨. (초록색 노드)
	UFUNCTION(BlueprintCallable)
	float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
	float TransformedCos();
};
