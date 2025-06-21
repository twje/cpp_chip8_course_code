#include <iostream>

// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Application.h"
  
//--------------------------------------------------------------------------------
int main()
{
	Application app;
		
	const olc::vi2d canvasSize = app.GetCanvasSize();	
	const int32_t pixelScale = 2;
	
	app.Construct(canvasSize.x, canvasSize.y, pixelScale, pixelScale);	
	app.Start();

	return 0;
}