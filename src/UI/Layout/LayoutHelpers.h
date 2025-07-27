#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IWidget.h"

// System
#include <cstdint>

//--------------------------------------------------------------------------------
enum class VertAlign
{
    TOP,
    BOTTOM
};

//--------------------------------------------------------------------------------
enum class HortAlign
{
    LEFT,
    RIGHT
};

//--------------------------------------------------------------------------------
void PlaceRIGHTOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align);
void PlaceLEFTOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align);
void PlaceBelow(IWidget& target, const IWidget& anchor, int32_t spacing, HortAlign align);