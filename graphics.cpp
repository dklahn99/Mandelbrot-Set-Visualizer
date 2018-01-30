#include "graphics.h"

void graphicsInit() {
	static bool wasInit = false; //Static variable to store whether or not the library is already initialized. Maintains state between function calls

	if (!wasInit) { //Check if library already initialized

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) { //Attempt to initialize SDL
			throw sdl_error(std::string("Error initializing SDL: ") += SDL_GetError()); //Throw exception if SDL cannot initialize
		}

		wasInit = true;
	}
}

Window& Window::getInstance(const int& windowWidth, const int& windowHeight, const bool& fullscreen) {
	graphicsInit();
	static Window window(windowWidth, windowHeight, fullscreen);
	return window;
}

Window::Window(const int windowWidth, const int windowHeight, const bool fullscreen) {
	//set window parameters
	this->simulatedWindowWidth = windowWidth;
	this->simulatedWindowHeight = windowHeight;
	this->fullscreen = fullscreen;

	//get display mode
	SDL_DisplayMode currentDisplayMode;
	SDL_GetCurrentDisplayMode(0, &currentDisplayMode);

	if (fullscreen) { //enter fullscreen mode

		SDL_ShowCursor(SDL_DISABLE); //Hide the mouse pointer
		//int simulatedDisplayWidth = currentDisplayMode.w; //Calculate the actual width of the fullscreen window (accounts for LEFT_UNDERSCAN)

		float scaleFactorX = currentDisplayMode.w / (float) simulatedWindowWidth; //calculate x scaling
		float scaleFactorY = currentDisplayMode.h / (float) simulatedWindowHeight; //calculate y scaling

		scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorY : scaleFactorX; //Use the smaller scale factor. This expands the window until it hits an edge of the screen

		//Calculate the actual pixel size of the fullscreen window
		realWindowWidth = simulatedWindowWidth * scaleFactor;
		realWindowHeight = simulatedWindowHeight * scaleFactor;

		//Calculate x and y offsets
		xOffset = (currentDisplayMode.w - realWindowWidth) / 2.0;
		yOffset = (currentDisplayMode.h - realWindowHeight) / 2.0;

		//Calculate black edge positions
		if (scaleFactorX > scaleFactorY) { //If sides are black
			edge1 = {
				0,
				0,
				xOffset,
				currentDisplayMode.h + 3 //Add three to account for rounding errors that leave out pixels on the edge
			};

			edge2 = {
				xOffset + realWindowWidth,
				0,
				xOffset,
				currentDisplayMode.h + 3
			};
		}
		else { //If top and bottom are black
			edge1 = {
				0,
				0,
				currentDisplayMode.w + 3,
				yOffset
			};

			edge2 = {
				0,
				yOffset + realWindowHeight,
				currentDisplayMode.w + 3,
				yOffset
			};
		}

		window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, currentDisplayMode.w, currentDisplayMode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
	} else { //enter windowed mode
		scaleFactor = 1; //No scaling
		xOffset = yOffset = 0; //No offsets
		window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, simulatedWindowWidth, simulatedWindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
		realWindowWidth = simulatedWindowWidth;
		realWindowHeight = simulatedWindowHeight;
	}

	if (window == NULL) { //Check for errors creating window
		throw sdl_error("Error creating the window: " + std::string(SDL_GetError())); //Throw an error if the window was not correctly initialized
		return; //exit
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //create renderer
	if (renderer == NULL) { //Check for errors creating renderer
		throw sdl_error("Error creating the renderer: " + std::string(SDL_GetError()));
		return; //exit
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

Window::~Window() {
	if (window) { //if window is not a NULL pointer
		SDL_DestroyWindow(window); //free memory
	}

	SDL_ShowCursor(SDL_ENABLE); //Show mouse pointer
}

void Window::render(Color color, Rect destination) {

	destination.x *= scaleFactor;
	destination.y *= scaleFactor;
	destination.w *= scaleFactor;
	destination.h *= scaleFactor;

	destination.x += xOffset;
	destination.y += yOffset;

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0);
	SDL_RenderFillRect(renderer, &destination);
}

void Window::refreshBackground() {
	static const Rect WHOLE_SCREEN = {0, 0, realWindowWidth, realWindowHeight};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	SDL_RenderFillRect(renderer, &WHOLE_SCREEN);
}

void Window::flip() {
	SDL_RenderPresent(renderer);
}

int Window::getWidth() {
	return simulatedWindowWidth;
}

int Window::getHeight() {
	return simulatedWindowHeight;
}

sdl_error::sdl_error(const std::string& errorString) {
	this->errorString = errorString;
}

const char* sdl_error::what() {
	return errorString.c_str();
}

Input getInputState() {
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
	}
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	Input output;
	output.UP = currentKeyStates[SDL_GetScancodeFromKey(SDLK_UP)];
	output.DOWN = currentKeyStates[SDL_GetScancodeFromKey(SDLK_DOWN)];
	output.LEFT = currentKeyStates[SDL_GetScancodeFromKey(SDLK_LEFT)];
	output.RIGHT = currentKeyStates[SDL_GetScancodeFromKey(SDLK_RIGHT)];
	output.BUTTON_1 = currentKeyStates[SDL_GetScancodeFromKey(SDLK_SPACE)];
	output.BUTTON_2 = currentKeyStates[SDL_GetScancodeFromKey(SDLK_z)];
	output.BUTTON_3 = currentKeyStates[SDL_GetScancodeFromKey(SDLK_RETURN)];
	output.BUTTON_4 = currentKeyStates[SDL_GetScancodeFromKey(SDLK_s)];

	output.BUTTON_ESC = currentKeyStates[SDL_GetScancodeFromKey(SDLK_ESCAPE)];

	return output;
}
