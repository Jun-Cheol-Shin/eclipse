// Fill out your copyright notice in the Description page of Project Settings.


#include "EpTabListWidgetBase.h"
#include "EpBoundActionButton.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"
#include "Groups/CommonButtonGroupBase.h"

#include "Components/HorizontalBox.h"

#include "CommonActionWidget.h"

void UEpTabListWidgetBase::SelectTab(const FGameplayTag& InTag)
{
	SelectTabByID(InTag.GetTagName());
}

void UEpTabListWidgetBase::SetEnableButton(FName InTabId, bool bIsEnable)
{
	UCommonButtonBase* ButtonBase = GetTabButtonBaseByID(InTabId);
	if (nullptr != ButtonBase)
	{
		ButtonBase->SetIsEnabled(bIsEnable);
	}
}

void UEpTabListWidgetBase::SetHiddenButton(FName InTabId, bool bIsVisible)
{
	UCommonButtonBase* ButtonBase = GetTabButtonBaseByID(InTabId);
	if (nullptr != ButtonBase)
	{
		SetTabVisibility(InTabId, true == bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

bool UEpTabListWidgetBase::RegisterDynamicTab(const FEpTabParameter& InTabParams)
{
	PendingTabLabelInfoMap.Add(InTabParams.TabActionTag.GetTagName(), InTabParams);

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

				if (GetTabButtonBaseByID(InTabParams.TabActionTag.GetTagName()) == nullptr)
				{
					RegisterTab(InTabParams.TabActionTag.GetTagName(), InTabParams.TabButton, TabContentWidget);
				}
			}
		}));

	return true;
}


void UEpTabListWidgetBase::OnSelectedTabButton(UCommonButtonBase* SelectedTabButton, int32 ButtonIndex)
{
	/*if (UEpBoundActionButton* ActionButton = Cast<UEpBoundActionButton>(SelectedTabButton))
	{


	}*/
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

					if (GetTabButtonBaseByID(Param.TabActionTag.GetTagName()) == nullptr)
					{
						RegisterTab(Param.TabActionTag.GetTagName(), Param.TabButton, TabContentWidget);
					}
				}
			}));

	}
}

bool UEpTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FEpTabParameter& OutTabInfo)
{
	const FEpTabParameter* const FoundTabInfo = PreregisteredTabInfoArray.FindByPredicate([&](FEpTabParameter& TabInfo) -> bool
		{
			return TabInfo.TabActionTag.GetTagName() == TabNameId;
		});

	if (!FoundTabInfo)
	{
		return false;
	}

	OutTabInfo = *FoundTabInfo;
	return true;
}

void UEpTabListWidgetBase::RefreshTabAction()
{
	if (nullptr != NextTabActionWidget)
	{
		if (NextTabEnhancedInputAction)
		{
			NextTabActionWidget->SetEnhancedInputAction(NextTabEnhancedInputAction);
		}

		else if (false == NextTabInputActionData.IsNull())
		{
			NextTabActionWidget->SetInputAction(NextTabInputActionData);
		}
	}


	if (nullptr != PrevTabActionWidget)
	{
		if (PreviousTabEnhancedInputAction)
		{
			PrevTabActionWidget->SetEnhancedInputAction(PreviousTabEnhancedInputAction);
		}

		else if (false == PreviousTabInputActionData.IsNull())
		{
			PrevTabActionWidget->SetInputAction(PreviousTabInputActionData);
		}
	}
}


void UEpTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabButtonBox)
	{
		TabButtonBox->ClearChildren();
	}

	if (TabButtonGroup)
	{
		TabButtonGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &UEpTabListWidgetBase::OnSelectedTabButton);
	}

	SetListeningForInput(true);
	CreateTabs();
	RefreshTabAction();
}

void UEpTabListWidgetBase::NativeDestruct()
{
	//PreregisteredTabInfoArray.Reset();
	PendingTabLabelInfoMap.Reset();

	if (TabButtonGroup)
	{
		TabButtonGroup->OnSelectedButtonBaseChanged.RemoveDynamic(this, &UEpTabListWidgetBase::OnSelectedTabButton);
	}

	if (TabButtonBox)
	{
		TabButtonBox->ClearChildren();
	}

	Super::NativeDestruct();
}

void UEpTabListWidgetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	TabButtonBox->ClearChildren();

	if (bDebugPreview && TabButtonBox)
	{
		for (int i = 0; i < PreregisteredTabInfoArray.Num(); ++i)
		{
			FEpTabParameter Param = PreregisteredTabInfoArray[i];

			if (nullptr == Param.TabButton) continue;

			if (UCommonButtonBase* Button = WidgetTree->ConstructWidget(Param.TabButton))
			{
				NativeOnCreatedTabButton(i, Button, TabButtonBox);

				if (IEclipseTabButtonInterface* TabButtonInterface = Cast<IEclipseTabButtonInterface>(Button))
				{
					TabButtonInterface->NativeOnSetTabInfo(Param);
				}

				if (UEpBoundActionButton* ActionButton = Cast<UEpBoundActionButton>(Button))
				{
					ActionButton->SetButtonState(Param.PreviewButtonState);
				}
			}
		}
	}
#endif
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

	if (ensure(TabInfoPtr))
	{
		if (IEclipseTabButtonInterface* TabButtonInterface = Cast<IEclipseTabButtonInterface>(TabButton))
		{
			TabButtonInterface->NativeOnSetTabInfo(*TabInfoPtr);
		}

		PendingTabLabelInfoMap.Remove(TabId);


		int Index = PreregisteredTabInfoArray.IndexOfByPredicate([TabInfo = TabInfoPtr](const FEpTabParameter& InTabInfo) -> bool
			{
				return TabInfo && InTabInfo.TabActionTag.GetTagName() == TabInfo->TabActionTag.GetTagName();
			});


		NativeOnCreatedTabButton(Index, TabButton, TabButtonBox);
	}
}


void UEpTabListWidgetBase::NativeOnCreatedTabButton(uint8 Index, UCommonButtonBase* InButton, UPanelWidget* InPanelWidget)
{
	if (!ensure(TabButtonBox)) return;

	OnCreatedTabButton(Index, InButton, InPanelWidget);
}
