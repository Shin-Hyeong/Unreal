// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Component.h"

// Sets default values
AComponent::AComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 객체 생성 <Type>(TEXT(내부 이름))
	// CreateDefaultSubobject : 단순히 객체를 구현하는 함수 (팩토리), <Type>의 포인터를 반환함.
	// 일반적으로 C++로 Component를 생성하고 Mesh 설정은 BP에서 하는 것이 좋다.
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));

	// ItemMesh를 RootComponent로 지정함.
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

