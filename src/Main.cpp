#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Application.h"
  
//--------------------------------------------------------------------------------
int main()
{
	Application app;
	app.Construct(640, 500, 2, 2);
	app.Start();

	return 0;
}