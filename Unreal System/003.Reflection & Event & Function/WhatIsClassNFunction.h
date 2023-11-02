#pragma once
// 현재 소스 파일이 컴파일을 한번만 하도록 만든다.
// 여러 파일이 해당 파일을 포함시켜 읽어드릴때 사용하여 여러번 컴파일 하지 않고 한번만 컴파일하도록 함.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhatIsClassNFunction.generated.h" //"모듈명.generated.h" -> 리플렉션을 가지게 하려면 해당 헤더를 포함해야 함.
// 해당 헤더를 가져야 리플레션이 있는 시스템에서 해당 클래스를 고려해야 하고 시스템 구현이 필요함을 언리얼 헤더툴 UHT(Unreal Header Tool)에 알린다
// 컴파일하기 이전에 UHT를 사용해 클래스 선언 내의 리플렉션을 위한 매크로 및 키워드를 분석한다.
// 분석한 결과에 따라 언리얼 실행 환경에 필요한 부가 정보를 해당 generated.h 파일에 생성한다.
// 생성된 generated.h헤더 파일과 함께 소스코드를 컴파일하면 언리얼 실행 환경은 이를 사용해 해당 언리얼 오브젝트를 관리하고 에디터에서 해당 값을 편집할 수 있는 인터페이스를 제공한다.
// generated.h 헤더를 포함한 클래스는 UCLASS()와 GENERATED_BODY() 매크로를 반드시 포함해야 한다.

// UCLASS() & GENERATED_BODY(), UENUM(), USTRCUT(), UFUNCTION(), UPROPERTY()
// 리플렉션 기능을 활성화 시키는 매크로
// 오브젝트가 리플렉션을 가지면 디테일 패널, 직렬화, 가비지 콜렉션, 네트워크 리플리케이션, 블루프린트 등 언리얼 엔진의 유용한 시스템 사용 가능
// BeginPlay(), UE_LOG()등 기능 사용할 수 있도록 함.
// UObject() : Super:: 와 같은 가상 이벤트 함수 호출 할때도  UObject가 된 클래스만 사용할 수 있다록 되어있음.

UCLASS()
// UCLASS() 괄호에 특수한 기능을 하는 지정자를 삽입할 수 있다.
// 지정자 : https://docs.unrealengine.com/4.27/ko/ProgrammingAndScripting/GameplayArchitecture/Classes/Specifiers/
class FORENHANCEDINPUT_API AWhatIsClassNFunction : public AActor
{
	GENERATED_BODY()
	//언리얼 오브젝트로 만든 클래스가 언리얼 시스템을 위한 추가적인 함수 
	//Super:: 또는 ThisClass::와 같은 유용한 typedef를 사용 할 수 있게 해준다.
	
public:	
	AWhatIsClassNFunction();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type) override;
	// Type의 종류 : Destoryed, LevelTransition, EndPlayInEditor, RemovedFromWorld, Quit
	// Type의 종류에 맞게 호출되며 호출될때 인자를 받아온다.
	virtual void PostInitializeComponents() override;
	// 게임이 실행됬을 때 레벨에 배치된 액터가 작동하기 위한 컴포넌트 세팅이 완료되었을 때 호출하는 함수
	// 이 함수가 호출된 시점에 액터가 스폰되고, 액터가 스폰된 후 BeginPlay()를 호출한다.

public:	
	virtual void Tick(float DeltaTime) override;
	// 기초 클래스는 virtual 키워드를, 파생 클래스에는 override 키워드를 추가한다.
	// 파생 클래스에 virtual 키워드까지 포함하는 이유는 혹시  또 다른 클래스가 파생 클래스를 상속하게 되었을 때 가상 이벤트 함수를 호출 하는 것을 대비하기 위함.
	// WhatIsClassNFunction 클래스는 AActor 클래스를 상속 받고, AActor 클래스에 위 코드에 있는 BeginPlay(), Tick() 등의 이벤트 함수가 가상 함수로 정의되어 있다.
	// 따라서 WhatIsClassNFunction클래스에서 해당 함수들을 오버라이딩해서 사용한다.

};
// 이벤트 함수 접근 지시자
// BeginPlay() : 액터가 직접 이 함수을 호출 하기 때문
// TIck() : 언리얼 엔진에서 리스트를 관리하여 호출하기 때문.