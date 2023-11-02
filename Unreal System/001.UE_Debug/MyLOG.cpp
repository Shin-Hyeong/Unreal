#include "MyLOG.h"


AMyLOG::AMyLOG()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize time
	time = 0;

}

void AMyLOG::BeginPlay()
{
	Super::BeginPlay();

	// 출력 로그창에 출력하기
	// 로그 카테고리, 로그 상세 수준, 로그로 남길 메시지
	UE_LOG(LogTemp, Log, TEXT("Log Message")); 

	// 로그 카테고리 - 출력된 로그가 어떤 시스템에서 출력되었는지 표시함.
	// LogTemp : 특정한 카테고리에 속하지고 않고 임시로 띄우는 카테고리
	// Log를 치고 자동완성으로 다양한 종류(90여 개)의 카테고리를 볼 수 있다.

	// 사용자가 카테고리를 직접 만들 수 있다.
	// 프로젝트 이름으로 된 헤더파일(.h)아 cpp 파일을 찾는다.
	// 헤더파일에서 DECLARE_LOG_CATEGORY_EXTERN(카테고리 이름, 로그 상세 수준, 컴파일 타입의 로그 상세 수준)을 입력한다.
	// DECLARE_LOG_CATEGORY_EXTERN(MyLogCategory, Log, All);
	// Cpp파일에서 DECLARE_LOG_CATEGORY(카테고리 이름)을 입력한다.
	// DECLARE_LOG_CATEGORY_EXTERN(MyLogCategory);
	// 로그를 사용할려면 사용할 파일에 프로그램 이름으로 된 헤더파일을 include한다.
	// #include "ProjectFileName.h"
	// UE_LOG(MyLogCategory, Log, TEXT("My Category Log"));

	// 로그 상세 수준 - 로그가 출력되는 중요도를 나타냄. 중요한 내용만 보이게 하거나 상세적으로 보이도록 함.
	// Fatal : 가장 치명적인 수준. Fatal이 실행되면 Crash Reporter를 띄움. 로그로 항상 콘솔 및 로그 파일에 출력되며 로그가 비활성화된 상태에서도 모든 작동을 중단.
	// Error : 콘솔 및 로그 파일에 출력되며 빨간색으로 표시.
	// Warning : 콘솔 및 로그 파일에 출력되며 로그는 노란색으로 표시.
	// Display : 콘솔 및 로그 파일에 출력.
	// Log : 로그 파일에는 출력되지만 게임 내의 콘솔에서는 출력되지 않음. 언리얼 에디터의 출력 로그 패널에서는 계속 출력.
	// Verbose : 로그 파일에는 출력되지만 게임 내의 콘솔에서는 출력되지 않음. 일반적으로 자세한 로깅 및 디버깅에 사용.
	// VeryVerbose : 로그 파일에는 출력되지만 게임 내의 콘솔에서는 출력되지 않음. 대량의 로그를 출력하는 상세한 로깅에 사용. 
	// 주로 Error, Warning, Display를 사용함.

	// 로그 메시지 - 고정된 메시지가 아닌 변수를 사용해 값이 변할 수 있게 할 수 있다.
	// 문자열 출력하기
	FString CharacterName = TEXT("HiWer");
	UE_LOG(LogTemp, Log, TEXT("Character Name = %s"), *CharacterName);
	// %s는 TCHAR 포인터를 받아야한다.

	// Bool형태 출력하기
	bool isAttackable = true;
	UE_LOG(LogTemp, Log, TEXT("Is Attackable = %s"), isAttackable ? TEXT("true") : TEXT("false")); // 직접 문자열을 넣어줘야 함.

	// 정수 & 실수형 출력하기
	int hp = 100;
	UE_LOG(LogTemp, Log, TEXT("HP = %d"), hp);
	float AttackSpeed = 1.0f;
	UE_LOG(LogTemp, Log, TEXT("Attack Speed = %f"), AttackSpeed);

	// Vector형 출력하기
	FVector CharacterPosition = GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("Position = %s"), *(CharacterPosition.ToString())); 
	// ToString : 해당 클래스가 가지고 있는 내용을 문자열로 바꿔줌.
	// 문자열로 사용하기 때문에 포인터 값을 줘야한다.

	// 여러 자료형을 한번에 출력하기
	UE_LOG(LogTemp, Log, TEXT("Character Name = %s, Hp = %d, Attack Speed = %f"), *CharacterName, hp, AttackSpeed);

	// 게임 화면에 LOG 출력하기
	// key, time, color, text
	// key : 1 -> 새로운 로그가 출력되면 기본의 로그는 사라짐.
	// key : -1 -> 새로운 로그가 출력되면 기본의 로그는 아래로 내려감.
	GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Yellow, FString("BeginPlay C++ Call"));

}

void AMyLOG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	time += DeltaTime;
	if (GEngine)
	{	
		// TEXT() : 어떤 플랫폼에서든 2byte 문자열로 동작할 수 있도록 언리얼이 인코딩 해준다.
		// FString::Printf() : %f와 같은 형식지정자를 문자열에 인식 할 수 있도록 한다.
		GEngine->AddOnScreenDebugMessage(
			-1, 
			1.0f, 
			FColor::Yellow,
			FString::Printf(TEXT("%f"), time)
		);

		// Log창에 FString::Printf를 직접 넣는것이 아닌 형식화된 문자열을 넣을 수 있다.
		FString Message = FString::Printf(TEXT("DeltaTime : %f"), DeltaTime);
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Red,
			Message
		);

		// GetName() : Actor의 이름을 가져옴.
		FString Name = GetName();
		
		// ! 형식지정자가 %s(문자열)일떄 변수 앞에 *를 붙여줘야 한다.
		// *를 붙여서 포인터를 참조하는 것처럼 보이지만 
		// 문자열이 TCHAR를 넘길 수 있도록 오버로딩시킨다.
		FString NameMessage = FString::Printf(TEXT("Item Name : %s"), *Name);
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Blue,
			NameMessage
		);
	}
	// UE_LOG에선 FString::SanitizeFloat(DeltaTime) 안됨.
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime : %f"), DeltaTime);
}

