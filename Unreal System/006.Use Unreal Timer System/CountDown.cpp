#include "CountDown.h"

ACountDown::ACountDown()
{
	PrimaryActorTick.bCanEverTick = false;

	CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Countdown Text"));	// ������Ʈ ����
	CountdownText->SetHorizontalAlignment(EHTA_Center);	// �������Ǵ� �ؽ�Ʈ�� �߾����� ����
	CountdownText->SetWorldSize(150.0f);	// ũ�� ����.
	RootComponent = CountdownText;	// CountdownText�� RootComponent�� ������.

	CountdownTime = 5;
	
}

void ACountDown::BeginPlay()
{
	Super::BeginPlay();

	UpdateTimerDisplay(); // ȭ�鿡 CountdownTime�� ȭ�鿡 ���.

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ACountDown::AdvanceTimer, 1.0f, true);
	// Ÿ�̸� �Ŵ����� ���ؼ� Ÿ�̸Ӹ� ��������ش�.
	// AdvanceTimer�� 1�� ���� �����Ŵ.
}

void ACountDown::UpdateTimerDisplay()
{
	CountdownText->SetText(FText::FromString(FString::FromInt(FMath::Max(CountdownTime,0))));
	// SetText�� ���ؼ� CountdownTime�� ������ ���ڿ��� ��ȯ�Ͽ� ����ϵ��� �Ѵ�.
}

void ACountDown::AdvanceTimer()
{
	--CountdownTime;	// CountdownTime�� ������ ���� ���̰�
	UpdateTimerDisplay(); // ȭ���� ������ ���.

	if (CountdownTime < 1)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		// Ÿ�̸� �Ŵ����� ȣ��, Ÿ�̸� �ڵ��� �����ϰ� �ִ� Ÿ�̸Ӹ� ���߰� ��.
		CountdownHasFinished(); //CountdownHasFinished �Լ� ȣ��.
	}
}

void ACountDown::CountdownHasFinished_Implementation()
{
	CountdownText->SetText(FText::FromString(TEXT("Go!")));
}
