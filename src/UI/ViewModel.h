#pragma once

// Includes
//--------------------------------------------------------------------------------
#include "Interpreter/Snapshot/Snapshot.h"

// Forward Declarations
//--------------------------------------------------------------------------------
struct Bus;

//--------------------------------------------------------------------------------
struct ViewModel
{
	Bus* mBus = nullptr;
	bool mIsDisplayInteractive = false;
    Snapshot mSnapshot;
};