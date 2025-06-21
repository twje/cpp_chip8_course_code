#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Application.h"
  
//--------------------------------------------------------------------------------
int main()
{
	Application app;
	
	// The window size is fixed to fit all UI elements comfortably.
	// You can change this if you rearrange or add more widgets.
	app.Construct(640, 500, 2, 2);	
	app.Start();

	return 0;
}