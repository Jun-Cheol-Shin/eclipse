// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "EpTabListWidgetBase.generated.h"

class UHorizontalBox;

USTRUCT(BlueprintType)
struct FEpTabParameter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> TabIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UCommonUserWidget> TabContent;
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
	//UFUNCTION(BlueprintImplementableEvent, Category = "Tab Button")
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
	bool RegisterDynamicTab(const FEpTabParameter& InTabParams);

private:
	void OnSelectedTabButton(UCommonButtonBase* SelectedTabButton, int32 ButtonIndex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

private:
	void CreateTabs();

	bool GetPreregisteredTabInfo(const FName TabNameId, FEpTabParameter& OutTabInfo);

#if WITH_EDITOR

	UFUNCTION(CallInEditor, Category = "Tab List")
	void DebugPreviewTab();
#endif

private:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TabId"))
	TArray<FEpTabParameter> PreregisteredTabInfoArray;

	/**
	 * Stores label info for tabs that have been registered at runtime but not yet created.
	 * Elements are removed once they are created.
	 */
	UPROPERTY()
	TMap<FName, FEpTabParameter> PendingTabLabelInfoMap;


private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* TabButtonBox = nullptr;
};
