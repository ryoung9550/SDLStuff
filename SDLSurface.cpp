#include <SDL2/SDL.h>
#include <string>
#include <cstdio>
#include <cmath>
#include <vector>

#define PI 3.14159265
#define H 600
#define W 800

static void drawPixel(SDL_Surface* surf, int xpos, int ypos, int red, int green, int blue) {
	if( xpos >= 0 && xpos < surf->w && ypos >= 0 && ypos < surf->h)
		((Uint32*)surf->pixels)[ypos * surf->pitch/4 + xpos] = SDL_MapRGB(surf->format, red, green, blue);
}

static void drawCheckered(SDL_Surface* surf, int red, int green, int blue) {
	bool odd = true;
	for( int row = 0; row < surf->h; ++row)
	{
		if(odd) {
			for( int col = 1; col < surf->w; col = col + 2)
				drawPixel(surf, col, row, red, green, blue);
			odd = false;
		} else {
			for( int col = 0; col < surf->w; col = col + 2)
				drawPixel(surf, col, row, red, green, blue);
			odd = true;
		}
	}
}

void swapNum(int &a, int &b) {
	int abuffer = a;
	a = b;
	b = abuffer;
}

static void drawLine(SDL_Surface* surf, int x1, int y1, int x2, int y2) {
	bool step = abs(x1 - x2) < abs(y1 - y2);
	if(step) { swapNum(x1, y1); swapNum(x2, y2); }
	if(x2 < x1) { swapNum(x1, x2); swapNum(y1, y2); }
	float error = 0.0;
	float slope = ((float) abs(y2-y1)) / ((float) (x2-x1));
	int y = y1;
	int ystep =  (y2>y1 ? 1 : -1); 
	for( int pixel = x1; pixel < x2; ++pixel)
	{
		if(step)
			drawPixel(surf, y, pixel, 255, 255, 255);
		else
			drawPixel(surf, pixel, y, 255, 255, 255);
		error+=slope;
		if(error  >= 0.5)
		{
			y+=ystep;
			error -= 1.0;
		}
	}
}

static void drawCircle(SDL_Surface* surf, const float &x, const float &y, const float &r) {
	int x1, x2, y1, y2;
	int precision = 2;
	for(int deg = precision; deg <= 360; deg = deg + precision)
	{
		x1 = cos((deg - precision) * PI / 180.0) * r + x;
		y1 = sin((deg - precision) * PI / 180.0) * r + y;
		x2 = cos(deg * PI / 180.0) * r + x;
		y2 = sin(deg * PI / 180.0) * r + y;
		drawLine(surf, x1, y1, x2, y2);
	}
}

static void drawCircleFill(SDL_Surface* surf, const int &x, const int &y, const int &r) {
	int x1, y1, y2;
	float precision = 50;
	drawCircle(surf, x, y, r);
	for(int deg = 1; deg < 180 * r / precision; ++deg)
	{
		x1 = cos(  ( (float) deg / r * precision) * PI / 180.0) * r + x;
		y1 = sin( ( (float) deg / r * precision) * PI /180.0) * r + y + 0.5;
		y2 = -(sin( ( (float) deg / r * precision) * PI / 180.0)) * r + y;

		drawLine(surf, x1, y1, x1, y2);
	}
}

static void quit(SDL_Window* window, std::vector<SDL_Surface*> surfaces) {
	SDL_DestroyWindow(window);
	for(int i = 0; i < (int) surfaces.size(); ++i) {
		SDL_FreeSurface(surfaces[0]);
		printf("Delted object %d\n", i);
	}
	/*
	for(auto it = surfaces.end(); it != surfaces.begin(); it--)
	{
		SDL_FreeSurface(*it);
		printf("Delted object\n");
	}
	*/
	SDL_Quit();
}

/*
static void drawLine(SDL_Surface* surf, int x1, int y1, int x2, int y2) {
	if( x1 > x2) { swapNum(x1, x2); swapNum(y1, y2); } 
	float dx = x2 - x1;
	float dy = y2 - y1;
	for(int pixel = x1; pixel <= x2; ++pixel) {
		drawPixel(surf, pixel, y1 + (pixel - x1) * dy / dx, 255, 255, 255);
	}
}
*/

SDL_Surface* loadSurface(std::string path, SDL_PixelFormat* format) {
	SDL_Surface* returnSurface = NULL;
	SDL_Surface* texture = SDL_LoadBMP(path.c_str());
	if(texture == NULL)
	{
		printf("Unable to load the image at %s. ERROR: %s", path.c_str(), SDL_GetError());
	} else {
		returnSurface = SDL_ConvertSurface(texture, format, 0);
		if( returnSurface == NULL)
		{
			printf("Unable to optimize image. ERROR: %s\n", SDL_GetError());
		}
	}
	SDL_FreeSurface(texture);
	return returnSurface;
}

static bool loadMedia(SDL_Surface* textureBuffer[], SDL_Surface* window) {
	bool success = true;

	textureBuffer[0] = loadSurface("./pizzahut.bmp", window->format);
	if(textureBuffer[0] == NULL){ success = false; }
	textureBuffer[1] = loadSurface("./red.bmp", window->format);
	if(textureBuffer[1] == NULL) { success = false; }
	textureBuffer[2] = loadSurface("./blue.bmp", window->format);
	if(textureBuffer[2] == NULL) { success = false; }


	return success;
}

static void Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
}

int main()
{
	Init(); // Initialize SDL2

	// Creates main window for the program
	SDL_Window* window = SDL_CreateWindow("Lol chicken shit",
												SDL_WINDOWPOS_UNDEFINED, // x window pos
												SDL_WINDOWPOS_UNDEFINED, // y window pos
												W, // width
												H, // height
												0);

	// This is the surface that will be associated with what is seen in the window
	SDL_Surface* screenWindow = SDL_GetWindowSurface(window);
	if( screenWindow == NULL)
	{
		printf("\"screenWindow\" could not be created! ERROR: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// This is the surface that will be a buffer before it goes into the main display surface
	SDL_Surface* screenBuffer = SDL_ConvertSurface(screenWindow, screenWindow->format, 0);
	if( screenBuffer == NULL )
	{
		printf("\"screenBuffer\" could not be created! ERROR: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	const int numTextureElms = 3; // Number of Textures in the program
	SDL_Surface* textures[numTextureElms]; // Surface array which holds all the textures for the program
	std::vector<SDL_Surface*> allSurfaces; // Vector which holds all surfaces in the program

	// Loads all textures for the program into the textures surface pointer array
	loadMedia(textures, screenWindow);

	// Adding all surfaces to allSurfaces vector
	allSurfaces.push_back(screenWindow); 
	allSurfaces.push_back(screenBuffer);
	for( int i = 0; i < numTextureElms; ++i)
		allSurfaces.push_back(textures[i]);
	printf("The number of all textures: %ld\n", allSurfaces.size());
	
	int mx=10, my=10; // Starting mouse Coord 
	const int FPS = 60; // Fixed framerate of the window
	bool running = true; // Game loop condiditon (false closes the program)
	bool keyPressed = false; // States if a keyboard key has been pressed

	// Rectangle that will be drawn
	SDL_Rect rect;
	rect.x = 20;
	rect.y = 20;
	rect.w = 20;
	rect.h = 20;
	//const Uint32 colorBlue = SDL_MapRGB(screenWindow->format, 0x0, 0xff, 0xff);  

	// Main Game loop
	while(running) 
	{
		Uint32 startTick = SDL_GetTicks(); // Marks the begining of the cycle

		// Clears screenWindow
		/*
		if(SDL_FillRect(screenWindow, NULL, 0))
		{
			printf("Window Screen could not be cleared! ERROR: %s\n", SDL_GetError());
		}
		if(SDL_FillRect(screenBuffer, NULL, 0))
		{
			printf("Screen Buffer could not be cleared! ERROR: %s\n", SDL_GetError());
		}
		*/

		// Event tracker
		SDL_Event event;
		while(SDL_PollEvent(&event)) // Analyze input
		{
			switch(event.type) // Status of the program
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEMOTION:
					mx = event.motion.x;
					my = event.motion.y;
					break;	
			}
			if( event.type == SDL_KEYDOWN ) // Input from the keyboard
			{
				switch(event.key.keysym.sym)
				{
					default:
						keyPressed = true;
					case SDLK_w:
						screenBuffer = textures[0];
						printf("W is pressed down\n");
						break;
					case SDLK_a:
						screenBuffer = textures[1];
						printf("A is pressed down\n");
						break;
					case SDLK_d:
						screenBuffer = textures[2];
						printf("D is pressed down\n");
						break;
					case SDLK_ESCAPE:
						running = false;
						break;
				}
			} else { keyPressed = false; }
		}

		// SDL_LockSurface(screenBuffer); // Protects the surface from edits this instance

		

		// Draws pixel onto screen
		/*
		if(!keyPressed)	
		{
		SDL_FillRect(screenBuffer, &rect, colorBlue);
		drawLine(screenBuffer, mx, my, 400, 300);
		// drawCircle(screenBuffer, mx, my, 200);
		drawCircleFill(screenBuffer, mx, my, 300);
		}
		*/

		// SDL_UnlockSurface(screenWindow); // Finalizes edits to surface

		SDL_BlitSurface(screenBuffer, NULL, screenWindow, NULL);

		SDL_UpdateWindowSurface(window); // Pushes the surface buffer to window

		Uint32 cycleTime = SDL_GetTicks()-startTick;
		if(1000/FPS > cycleTime)
			SDL_Delay(1000/FPS - cycleTime);
	}

	SDL_Delay(100);
	quit(window, allSurfaces);

	return 0;
}
