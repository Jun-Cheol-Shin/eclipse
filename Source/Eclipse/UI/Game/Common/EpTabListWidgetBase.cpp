// Fill out your copyright notice in the Description page of Project Settings.


#include "EpTabListWidgetBase.h"
#include "EpBoundActionButton.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"
#include "Groups/CommonButtonGroupBase.h"

#include "Components/HorizontalBox.h"

void UEpTabListWidgetBase::SetEnableButton(FName InTabId, bool bIsEnable)
{
	UCommonButtonBase* ButtonBase = GetTabButtonBaseByID(InTabId);
	if (nullptr != ButtonBase)
	{
		ButtonBase->SetIsEnabled(bIsEnable);
	}
}

bool UEpTabListWidgetBase::RegisterDynamicTab(const FEpTabParameter& InTabParams)
{
	PendingTabLabelInfoMap.Add(InTabParams.TabId, InTabParams);

	UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher();
	if (!ensure(CurrentLinkedSwitcher)) return false;

	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(InTabParams.TabContent.ToSoftObjectPath(), FStreamableDelegateWithHandle::CreateWeakLambda(this,
		[this, CurrentLinkedSwitcher, InTabParams](TSharedPtr<FStreamableHandle> InHandle)
		{
			if (ensure(InHandle.IsValid() && InHandle->GetLoadedAsset()))
			{
				UCommonUserWidget* TabContentWidget = CreateWidget<UCommonUserWidget>(GetOwningPlayer(), Cast<UClass>(InHandle->GetLoadedAsset()));
				if (!ensure(TabContentWidget)) return;

				UE_LOG(LogTemp, Warning, TEXT("Created Contents Tab! %s"), ANSI_TO_TCHAR(__FUNCTION__));

				// Add the tab content to the newly linked switcher.
				if (!CurrentLinkedSwitcher->HasChild(TabContentWidget))
				{
					CurrentLinkedSwitcher->AddChild(TabContentWidget);
				}

				if (GetTabButtonBaseByID(InTabParams.TabId) == nullptr)
				{
					RegisterTab(InTabParams.TabId, InTabParams.TabButton, TabContentWidget);
				}
			}
		}));

	return true;
}

void UEpTabListWidgetBase::OnSelectedTabButton(UCommonButtonBase* SelectedTabButton, int32 ButtonIndex)
{
	if (UEpBoundActionButton* ActionButton = Cast<UEpBoundActionButton>(SelectedTabButton))
	{


	}
}

void UEpTabListWidgetBase::CreateTabs()
{
	UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher();
	if (!ensure(CurrentLinkedSwitcher)) return;
	
	for (FEpTabParameter& Param : PreregisteredTabInfoArray)
	{
		if (nullptr == Param.TabContent) continue;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(Param.TabContent.ToSoftObjectPath(), FStreamableDelegateWithHandle::CreateWeakLambda(this,
			[this, CurrentLinkedSwitcher, Param](TSharedPtr<FStreamableHandle> InHandle)
			{
				if (ensure(InHandle.IsValid() && InHandle->GetLoadedAsset()))
				{
					UCommonUserWidget* TabContentWidget = CreateWidget<UCommonUserWidget>(GetOwningPlayer(), Cast<UClass>(InHandle->GetLoadedAsset()));
					if (!ensure(TabContentWidget)) return;

					UE_LOG(LogTemp, Warning, TEXT("Created Contents Tab! %s"), ANSI_TO_TCHAR(__FUNCTION__));

					// Add the tab content to the newly linked switcher.
					if (!CurrentLinkedSwitcher->HasChild(TabContentWidget))
					{
						CurrentLinkedSwitcher->AddChild(TabContentWidget);
					}

					if (GetTabButtonBaseByID(Param.TabId) == nullptr)
					{
						RegisterTab(Param.TabId, Param.TabButton, TabContentWidget);
					}
				}
			}));

	}
}

bool UEpTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FEpTabParameter& OutTabInfo)
{
	const FEpTabParameter* const FoundTabInfo = PreregisteredTabInfoArray.FindByPredicate([&](FEpTabParameter& TabInfo) -> bool
		{
			return TabInfo.TabId == TabNameId;
		});

	if (!FoundTabInfo)
	{
		return false;
	}

	OutTabInfo = *FoundTabInfo;
	return true;
}

void UEpTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabButtonGroup)
	{
		TabButtonGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &UEpTabListWidgetBase::OnSelectedTabButton);
	}

	CreateTabs();
}

void UEpTabListWidgetBase::NativeDestruct()
{
	PreregisteredTabInfoArray.Reset();
	PendingTabLabelInfoMap.Reset();

	Super::NativeDestruct();
}

void UEpTabListWidgetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();


}

void UEpTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	FEpTabParameter* TabInfoPtr = nullptr;

	FEpTabParameter TabInfo;
	if (GetPreregisteredTabInfo(TabId, TabInfo))
	{
		TabInfoPtr = &TabInfo;
	}
	else
	{
		TabInfoPtr = PendingTabLabelInfoMap.Find(TabId);
	}

	if (IEclipseTabButtonInterface* TabButtonInterface = Cast<IEclipseTabButtonInterface>(TabButton))
	{
		TabButtonInterface->NativeOnSetTabInfo(TabInfo);
	}

	PendingTabLabelInfoMap.Remove(TabId);

	if (TabButtonBox)
	{
		TabButtonBox->AddChild(TabButton);
	}
}

void UEpTabListWidgetBase::DebugPreviewTab()
{
	TabButtonGroup = NewObject<UCommonButtonGroupBase>(this);

	for (FEpTabParameter& Param : PreregisteredTabInfoArray)
	{
		RegisterTab(Param.TabId, Param.TabButton, nullptr);
	}
}
