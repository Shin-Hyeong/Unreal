#include "WhatIsClassNFunction.h"

AWhatIsClassNFunction::AWhatIsClassNFunction()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWhatIsClassNFunction::BeginPlay()
{
	Super::BeginPlay();
	//Super:: : �̺�Ʈ �Լ��� �������̵��Ͽ� ����� �� �ݵ�� ���� Ŭ������ ���� �Լ� Super:: Ű����� ȣ������� �Ѵ�.
	//AActor�� ������ �ִ� �߿��� ������ �����ϱ� ���� ���� �Լ� ������ �����ϰ� �߰������� �ʿ��� ������ �����Ѵ�.
	//Super::BeginPlay()�� ȣ���ؾ� BeginPlay �ܰ踦 ���������� ���� �� �� �ִ�.
	
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

