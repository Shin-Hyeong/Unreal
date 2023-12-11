#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// HitInterface를 상속시키기 위함
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;


UCLASS()
class UDEMYCPP_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

private:
	/*
	* Animation Montage
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	// UParticleSystem은 기본으로 지원됨. 따로 class UParticleSystem 선언 불필요
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

public:	
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// HitInterface
	// _Implementation 붙여 Native Function으로 만듦.
	virtual void GetHit_Implementation(const FVector& ImpactPoint);


protected:
	virtual void BeginPlay() override;

	/*
	*  Play Montage functions
	*/
	// 공격 모션 재생
	void PlayHitReactMontage(const FName& SectionName);

private:
	// 공격 받는 방향을 구하는 함수
	void DirectionalHitReact(const FVector& ImpactPoint);
	// 공격 받는 벡터를 Debug Arrow 하는 함수
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);
};
