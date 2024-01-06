// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TemplateFunction.generated.h"

UCLASS()
class UDEMYCPP_API ATemplateFunction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATemplateFunction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Time = 0;

	template<typename T>
	T AVG(T first, T second);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

