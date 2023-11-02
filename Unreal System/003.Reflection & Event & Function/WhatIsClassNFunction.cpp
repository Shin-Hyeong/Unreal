#include "WhatIsClassNFunction.h"

AWhatIsClassNFunction::AWhatIsClassNFunction()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWhatIsClassNFunction::BeginPlay()
{
	Super::BeginPlay();
	//Super:: : 이벤트 함수를 오버라이딩하여 사용할 때 반드시 기초 클래스의 가상 함수 Super:: 키워드로 호출해줘야 한다.
	//AActor가 가지고 있는 중요한 로직을 실행하기 위해 가상 함수 원형을 실행하고 추가적으로 필요한 내용을 구현한다.
	//Super::BeginPlay()를 호출해야 BeginPlay 단계를 정상적으로 실행 할 수 있다.
	
}

void AWhatIsClassNFunction::EndPlay(EEndPlayReason::Type)
{

}

void AWhatIsClassNFunction::PostInitializeComponents()
{

}

void AWhatIsClassNFunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

