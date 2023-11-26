#include "Actor/Weapons/Weapon.h"
// 충돌한 Actor가 플레이어인지 감지하기 위한 참조
#include "Player/PlayerCharacter.h"

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{	
	// FAttachmentTransformRules : Attench가 될 때 Location, Rotation, Scale의 규칙을 정함.
	// FAttachmentTransformRules는 기본 생성자가 없기 떄문에 인자를 넘겨줘야한다.
	// 인자들은 열거형으로 이루어져 있다.(EAttachmentRule::)
	// 인자는 괄호 안에 커서를 두고 Ctrl + Shift + Spacebar를 하면 인자를 확인할 수 있다.
	// bInWeldSimulatedBodies(true) : 부모가 피직스 바디가 적용되면 같이 적용시킬지 여부 확인
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);

	// AttachToComponent : 해당 Component를 다른 Component에 부착시킴.
	// (USceneComponent* InParent, cont FAttachmentTransformRules, FName InSocketName)
	// (Component가 부착될 Compoenet, 부착 규칙, 소켓이름)
	StaticMesh->AttachToComponent(InParent, TransformRules, InSocketName);

	// 아이템을 장착 상태로 변경
	ItemState = EItemState::EIS_Equipped;
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
