// Includes
//--------------------------------------------------------------------------------
// Chip 8
#include "Application/Application.h"
#include "Constants.h"
  
//--------------------------------------------------------------------------------
int main()
{
	Application app;
	
	// Canvas size chosen to fit all UI widgets.
	app.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_PIXEL_SCALE, SCREEN_PIXEL_SCALE);
	app.Start();

	return 0;
}