// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DrawDebugLine.h"
#include "DrawDebugHelpers.h"

// 매번 함수를 생성하기 귀찮으니 매크로로 해결한다.
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f)

// Sets default values
ADrawDebugLine::ADrawDebugLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADrawDebugLine::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	if (World)
	{
		// UWorld, FVector LineStart, FVector LineEnd, FColor, bool PersistentLines(false), 
		// float LifeTime(-1.f), uint8 DepthPriority, float Thickness
		// DepthPriority : 선이 다른 선 위에 그려질 것인가, 뒤에 그려질 것인가, 0에 가까울 수록 먼저 그림.
		DrawDebugLine(World, Location, Location + Forward * 100.f , FColor::Red, true, -1.f, 0, 2.f);
	}

	DRAW_LINE(Location, Location + Forward * 200.f);
}

// Called every frame
void ADrawDebugLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

