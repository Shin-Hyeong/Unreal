// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "Engine/Classes/Camera/CameraComponent.h"
//UCameraComponent를 사용하기 위한 전처리

#include "Engine/Classes/Components/StaticMeshComponent.h"
//StaticMeshComponent를 사용하기 위한 전처리

#include "Engine/Classes/Components/InputComponent.h"
//바인딩에 사용할 InputComponent를 사용하기 위한 전처리

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
//ConstructorHelpers를 사용하기 위한 전처리

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; //Tick 함수 호출

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	//Player0의 입력이 연결되도록 함 -> 별도의 게임모드를 사용하지 않고 플레이어로 인식시킴.

	/*RootComponent*/
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); //RootComponent 생성

	/*CameraComponent*/
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));//CameraComponent 생성
	OurCamera->SetupAttachment(RootComponent); //OurCamera가 RootComponent를 상속함.(RootComponent > OurCamera)
	OurCamera->SetRelativeLocation(FVector(-500.0f, 0.0f, 500.0f)); //상위 컴포넌트나 오브젝트로부터 떨어진 위치
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f)); //부모 클래스 기준으로 얼마나 회전할지 정함.

	/*SceneComponent*/
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
	///StaticMeshComponent(SceneComponent에 렌더링 기능 추가) 생성, StaticMeshComponent는 SceneComponent를 상속받는다.
	StaticMesh->SetupAttachment(RootComponent); // RootComponent 하위에 SceneComponent를 추가함.

	/*StaticMesh를 불러오기*/
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));
	// //ConstructorHelpers를 이용하여 리소스 파일 불러오기
	// if(MeshAsset.Succeeded() && MeshAsset.Object != NULL){
	// 	//Mesh를 불러오기 성공하고 메쉬가 존재한다면
	// 	StaticMesh->SetStaticMesh(MeshAsset.Object);
	// 	StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	// 	StaticMesh->SetWorldScale3D(FVector(1.0f));
	// }
	
	/*구조체를 사용하여 다중 오브젝트를 불러올때 사용*/
	struct FConstructorStatics{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> MeshAsset01;

		FConstructorStatics()
			: MeshAsset01(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"))
		{}
	};
	static FConstructorStatics ConstructorStatics;
	StaticMesh->SetStaticMesh(ConstructorStatics.MeshAsset01.Get());
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Test"));
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*Space Bar 입력시 Scale 조정*/
	float CurrentScale = StaticMesh->GetComponentScale().X;
	//SceneComponent로 부터 Scale(transform)값을 받음
	if(bGrowing) {
		if(InputTime > 0.0f && InputTime <= 0.5f){
			CurrentScale = 3.0f;
		}
		else {
			CurrentScale += DeltaTime;
		}
	}
	//bGrowing이 true이면 시간에 비례해서 Scale.x값에 더함.
	else {
		CurrentScale -= DeltaTime;
	}
	//bGrowing이 false이면 시간에 비례해서 Scale.x값을 뺀다.

	CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 3.0f);
	//입력으로 CurrentScale이 1.0f ~ 3.0f를 벗어나지 못하도록 한다.
	StaticMesh->SetWorldScale3D(FVector(CurrentScale));
	//SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));
	//월드 스페이스에서의 컴포넌트 크기를 수정함.

	/*WASD 입력시 이동*/
	if(!CurrentVelocity.IsZero()){//이동 입력을 받고 있을 때 ->CurrentVelocity의 값이 (0.0f, 0.0f, 0.0f)이 아닐 때
		if(InputTime < 2.0f){
			InputTime += DeltaTime;
		}
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime * (InputTime >= 1.0f ? InputTime : 1.0f));
		//누른 시간이 1초 이상이면 속도 증가
		SetActorLocation(NewLocation);
		//이동한 위치를 NewLocation으로 지정함.
	}
	else InputTime = 0;
	//이동하지 않으면 InputTime 초기화
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*바인딩*/
	InputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	//MoveForward에서의 입력 값을 MoveForward함수에 전달
	InputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);
	//MoveRight에서의 입력 값을 MOveRight함수에 전달.

	InputComponent->BindAction("Grow", EInputEvent::IE_Pressed, this, &AMyPawn::StartGrowing);
	//Grow입력을 눌렀을 때 StartGrowing
	InputComponent->BindAction("Grow", EInputEvent::IE_Released, this, &AMyPawn::StopGrowing);
	//Grow입력을 땟을 떄 StopGrowing
}

void AMyPawn::MoveForward(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -5.0f, 5.0f) * 100.0f;
	//-10 ~ 10 제한하는 이유 : W와 방향키를 동시에 누르면 입력값이 가산되어 10 + 10 값을 받음.
}

void AMyPawn::MoveRight(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -5.0f, 5.0f) * 100.0f;
}

void AMyPawn::StartGrowing()
{
	bGrowing = true;
}

void AMyPawn::StopGrowing()
{
	bGrowing = false;
}