// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedInventoryEntry.h"

#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UGridBasedInventoryEntry::Set(int32 InRow, int32 InColumn, float InSlotSize, const TArray<int32>& InHidden)
{
    if (!CachedInstance) return;

    // 0~127 = 32비트*4 = RGBA
    uint32 W0 = 0, W1 = 0, W2 = 0, W3 = 0;

    for (int32 i : InHidden)
    {
        if (i < 0 || i >= 128) continue;       // A만 쓰므로 범위를 0..127로 제한
        const uint32 word = (uint32)i >> 5;    // 0..3 (각 32칸)
        const uint32 bit = (uint32)i & 31u;   // 0..31
        const uint32 m = 1u << bit;

        switch (word)
        {
        case 0: W0 |= m; break; // 0..31   -> R
        case 1: W1 |= m; break; // 32..63  -> G
        case 2: W2 |= m; break; // 64..95  -> B
        case 3: W3 |= m; break; // 96..127 -> A
        default: break;
        }
    }

    // uint32를 float로 비트 재해석해 Vector4로 패킹
    auto PackToFColor = [](uint32 a, uint32 b, uint32 c, uint32 d) -> FLinearColor
        {
            return FLinearColor(
                *reinterpret_cast<float*>(&a),
                *reinterpret_cast<float*>(&b),
                *reinterpret_cast<float*>(&c),
                *reinterpret_cast<float*>(&d));
        };

    CachedInstance->SetVectorParameterValue(TEXT("ExcludedBitsA"), PackToFColor(W0, W1, W2, W3));
    CachedInstance->SetScalarParameterValue(TEXT("TileX"), InRow);
    CachedInstance->SetScalarParameterValue(TEXT("TileY"), InColumn);
    CachedInstance->SetVectorParameterValue(TEXT("ExcludedBitsB"), FLinearColor::Black);

    if (SizeBox)
    {
        SizeBox->SetHeightOverride(InSlotSize * Column);
        SizeBox->SetWidthOverride(InSlotSize * Row);
    }
}

void UGridBasedInventoryEntry::NativeConstruct()
{
    Super::NativeConstruct();

    ExcludeHiddenIdx.Reset();
}

void UGridBasedInventoryEntry::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    if (nullptr == CachedInstance && nullptr != GridWidget)
    {
        CachedInstance = GridWidget->GetDynamicMaterial();
    }

    if (CachedInstance)
    {
        CachedInstance->SetTextureParameterValue(TEXT("Pattern"), SlotTexture);
    }


    if (IsDesignTime())
    {
        TArray<int> HiddenIndexList;

        for (FDoubleArrayIndexes& Indexes : ExcludeHiddenIdx)
        {
            int X = FMath::Clamp(Indexes.X, 0, Row);
            int Y = FMath::Clamp(Indexes.Y, 0, Column);

            HiddenIndexList.Emplace(X + Row * Y);
        }

        Set(Row, Column, SlotSize, HiddenIndexList);
    }
}
