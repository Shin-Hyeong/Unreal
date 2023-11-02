#include "CountDown.h"

ACountDown::ACountDown()
{
	PrimaryActorTick.bCanEverTick = false;

	CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Countdown Text"));	// 컴포넌트 생성
	CountdownText->SetHorizontalAlignment(EHTA_Center);	// 렌더링되는 텍스트를 중앙으로 정렬
	CountdownText->SetWorldSize(150.0f);	// 크기 조절.
	RootComponent = CountdownText;	// CountdownText를 RootComponent로 설정함.

	CountdownTime = 5;
	
}

void ACountDown::BeginPlay()
{
	Super::BeginPlay();

	UpdateTimerDisplay(); // 화면에 CountdownTime를 화면에 띄움.

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ACountDown::AdvanceTimer, 1.0f, true);
	// 타이머 매니저를 통해서 타이머를 실행시켜준다.
	// AdvanceTimer를 1초 마다 실행시킴.
}

void ACountDown::UpdateTimerDisplay()
{
	CountdownText->SetText(FText::FromString(FString::FromInt(FMath::Max(CountdownTime,0))));
	// SetText를 통해서 CountdownTime의 변수를 문자열로 변환하여 출력하도록 한다.
}

void ACountDown::AdvanceTimer()
{
	--CountdownTime;	// CountdownTime의 변수의 값을 줄이고
	UpdateTimerDisplay(); // 화면의 문구를 띄움.

	if (CountdownTime < 1)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		// 타이머 매니저를 호출, 타이머 핸들이 관리하고 있는 타이머를 멈추게 함.
		CountdownHasFinished(); //CountdownHasFinished 함수 호출.
	}
}

void ACountDown::CountdownHasFinished_Implementation()
{
	CountdownText->SetText(FText::FromString(TEXT("Go!")));
}
