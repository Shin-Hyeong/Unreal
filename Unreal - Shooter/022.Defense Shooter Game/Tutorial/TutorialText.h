#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialText.generated.h"

class UBoxComponent;

UCLASS()
class SHOOTER_API ATutorialText : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialText();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// Delegate
	UFUNCTION()
	void GuideAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void GuideAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void SineMove(float DeltaTime);

private:
	// Root
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Root", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Root;

	// Arrow Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	float YawRate{ 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	bool bCanTurn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	bool bBeginVisible{ false };

	// Area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Area", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TutorialBoxArea;

	// Next Target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guide | Target", meta = (AllowPrivateAccess = "true"))
	ATutorialText* NextTarget{ nullptr };

	// SineMove
	UPROPERTY(EditAnywhere, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	float Amplitude = 0.f;

	UPROPERTY(EditAnywhere, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	float SineSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Guide | Mesh", meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0.f;

	// Text
	UPROPERTY(EditAnywhere, Category = "Guide | Text", meta = (AllowPrivateAccess = "true"))
	int32 GuideTextIndx{ 0 };

	

public:	
	void SetCanTurn(bool CanTurn) { bCanTurn = CanTurn; }
	UStaticMeshComponent* GetArrowMesh() const { return ArrowMesh; }
};
