#pragma once

//아무 것도 상속 받지 않고 빈 C++파일에 구조체를 만들어 여러 파일에서 사용할 수 있도록 만든 범용성 구조체 파일
#include "CoreMinimal.h"
#include "CustomStructs.generated.h"

class FORENHANCEDINPUT_API CustomStructs
{
public:
	CustomStructs();
	~CustomStructs();
};

USTRUCT(Atomic, BlueprintType)
//Atomic : 해당 구조체가 하나의 단위로 직렬화함을 의미,  에디터 디테일 패널에서 보여줌.
//BlueprintType : 블루프린트에서 사용될 수 있음을 의미
struct FCustomStruct
//구조체의 이름시작은 F로 시작해야함.
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* actor;
	//구조체 멤버를 포인터로 하면 얕은 복사와 깊은 복사에 유의해야 함
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 i;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float f;
};
