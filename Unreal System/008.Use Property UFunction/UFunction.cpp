// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/UFunction.h"
#include "UdemyCpp/DebugMacros.h"

// Sets default values
AUFunction::AUFunction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUFunction::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUFunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	FVector Location = GetActorLocation();

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);
}

float AUFunction::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * SineSpeed);
}

float AUFunction::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * SineSpeed);
}
