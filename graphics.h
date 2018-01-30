#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <SDL2/SDL.h>
#include <exception>
#include <string>
#include <iostream>

typedef SDL_Rect Rect;

struct Color {
	int r, g, b;
};

void graphicsInit();

class Window {

	friend void screenShot(std::string fileName);

public:
	static Window& getInstance(const int& windowWidth = 0, const int& windowHeight = 0, const bool& fullscreen = false);
	~Window();

	void render(Color color, Rect destination);
	void refreshBackground();
	void flip();

	int getWidth();
	int getHeight();
private:
	//Enforce singleton status
	Window(const int windowWidth = 0, const int windowHeight = 0, const bool fullscreen = false);
	void operator=(const Window&) = delete;
	Window(const Window&) = delete;

	int simulatedWindowWidth; //Holds the width in pixels of the window
	int simulatedWindowHeight; //Holds the height in pixels of the window
	int realWindowWidth;
	int realWindowHeight;
	bool fullscreen; //Is TRUE if the window is in fullscreen mode

	Rect edge1;
	Rect edge2;

	SDL_Window* window; //Holds a pointer to the SDL_Window object used in the background
	SDL_Renderer* renderer;

	float scaleFactor;
	int xOffset;
	int yOffset;
};

class sdl_error: public std::exception {
public:
	sdl_error(const std::string& errorString);
	const char* what();
private:
	std::string errorString;
};

struct Input {
	bool BUTTON_1;
	bool BUTTON_2;
	bool BUTTON_3;
	bool BUTTON_4;
	bool UP;
	bool DOWN;
	bool LEFT;
	bool RIGHT;

	bool BUTTON_ESC;
};

Input getInputState();

#endif /* GRAPHICS_H_ */
