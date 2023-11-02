// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "Engine/Classes/Camera/CameraComponent.h"
//UCameraComponent�� ����ϱ� ���� ��ó��

#include "Engine/Classes/Components/StaticMeshComponent.h"
//StaticMeshComponent�� ����ϱ� ���� ��ó��

#include "Engine/Classes/Components/InputComponent.h"
//���ε��� ����� InputComponent�� ����ϱ� ���� ��ó��

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
//ConstructorHelpers�� ����ϱ� ���� ��ó��

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; //Tick �Լ� ȣ��

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	//Player0�� �Է��� ����ǵ��� �� -> ������ ���Ӹ�带 ������� �ʰ� �÷��̾�� �νĽ�Ŵ.

	/*RootComponent*/
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); //RootComponent ����

	/*CameraComponent*/
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));//CameraComponent ����
	OurCamera->SetupAttachment(RootComponent); //OurCamera�� RootComponent�� �����.(RootComponent > OurCamera)
	OurCamera->SetRelativeLocation(FVector(-500.0f, 0.0f, 500.0f)); //���� ������Ʈ�� ������Ʈ�κ��� ������ ��ġ
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f)); //�θ� Ŭ���� �������� �󸶳� ȸ������ ����.

	/*SceneComponent*/
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
	///StaticMeshComponent(SceneComponent�� ������ ��� �߰�) ����, StaticMeshComponent�� SceneComponent�� ��ӹ޴´�.
	StaticMesh->SetupAttachment(RootComponent); // RootComponent ������ SceneComponent�� �߰���.

	/*StaticMesh�� �ҷ�����*/
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere"));
	// //ConstructorHelpers�� �̿��Ͽ� ���ҽ� ���� �ҷ�����
	// if(MeshAsset.Succeeded() && MeshAsset.Object != NULL){
	// 	//Mesh�� �ҷ����� �����ϰ� �޽��� �����Ѵٸ�
	// 	StaticMesh->SetStaticMesh(MeshAsset.Object);
	// 	StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	// 	StaticMesh->SetWorldScale3D(FVector(1.0f));
	// }
	
	/*����ü�� ����Ͽ� ���� ������Ʈ�� �ҷ��ö� ���*/
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
	/*Space Bar �Է½� Scale ����*/
	float CurrentScale = StaticMesh->GetComponentScale().X;
	//SceneComponent�� ���� Scale(transform)���� ����
	if(bGrowing) {
		if(InputTime > 0.0f && InputTime <= 0.5f){
			CurrentScale = 3.0f;
		}
		else {
			CurrentScale += DeltaTime;
		}
	}
	//bGrowing�� true�̸� �ð��� ����ؼ� Scale.x���� ����.
	else {
		CurrentScale -= DeltaTime;
	}
	//bGrowing�� false�̸� �ð��� ����ؼ� Scale.x���� ����.

	CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 3.0f);
	//�Է����� CurrentScale�� 1.0f ~ 3.0f�� ����� ���ϵ��� �Ѵ�.
	StaticMesh->SetWorldScale3D(FVector(CurrentScale));
	//SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));
	//���� �����̽������� ������Ʈ ũ�⸦ ������.

	/*WASD �Է½� �̵�*/
	if(!CurrentVelocity.IsZero()){//�̵� �Է��� �ް� ���� �� ->CurrentVelocity�� ���� (0.0f, 0.0f, 0.0f)�� �ƴ� ��
		if(InputTime < 2.0f){
			InputTime += DeltaTime;
		}
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime * (InputTime >= 1.0f ? InputTime : 1.0f));
		//���� �ð��� 1�� �̻��̸� �ӵ� ����
		SetActorLocation(NewLocation);
		//�̵��� ��ġ�� NewLocation���� ������.
	}
	else InputTime = 0;
	//�̵����� ������ InputTime �ʱ�ȭ
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*���ε�*/
	InputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	//MoveForward������ �Է� ���� MoveForward�Լ��� ����
	InputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);
	//MoveRight������ �Է� ���� MOveRight�Լ��� ����.

	InputComponent->BindAction("Grow", EInputEvent::IE_Pressed, this, &AMyPawn::StartGrowing);
	//Grow�Է��� ������ �� StartGrowing
	InputComponent->BindAction("Grow", EInputEvent::IE_Released, this, &AMyPawn::StopGrowing);
	//Grow�Է��� ���� �� StopGrowing
}

void AMyPawn::MoveForward(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -5.0f, 5.0f) * 100.0f;
	//-10 ~ 10 �����ϴ� ���� : W�� ����Ű�� ���ÿ� ������ �Է°��� ����Ǿ� 10 + 10 ���� ����.
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