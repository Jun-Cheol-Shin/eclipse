// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedFootprint.h"

#include "Components/Image.h"

void UGridBasedFootprint::SetStyle(const FFootprintStyle& InStyle)
{
    if (CachedInstance)
    {
        if (InStyle.FootprintTexture)
        {
            CachedInstance->SetTextureParameterValue(TEXT("Pattern"), InStyle.FootprintTexture);
        }

        if (FootprintImage)
        {
            FootprintImage->SetBrushTintColor(InStyle.FootprintColor);
        }
    }
}

void UGridBasedFootprint::SetFootprint(int InX, int InY, const TArray<int>& InHiddenIndex)
{
    if (!CachedInstance) return;

    uint32 W0 = 0, W1 = 0, W2 = 0, W3 = 0;

    for (int32 i : InHiddenIndex)
    {
        if (i < 0 || i >= 128) continue; 
        const uint32 word = (uint32)i >> 5;    // 0..3
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

    // uint32�� float�� ��Ʈ ���ؼ��� Vector4�� ��ŷ
    auto PackToFColor = [](uint32 a, uint32 b, uint32 c, uint32 d) -> FLinearColor
        {
            return FLinearColor(
                *reinterpret_cast<float*>(&a),
                *reinterpret_cast<float*>(&b),
                *reinterpret_cast<float*>(&c),
                *reinterpret_cast<float*>(&d));
        };

    CachedInstance->SetVectorParameterValue(TEXT("ExcludedBitsA"), PackToFColor(W0, W1, W2, W3));
    CachedInstance->SetScalarParameterValue(TEXT("TileX"), InY);
    CachedInstance->SetScalarParameterValue(TEXT("TileY"), InX);
    CachedInstance->SetVectorParameterValue(TEXT("ExcludedBitsB"), FLinearColor::Black);
}

void UGridBasedFootprint::NativeConstruct()
{
	Super::NativeConstruct();

}

void UGridBasedFootprint::SynchronizeProperties()
{
	Super::SynchronizeProperties();

    if (nullptr == CachedInstance && nullptr != FootprintImage)
    {
        CachedInstance = FootprintImage->GetDynamicMaterial();
    }


#if WITH_EDITOR
    if (IsDesignTime())
    {
        TArray<int> HiddenIndexList;

        for (const FIntPoint& Indexes : ExcludeHiddenIdx)
        {
            int X = FMath::Clamp(Indexes.X, 0, Row);
            int Y = FMath::Clamp(Indexes.Y, 0, Column);

            HiddenIndexList.Emplace(X + Row * Y);
        }

        SetStyle(PreviewStyle);
        SetFootprint(Row, Column, HiddenIndexList);
    }

#endif
}
