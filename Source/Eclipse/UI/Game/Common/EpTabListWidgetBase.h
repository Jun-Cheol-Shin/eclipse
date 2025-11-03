// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "GameplayTagContainer.h"
#include "EpTabListWidgetBase.generated.h"

class UHorizontalBox;

UENUM(BlueprintType)
enum class EEpButtonState : uint8
{
	Disable = 0,
	Enable,
	Selected,
};

USTRUCT(BlueprintType)
struct FEpTabParameter
{
	GENERATED_BODY()

public:

	// PlayerController Input Action...
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag TabActionTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> TabIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButton = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UCommonUserWidget> TabContent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	EEpButtonState PreviewButtonState = EEpButtonState::Disable;
#endif
};


// Tab Button Interface.....
UINTERFACE(BlueprintType)
class UEclipseTabButtonInterface : public UInterface
{
	GENERATED_BODY()
};


class IEclipseTabButtonInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnChangedTabState(EEpButtonState InState) { OnChangedTabState(InState); }

	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnChangedTabState(EEpButtonState TabDesInStatecriptor);

	virtual void NativeOnSetTabInfo(const FEpTabParameter& TabDescriptor) { OnSetTabInfo(TabDescriptor); }

	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnSetTabInfo(const FEpTabParameter& TabDescriptor);

};


// Tab Widget Interface.....
UINTERFACE(BlueprintType)
class UEclipseTabContentInterface : public UInterface
{
	GENERATED_BODY()
};


class IEclipseTabContentInterface
{
	GENERATED_BODY()

public:
	virtual void NativeOnCreatedContentWidget() { OnCreatedContentWidget(); }

	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnCreatedContentWidget();

	virtual void NativeOnActiveContent() { OnActiveContent(); }

	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnActiveContent();

	virtual void NativeOnInactiveContent() { OnInactiveContent(); }

	UFUNCTION(BlueprintImplementableEvent, Category = ObjectListEntry, meta = (DisplayName = "On Entry Item Object Set"))
	void OnInactiveContent();
};



UCLASS()
class ECLIPSE_API UEpTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetEnableButton(FName InTabId, bool bIsEnable);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetHiddenButton(FName InTabId, bool bIsVisible);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool RegisterDynamicTab(const FEpTabParameter& InTabParams);

private:
	void OnSelectedTabButton(UCommonButtonBase* SelectedTabButton, int32 ButtonIndex);
	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

protected:
	void NativeOnCreatedTabButton(uint8 Index, UCommonButtonBase* InButton, UPanelWidget* InPanelWidget);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Created Tab Button"))
	void OnCreatedTabButton(uint8 Index, UCommonButtonBase* InButton, UPanelWidget* InPanelWidget);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

private:
	void CreateTabs();
	bool GetPreregisteredTabInfo(const FName TabNameId, FEpTabParameter& OutTabInfo);

private:
	/**
	 * Stores label info for tabs that have been registered at runtime but not yet created.
	 * Elements are removed once they are created.
	 */
	UPROPERTY()
	TMap<FName, FEpTabParameter> PendingTabLabelInfoMap;


private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, meta = (Category = "Tab List Option", AllowPrivateAccess = "true"))
	bool bDebugPreview = false;



#endif 

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (Category = "Tab List Option", AllowPrivateAccess = "true"))
	FSlateChildSize TabSize;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (Category = "Tab List Option", AllowPrivateAccess = "true"))
	float TabSpacing = 0.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (Category = "Tab List Option", AllowPrivateAccess = "true"))
	TEnumAsByte<EHorizontalAlignment> TabHorizontalAlignment;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (Category = "Tab List Option", AllowPrivateAccess = "true"))
	TEnumAsByte<EVerticalAlignment> TabVerticalAlignment;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UPanelWidget> TabButtonBox = nullptr;

	UPROPERTY(EditInstanceOnly, meta = (TitleProperty = "TabId"))
	TArray<FEpTabParameter> PreregisteredTabInfoArray;

};
