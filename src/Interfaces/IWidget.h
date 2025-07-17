#pragma once

// Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Types/ExecutionState.h"
#include "UI/ViewModel.h"
#include "Types/IntVec2.h"

//--------------------------------------------------------------------------------
class IWidget
{
public:
	virtual ~IWidget() = default;
	
	virtual IntVec2 GetSize() const = 0;
	virtual IntVec2 GetPosition() const = 0;
	virtual void SetPosition(const IntVec2& position) = 0;
	
	virtual void Update(float elapsedTime) = 0;
	virtual void Draw(const ViewModel& viewModel) = 0;
};