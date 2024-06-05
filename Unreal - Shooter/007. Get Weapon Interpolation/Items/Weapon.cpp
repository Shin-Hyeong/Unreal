#include "Items/Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 떨어지는 동안 Yaw만 회전
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		// ItemMesh의 Yaw
		const FRotator MeshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f }; 
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics); 

	// 무기의 전방 벡터
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() }; 
	// 무기의 오른쪽 벡터
	const FVector MeshRight{ GetItemMesh()->GetRightVector() }; 
	// 무기를 버릴때 던지는 방향 (MeshForward의 -20도 방향)
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward); 

	float RandoRotation = FMath::FRandRange(0.f, 30.f); 
	// MeshForward의 -20도를 회전시킨 값에 Z축 기준으로 랜덤 회전(0.0 ~ 30.0)
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandoRotation, FVector(0.0f, 0.0f, 1.0f));
	ImpulseDirection *= 15.f;

	// ImpulseDirection 방향으로 충돌
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

}

void AWeapon::StopFalling()
{
	bFalling = false;
	// 주울 수 있는 상태로 변경
	SetItemState(EItemState::EIS_PickUp);
}
