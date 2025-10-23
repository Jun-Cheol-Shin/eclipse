// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPromptEntry.h"
#include "../Common/ActionCombineBox.h"
#include "CommonTextBlock.h"

void UInteractPromptEntry::Set(const FInteractActionParam& InParam)
{
	if (nullptr != ActionCombineBox)
	{
		ActionCombineBox->SetAction(InParam.Action);
	}

	if (nullptr != ActionDisplayText)
	{
		ActionDisplayText->SetText(InParam.DisplayText);
	}
}
