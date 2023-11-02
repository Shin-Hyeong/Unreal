#pragma once
// ���� �ҽ� ������ �������� �ѹ��� �ϵ��� �����.
// ���� ������ �ش� ������ ���Խ��� �о�帱�� ����Ͽ� ������ ������ ���� �ʰ� �ѹ��� �������ϵ��� ��.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhatIsClassNFunction.generated.h" //"����.generated.h" -> ���÷����� ������ �Ϸ��� �ش� ����� �����ؾ� ��.
// �ش� ����� ������ ���÷����� �ִ� �ý��ۿ��� �ش� Ŭ������ ����ؾ� �ϰ� �ý��� ������ �ʿ����� �𸮾� ����� UHT(Unreal Header Tool)�� �˸���
// �������ϱ� ������ UHT�� ����� Ŭ���� ���� ���� ���÷����� ���� ��ũ�� �� Ű���带 �м��Ѵ�.
// �м��� ����� ���� �𸮾� ���� ȯ�濡 �ʿ��� �ΰ� ������ �ش� generated.h ���Ͽ� �����Ѵ�.
// ������ generated.h��� ���ϰ� �Բ� �ҽ��ڵ带 �������ϸ� �𸮾� ���� ȯ���� �̸� ����� �ش� �𸮾� ������Ʈ�� �����ϰ� �����Ϳ��� �ش� ���� ������ �� �ִ� �������̽��� �����Ѵ�.
// generated.h ����� ������ Ŭ������ UCLASS()�� GENERATED_BODY() ��ũ�θ� �ݵ�� �����ؾ� �Ѵ�.

// UCLASS() & GENERATED_BODY(), UENUM(), USTRCUT(), UFUNCTION(), UPROPERTY()
// ���÷��� ����� Ȱ��ȭ ��Ű�� ��ũ��
// ������Ʈ�� ���÷����� ������ ������ �г�, ����ȭ, ������ �ݷ���, ��Ʈ��ũ ���ø����̼�, �������Ʈ �� �𸮾� ������ ������ �ý��� ��� ����
// BeginPlay(), UE_LOG()�� ��� ����� �� �ֵ��� ��.
// UObject() : Super:: �� ���� ���� �̺�Ʈ �Լ� ȣ�� �Ҷ���  UObject�� �� Ŭ������ ����� �� �ִٷ� �Ǿ�����.

UCLASS()
// UCLASS() ��ȣ�� Ư���� ����� �ϴ� �����ڸ� ������ �� �ִ�.
// ������ : https://docs.unrealengine.com/4.27/ko/ProgrammingAndScripting/GameplayArchitecture/Classes/Specifiers/
class FORENHANCEDINPUT_API AWhatIsClassNFunction : public AActor
{
	GENERATED_BODY()
	//�𸮾� ������Ʈ�� ���� Ŭ������ �𸮾� �ý����� ���� �߰����� �Լ� 
	//Super:: �Ǵ� ThisClass::�� ���� ������ typedef�� ��� �� �� �ְ� ���ش�.
	
public:	
	AWhatIsClassNFunction();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type) override;
	// Type�� ���� : Destoryed, LevelTransition, EndPlayInEditor, RemovedFromWorld, Quit
	// Type�� ������ �°� ȣ��Ǹ� ȣ��ɶ� ���ڸ� �޾ƿ´�.
	virtual void PostInitializeComponents() override;
	// ������ �������� �� ������ ��ġ�� ���Ͱ� �۵��ϱ� ���� ������Ʈ ������ �Ϸ�Ǿ��� �� ȣ���ϴ� �Լ�
	// �� �Լ��� ȣ��� ������ ���Ͱ� �����ǰ�, ���Ͱ� ������ �� BeginPlay()�� ȣ���Ѵ�.

public:	
	virtual void Tick(float DeltaTime) override;
	// ���� Ŭ������ virtual Ű���带, �Ļ� Ŭ�������� override Ű���带 �߰��Ѵ�.
	// �Ļ� Ŭ������ virtual Ű������� �����ϴ� ������ Ȥ��  �� �ٸ� Ŭ������ �Ļ� Ŭ������ ����ϰ� �Ǿ��� �� ���� �̺�Ʈ �Լ��� ȣ�� �ϴ� ���� ����ϱ� ����.
	// WhatIsClassNFunction Ŭ������ AActor Ŭ������ ��� �ް�, AActor Ŭ������ �� �ڵ忡 �ִ� BeginPlay(), Tick() ���� �̺�Ʈ �Լ��� ���� �Լ��� ���ǵǾ� �ִ�.
	// ���� WhatIsClassNFunctionŬ�������� �ش� �Լ����� �������̵��ؼ� ����Ѵ�.

};
// �̺�Ʈ �Լ� ���� ������
// BeginPlay() : ���Ͱ� ���� �� �Լ��� ȣ�� �ϱ� ����
// TIck() : �𸮾� �������� ����Ʈ�� �����Ͽ� ȣ���ϱ� ����.