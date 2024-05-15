#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingUI.generated.h"


UCLASS()
class SOULSLIKE__API USettingUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetSaveDate(float ValueX, float ValueY, bool RunToggle, float CameraArm, float FPS);

protected:
	// NativeOninitialized : 위젯 생성시 한번 호출
	// NativeConstruct : AddToViewport시 호출
	// NativeDestruct : RemoveFromParent(RemoveFromViewPort_싷 ㅗ출
	virtual void NativeConstruct() override;
	
private:
	/** Bind DeleGate Event */
	// 설정값 데이터 적용
	UFUNCTION()
	void OnSaveButtonClick();
	// 설정 UI 끔 & 설정값 적용 안함
	UFUNCTION()
	void OnCancelButtonClick();
	// 게임 끔
	UFUNCTION()
	void OnQuitGameButtonClick();
	// 다시시작
	UFUNCTION()
	void OnRestartButtonClick();
	// 마우스 X 감도 변경
	UFUNCTION()
	void OnValueChangedSliderX(float Value);
	// 마우스 Y 감도 변경
	UFUNCTION()
	void OnValueChangedSliderY(float Value);
	// Run Toggle 상태 변경시 호출
	UFUNCTION()
	void OnValueChangedSliderArm(float Value);
	// FPS 슬라이더 변경
	UFUNCTION()
	void OnValueChangedSliderFPS(float Value);
	// 해상도 변경
	UFUNCTION()
	void OnClickResolutionBigger(); 
	UFUNCTION()
	void OnClickResolutionSmaller(); 

	// 창모드 변경
	UFUNCTION()
	void OnClickButtonWindowState();

	UFUNCTION()
	void OnClickButtonWindowMode();

	UFUNCTION()
	void OnClickButtonFullScreenMode();

private:
	UPROPERTY()
	class APlayerController* PlayerController;

	UPROPERTY()
	class APlayableCharacter* Player;

	// 마우스 감도 X
	UPROPERTY(meta = (BindWidget))
	class USlider* MouseXSlider;

	UPROPERTY(meta = (BineWidget))
	class UTextBlock* MouseXText;

	// 마우스 감도 Y
	UPROPERTY(meta = (BindWidget))
	class USlider* MouseYSlider;

	UPROPERTY(meta = (BineWidget))
	class UTextBlock* MouseYText;

	// 카메라 암 길이 조절
	UPROPERTY(meta = (BindWidget))
	class USlider* CameraArmSlider;

	UPROPERTY(meta = (BineWidget))
	class UTextBlock* CameraArmText;

	// 설정 저장 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* SaveButton;

	// 설정 취소 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	// 게임 종료 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	// 게임 재시작
	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;

	// 뛰기 토글/유지 
	UPROPERTY(meta = (BindWidget))
	class UCheckBox* RunCheckBox;

	UPROPERTY(meta = (BindWidget))
	class USlider* FPSSlider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FPSText;

	// 해상도 변경
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Smaller;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Bigger;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResolutionText;

	// 창모드/전체모드
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_WindowState;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* T_WindowState;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* WindowScroll;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_FullScreen;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Window;

	/** Attribute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName OpenLevel;

	class UGameUserSettings* ScreenMode;

	// 0 : 1280 * 720
	// 1 : 1920 * 1080
	// 2 : 2560 * 1440
	int32 ResolutionIdx = 1;
	int32 ResolutionX[3] = { 1280, 1920, 2560 };
	int32 ResolutionY[3] = { 720, 1080, 1440 };
	FString Resolution[3] =
	{
		"1280 * 720",
		"1920 * 1080",
		"2560 * 1440"
	};

	/** Save Value */
	float SaveMouseXValue = 1.f;
	float SaveMouseYValue = 1.f;
	bool SaveRunCheckBox = false;
	float SaveCameraArm = 330.f;
	float SaveFPS = 60.f;
	FString ScreenModeString = "Full Window";
	int32 SaveResolutionIdx = 1;
};
