// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DrawDebugSphere.h"
// Draw Debug를 위한 헤더
#include "DrawDebugHelpers.h"

// DrawDebug 코드를 매번 적기 귀찮으니 매크로로 만든다.
// 해당 매크로는 지금 파일에서만 사용할 수 있기 때문에 헤더 파일을 생성하여 #include하여 다른 파일에도 사용할 수 있도록 만들 수 있다.
#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 50.f, 48, FColor::Blue, true)

// Sets default values
ADrawDebugSphere::ADrawDebugSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADrawDebugSphere::BeginPlay()
{
	Super::BeginPlay();
	
	// UWorld를 얻는 방법 : GetWorld();
	// GetWorld() : World Pointer를 반환. Actor가 Level에 스폰되어 있지 않으면 NULL
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();

	// Actor가 Level에 있으면 World가 값을 가지고 있음.
	if (World)
	{
		// UWorld, FVector, float Radius, int Segments, FColor, bool PersistentLines(false) ,float LifeTime
		// PersistentLines를 true로 하면 Debug Line이 사라지지 않음. false로 할 경우 유지할 시간 입력해야함.
		DrawDebugSphere(World, Location, 25.0f, 24, FColor::Red, false, 10.0f);
	}

	DRAW_SPHERE(Location);

}

// Called every frame
void ADrawDebugSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

