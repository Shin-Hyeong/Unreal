// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SineFunction.h"
#include "UdemyCpp/DebugMacros.h"

// Sets default values
ASineFunction::ASineFunction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASineFunction::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASineFunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector Location = GetActorLocation();

	// Sin : double Value
	float DeltaZ = Amplitude *  FMath::Sin(RunningTime * SineSpeed);

	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

}

