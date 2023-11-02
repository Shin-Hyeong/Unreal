#pragma once

//�ƹ� �͵� ��� ���� �ʰ� �� C++���Ͽ� ����ü�� ����� ���� ���Ͽ��� ����� �� �ֵ��� ���� ���뼺 ����ü ����
#include "CoreMinimal.h"
#include "CustomStructs.generated.h"

class FORENHANCEDINPUT_API CustomStructs
{
public:
	CustomStructs();
	~CustomStructs();
};

USTRUCT(Atomic, BlueprintType)
//Atomic : �ش� ����ü�� �ϳ��� ������ ����ȭ���� �ǹ�,  ������ ������ �гο��� ������.
//BlueprintType : �������Ʈ���� ���� �� ������ �ǹ�
struct FCustomStruct
//����ü�� �̸������� F�� �����ؾ���.
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* actor;
	//����ü ����� �����ͷ� �ϸ� ���� ����� ���� ���翡 �����ؾ� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 i;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float f;
};
