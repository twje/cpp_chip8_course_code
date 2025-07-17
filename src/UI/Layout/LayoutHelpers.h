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
    Top,
    Bottom
};

//--------------------------------------------------------------------------------
enum class HortAlign
{
    Left,
    Right
};

//--------------------------------------------------------------------------------
void PlaceRightOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align);
void PlaceLeftOf(IWidget& target, const IWidget& anchor, int32_t spacing, VertAlign align);
void PlaceBelow(IWidget& target, const IWidget& anchor, int32_t spacing, HortAlign align);