// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DrawDebugPoint.h"
#include "DrawDebugHelpers.h"
// #include "UdemyCpp/DebugMacros.h"

#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true)

// DrawDebugLine과 DrawDebugPoint를 한번에 매크로로 만들어서 벡터 표시 형태로 만든다.
// 다중 매크로는 \를 통해서 여러 줄로 만들 수 있다.
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f); \
		DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Yellow, true); \
	} 

// Sets default values
ADrawDebugPoint::ADrawDebugPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADrawDebugPoint::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	if (World)
	{
		DrawDebugLine(World, Location, Location + Forward * 100.f, FColor::Red, true, -1.f, 0, 1.f);

		// DrawDebugPoint는 가까이 볼수록 작아지고 멀어질 수록 커진다.
		// DrawDebugPoint는 DrawDebugLine의 화살표시 처럼 방향을 표시할 때도 사용한다.

		// UWorld, FVector Position, float size, FColor, 
		// bool PersistentLines(false), float LifeTime(-1.f), uint8 DepthPriority 
		DrawDebugPoint(World, Location + Forward * 100.f, 15.f, FColor::Red, true);
	}

	DRAW_POINT(Location + Forward * 100.f);

	DRAW_VECTOR(Location, Location + Forward * -100.f);
}

// Called every frame
void ADrawDebugPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

