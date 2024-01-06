// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Component.generated.h"

UCLASS()
class UDEMYCPP_API AComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// StaticMesh를 주소를 가질 포인트 변수
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
};
