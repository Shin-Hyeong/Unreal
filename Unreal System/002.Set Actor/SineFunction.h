// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SineFunction.generated.h"

UCLASS()
class UDEMYCPP_API ASineFunction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASineFunction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float Amplitude = 0.25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine")
	float SineSpeed = 5.0f;

	float RunningTime = 0.f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
