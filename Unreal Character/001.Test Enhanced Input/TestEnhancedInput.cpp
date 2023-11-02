#include "TestEnhancedInput.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
/*AddOnScreenDebugMessage*/
#include "Engine/Engine.h"

ATestEnhancedInput::ATestEnhancedInput()
{
	//PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	/*SkeletalMesh*/
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("USkeletalMeshComponent"));
	SkeletalMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Character/StaticMesh/X_Bot"));
	if(MeshAsset.Succeeded() && MeshAsset.Object != NULL){
		SkeletalMesh->SetSkeletalMesh(MeshAsset.Object);
		SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
		SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		SkeletalMesh->SetWorldScale3D(FVector(1.0f));
	}

}

void ATestEnhancedInput::BeginPlay()
{
	Super::BeginPlay();

	/*Controller Cast PlayerController*/
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController())){
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())){
			Subsystem->AddMappingContext(TestMappingContext, 0);
		}
	}
}

//void ATestEnhancedInput::Tick(float DeltaTime){
//	Super::Tick(DeltaTime);
//
//}

void ATestEnhancedInput::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*BindAction*/
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)){
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Move);
	}
}

/*Fuction Move*/
void ATestEnhancedInput::Move(const FInputActionValue &Value)
{
	/*MoveAction이 Bool형식임으로 MoveAction 값을 CurrentValue에 넣음.*/
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("IA_Move Triggered"));
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("IA_Move Triggered"));
	}
}