#include "Actor/Weapons/Weapon.h"
// 충돌한 Actor가 플레이어인지 감지하기 위한 참조
#include "Player/PlayerCharacter.h"

// 로컬에서 사운드를 재생시키기 위함
// Damage를 주기 위해서
#include "Kismet/GameplayStatics.h"

// Weapon이 장착된 상태에서 Item의 Sphere의 중복 충돌을 막기 위해
#include "Components/SphereComponent.h"
// Weapon의 칼날에 충돌을 감지하기 위해
#include "Components/BoxComponent.h"
// 충돌한 Actor가 Interface를 가지고 있는 확인하기 위해
#include "Interfaces/HitInterface.h"

// BoxTrace 기능
#include "Kismet/KismetSystemLibrary.h"
// Draw Debug 기능
#include "DrawDebugHelpers.h"

// Niagara System 제어
#include "NiagaraComponent.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	/*
	* WeaponBox의 Collision 설정
	*/ 
	// Collision 활성화를 NoCollision로 설정
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Collsion 반응으로 모두 Overlap으로 설정
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	// 특정(Pawn) 대상으로만 Collision 반응을 수정(Ignore)
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// BoxTrace 지점 생성
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Delegate에 함수 바인드
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{	
	// 아이템을 장착 상태로 변경
	ItemState = EItemState::EIS_Equipped;

	// InParent : 부착될 부모
	// InSocketName : 부착될 Socket 이름
	AttachMashToSocket(InParent, InSocketName);

	// Actor 기능
	// 무기의 주인과 선동자를 설정 -> 데미지를 줄때 필요함.
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	DisableSphereCollision();
	DeactivateEmbers();
	if (NewOwner->ActorHasTag(FName("EngageableTarget")))
		PlayEquipSound();
}

// NS 이펙트 제거
void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		// 무기 획득 시 NS 이펙트 제거
		ItemEffect->Deactivate();
	}
}

// Sphere의 Collision 기능 비활성화
void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		// 무기 획득이 완료되면 Sphere의 Collision 기능을 비활성화 시킴.
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// 무기 획득 사운드 재생
void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		// UGamePlayStatic : 객체의 인스턴스가 월드에 존재할 필요없음. 특정 객체가 가지고 있는 함수를 호출하지 않음

		// (UObject*, USoundBase, FVecter Loacation, float Volume = 1.0f, float Pitch = 1.0f,
		// float StartTime = 0.0f, USoundAttenuation(거리에 따른 감쇠 정도) = nullptr, 
		// USoundConcurrency = nullptr,
		// UInitialActiveSOundparams = nullptr)
		UGameplayStatics::PlaySoundAtLocation(
			this,	// 소리의 주인인 객체
			EquipSound,	// 재생될 사운드
			GetActorLocation() // 사운드가 재생될 위치
		);
	}
}

void AWeapon::AttachMashToSocket(USceneComponent* InParent, const FName& InSocketName)
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
}


void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 무기의 주인이 Enemy이고 충돌한 상대가 Enemy이면.
	if (ActorIsSameType(OtherActor)) return;

	// FHitResult : 충돌 결과를 담는 구조체(충돌한 Actor, 등)
	FHitResult BoxHit;

	BoxTrace(BoxHit);

	// 적중하면 해당 Actor을 가져오고 실패하면 null값을 줌.
	// 먼저 ApplyDamage를 통해 데미지를 주고 GetHit를 통해 Health를 계산 후 AnimMontage를 재생
	if (BoxHit.GetActor())
	{	
		// 동시에 다른 Actor와 Enemy가 Overlap되었을 떄 대비하여 따로 다시 검사함.
		// 무기의 주인이 Enemy이고 충돌한 상대가 Enemy이면.
		if (ActorIsSameType(BoxHit.GetActor())) return;

		// Damage를 전달함. -> BoxHit.GetActor()의 TakeDamage가 반응함.
		// this가 GetInstigator()->GetController()에 의해
		// BoxHit.GetActor()에게 Damage만큼 UDamageType::StaticClass() 방식으로 피해를 줌.
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(), // AActor* DamagedActor
			Damage, // float BaseDamage
			// GetInstigator() -> Pawn 형태임.
			// Pawn->GetController() : Pawn이 가지고 있는 기본 컨트롤러를 가져옴.
			GetInstigator()->GetController(), // AController* EventInstigator
			this, // AActor* DamageCauser
			UDamageType::StaticClass() // TSubclassOf<UDamageType> DamageTypeClass
		);

		ExecuteGetHit(BoxHit);

		// ImpactPoint에 FieldSystem 생성
		CreateFields(BoxHit.ImpactPoint);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

// 피격 방향을 통해 반응하도록 함
void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	// 적중된 Actor가 HitInterface를 상속 하고 있는지 확인. Cast를 성공하면 값을 가짐.
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		// 충돌 지점을 매개변수로 넘기면서 GetHit 호출
		// NativeEvent를 컴파일하면서 생기는 함수 BP의 함수와 C++의 함수를 호출할 수 있음.
		// Execute_GetHit(UObject *O, const FVector &ImpactPoint, AActor* Hitter)
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	// 충돌 시 BoxTraceStart/End의 World Location를 가져옴.
	// GetComponentLocation() : World Location
	// GetRelativeLocation() : Local Location
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	// BoxTrace에서 충돌을 무시해야할 Actor 배열
	TArray<AActor*> ActorsToIgnore;
	// 자기 자신의 충돌을 무시하도로 함.
	ActorsToIgnore.Add(this);

	// 한번의 공격으로 한 Actor가 여러번 공격되는것을 막음.
	// 공격이 완료 된후 (Box Collison이 비활성화 될때) ActorsToIgnore를 비움.
	// IgnoreActors의 배열에 있는 Actor를 가지고 반복문을 돌린다.
	for (AActor* Actor : IgnoreActors)
	{
		// 충돌 무시할 Actor에 IgnoreActors 배열에 있는 Actors들을 추가한다.
		ActorsToIgnore.AddUnique(Actor);
	}

	// BoxTrace 함수
	UKismetSystemLibrary::BoxTraceSingle(
		this,	// UObject* WorldContextObjcet : Trace의 주인
		Start,	// FVector Start : 시작 지점
		End,	// FVector End : 끝 지점
		BoxTraceExtent,	// FVector halfSize : 중심점으로 부터 박스 크기
		BoxTraceStart->GetComponentRotation(),	// Box Trace의 방향 = Start의 Rotation
		ETraceTypeQuery::TraceTypeQuery1,	// ETraceTypeQuery : Box Trace는 기본적으로 Visibility와 충돌을 감지한다.
		false,	// bool bTraceComplex
		ActorsToIgnore, // const TArray<AActor*> ActorToIgnore
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, //EDrawDebugTrace::Type DrawDebugType
		BoxHit,	// FHitResult &OutHit -> 적중된 Actor 등
		true,	// bool bIgnoreSelf : 자신과의 충돌을 무시한다
		FColor::Red,	// FLinearColor TraceColor(Red)
		FColor::Green,	// FLinearColor TraceHitColor(Green)
		5.0f	// float DrawTime(5.0f))
	);

	// 공격이 한번 이루어진 Actor를 배열에 추가함.
	// AddUnique() : 추가할 때 같은 요소가 있으면 추가 안함.
	IgnoreActors.AddUnique(BoxHit.GetActor());
}