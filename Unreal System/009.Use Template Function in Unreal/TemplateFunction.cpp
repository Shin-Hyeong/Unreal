// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/TemplateFunction.h"
#include "UdemyCpp/DebugMacros.h"

// Sets default values
ATemplateFunction::ATemplateFunction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATemplateFunction::BeginPlay()
{
	Super::BeginPlay();
	
	// Template Int
	int32 AvgInt = AVG<int32>(1, 3);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3 : %d"), AvgInt);

	// Template float
	float AvgFloat = AVG<float>(3.45f, 7.85f);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 3.45 and 7.86 : %.3f"), AvgFloat);
}

// Called every frame
void ATemplateFunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Time += DeltaTime;

	if (Time >= 1)
	{
		Time = 0;
		// Template FVector
		// FVector::ZeroVector -> FVector(0, 0, 0)
		// 벡터를 생성할 필요없이 사용할 수 있음
		FVector AvgVector = AVG<FVector>(GetActorLocation(), FVector::ZeroVector);
		DRAW_POINT(AvgVector);
	}

}

// Type이 + / 를 지원해야함
// FVector는 지원하지만 비슷한 FRotator는 지원하지 않음.
template <typename T>
inline T ATemplateFunction::AVG(T first, T second)
{
	return (first + second) / 2;
}
