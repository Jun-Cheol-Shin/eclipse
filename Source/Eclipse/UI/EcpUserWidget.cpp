// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpUserWidget.h"

void UEcpUserWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UEcpUserWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();



}

FReply UEcpUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

FReply UEcpUserWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

FReply UEcpUserWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

void UEcpUserWidget::NativeOnMouseLeave(const FPointerEvent& InEvent)
{

}
