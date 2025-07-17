#pragma once

// Includes
//--------------------------------------------------------------------------------
// Project
#include "Interpreter/Snapshot/Snapshot.h"

// System
#include <string>

// Forward Declarations
//--------------------------------------------------------------------------------
struct Bus;

//--------------------------------------------------------------------------------
struct ViewModel
{
	Bus* mBus = nullptr;
    Snapshot mSnapshot;
	std::string mNotficationText;
	bool mIsDisplayInteractive = false;
	bool mIsNotificationError = false;
};