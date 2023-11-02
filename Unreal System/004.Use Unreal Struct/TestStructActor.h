// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomStructs.h" //여러 곳에서 사용할 수 있도록 만든 범용성 구조체 헤더 파일
#include "TestStructActor.generated.h"

UCLASS()
class FORENHANCEDINPUT_API ATestStructActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestStructActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FCustomStruct CustomStruct;
};
