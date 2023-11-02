#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h" //UTextRenderComponent 사용하기 위한 전처리
#include "CountDown.generated.h"

UCLASS()
class FORENHANCEDINPUT_API ACountDown : public AActor
{
	GENERATED_BODY()
	
public:	
	ACountDown();

protected:
	virtual void BeginPlay() override;

public:	
	// How long, in seconds, the Countdown will run
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CountDown")
	int32 CountdownTime;
	// 앞에 주석을 붙이면 에디터에 마우스를 가져다 대면 설명이 뜸. 한글로 하면 깨짐.

	UTextRenderComponent* CountdownText;

	void UpdateTimerDisplay();
	
	// 타이머 사용하기
	// Tick함수에서 DeltaTime 사용해도 되지만 지속적으로 실행될 필요 없이 특정 순간에 몇번만 실행되면 되거나
	// 시간의 텀이 1초를 넘길 경우 DeltaTime를 사용하기 보다 타이머를 사용하는게 좋다.

	void AdvanceTimer(); // 타이머가 돌아가면서 호출될 함수

	UFUNCTION(BlueprintNativeEvent) // 함수를 블루프린트에서 사용 할 수 있도록 만듬.
	// BlueprintCallable : 블루프린트에서 함수를 호출 할 수 있도록함. C++에서만 함수 변경 가능. 블루프린트에서 변경이나 덮어쓰기가 불가능.
	// BlueprintImplementableEvent : 선언은 C++에서 하지만 함수의 본문은 블루프린터에서 작성함.
	// BlueprintNativeEvent : BlueprintCallable, BlueprintImplementableEvent를 조합한 것.
	// 기본적인 작동방식은 C++로 작성하지만 블루프린터에서 덮어쓰고 변경이 가능하다.
	// C++코드는 함수 뒤에 _Implementation를 붙여서 가상 함수를 만든다.
	// 블루프린터에서 CountdownHasFinished() 이벤트를 호출하면 CountdownHasFinished()를 호출함
	// C++로 구현한 CountdownHasFinished() 이벤트를 호출하면 CountdownHasFinished_Implementation()를 호출함.
	// 블루프린터에서 CountdownHasFinished() 이벤트를 우클릭해서 부모 이벤트를 호출하면 CountdownHasFinished_Implementation()를 호출 할 수 있음.

	void CountdownHasFinished(); // AdvanceTimer 함수가 타이머가 지정한 만큼 실행되면 마무리하는 함수
	virtual void CountdownHasFinished_Implementation();
	// virtual : 해당 함수를 부모로 하여 가상 함수를 만들 수 있도록 만듦.

	FTimerHandle CountdownTimerHandle;	// 타이머를 컨트롤(핸들)하기 위한 구조체, 타이머가 종료 후 타이머가 계속 돌아가지 않도록 종료하기 위해서 필요함.
};
