#pragma once

#include "CoreMinimal.h"
#include "CustomEnum.generated.h"

// Enum을 블루프린터에서도 사용할 수 있도록 함.
UENUM(BlueprintType)
// 언리얼에서는 표준 Enum 형식이 아닌 enum class 형식으로 만들어야 한다.
// UENUM은 uint 형태만 지원함. 열거형의 타입을 uint8로 지정함.
enum class EMyColor : uint8
{
	// UMETA(DisplayName = "이름") 을 통해 에디터에서 열거자들의 이름을 알려준다.
	Red UMETA(DisplayName = "RED"),
	Green UMETA(DisplayName = "GREEN"),
	Blue UMETA(DisplayName = "BLUE")
};

class FORENHANCEDINPUT_API CustomEnum
{
public:
	CustomEnum();
	~CustomEnum();
};
