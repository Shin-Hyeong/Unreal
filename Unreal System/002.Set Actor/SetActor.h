// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SetActor.generated.h"

UCLASS()
class UDEMYCPP_API ASetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASetActor();

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector NewLocation;

	UPROPERTY()
	FVector Forward;

	UPROPERTY()
	FVector NewForward;

	UPROPERTY()
	FVector offSetLocation;

	UPROPERTY()
	FVector offSetForward;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
