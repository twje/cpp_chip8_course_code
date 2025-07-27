#include "UI/Layout/LayoutHelpers.h"

//--------------------------------------------------------------------------------
void PlaceRightOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align)
{
    const IntVec2 anchorPosition = anchor.GetPosition();
    const IntVec2 anchorSize = anchor.GetSize();
    const IntVec2 targetSize = target.GetSize();

    int32_t y = anchorPosition.mY;
    if (align == VertAlign::BOTTOM)
    {
        y += anchorSize.mY - targetSize.mY;
    }

    target.SetPosition({ anchorPosition.mX + anchorSize.mX + spacing, y });
}

//--------------------------------------------------------------------------------
void PlaceLeftOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align)
{
    const IntVec2 anchorPosition = anchor.GetPosition();
    const IntVec2 anchorSize = anchor.GetSize();
    const IntVec2 targetSize = target.GetSize();

    int32_t y = anchorPosition.mY;
    if (align == VertAlign::BOTTOM)
    {
        y += anchorSize.mY - targetSize.mY;
    }

    target.SetPosition({ anchorPosition.mX - targetSize.mX - spacing, y });
}

//--------------------------------------------------------------------------------
void PlaceBelow(IWidget& target, const IWidget& anchor, int32_t spacing, HortAlign align)
{
    const IntVec2 anchorPosition = anchor.GetPosition();
    const IntVec2 anchorSize = anchor.GetSize();
    const IntVec2 targetSize = target.GetSize();

    int32_t x = anchorPosition.mX;
    if (align == HortAlign::RIGHT)
    {
        x += anchorSize.mX - targetSize.mX;
    }

    target.SetPosition({ x, anchorPosition.mY + anchorSize.mY + spacing });
}