// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SetActor.h"

// Draw Debug Macros
#include "UdemyCpp/DebugMacros.h"

// Sets default values
ASetActor::ASetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASetActor::BeginPlay()
{
	Super::BeginPlay();

	// 처음 위치와 방향
	Location = GetActorLocation();
	Forward = GetActorForwardVector();

	// 위로 200 이동한 위치
	NewLocation = FVector(Location.X, Location.Y, 200.f);
	
	// 처음 위치와 방향 표시
	DRAW_SPHERE(Location);
	DRAW_VECTOR(Location, Location + Forward * 100);

	// Actor 이동
	// FVector NewLocation, bool Sweep(false), FHitResult, ETeleportType
	// bool Sweep : Actor가 움직이면 다른 오브젝트를 통해 움직이는지 여부를 감지 -> 방해가 될 수 있는 무언가와 부닺히면 중지됨.
	// FHitResult, ETeleportType : Sweep를 활성화할 때 입력 추가 입력값.
	SetActorLocation(NewLocation);

	// Actor 회전
	// FQuat NewRotation, ETeleportType(none)
	// FRotator : Pitch(y), Yaw(z), Roll(x)
	SetActorRotation(FRotator(0.f, 45.f, 0.f));

	// 새로운 방향
	NewForward = GetActorForwardVector();

	// 새로운 위치와 방향 표시
	DRAW_SPHERE(NewLocation);
	DRAW_VECTOR(NewLocation, NewLocation + NewForward * 100);
	
}

// Called every frame
void ASetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float MovementRate = 100.f;
	float RotationRate = 90.f;

	// AddActorWorldOffset : World 기준으로(Local 기준 X) 현재 위치(Location)에서 입력값을 더 함
	// MovementRate(cm/s) * DeltaTime(s/frame) = cm/s * s/frame = cm/frame
	// 원하는 이동 속도 * DeltaTime를 하여 프레임당 동일한 속도를 나오도록 함.
	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));

	// AddActorWorldRotation : World 기준으로(Local 기준 X) 현재 회전값(Rotator)에서 입력값을 더 함
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

}

