// Fill out your copyright notice in the Description page of Project Settings.


#include "Toast/ToastMessageTextUI.h"

void UToastMessageTextUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (nullptr != FadeOut)
	{
		FadeOut;
	}

}

void UToastMessageTextUI::NativeDestruct()
{
	Super::NativeDestruct();
}

void UToastMessageTextUI::OnCompleteFadeInAnimation()
{
}
