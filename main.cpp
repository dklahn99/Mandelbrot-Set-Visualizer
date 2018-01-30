#include <iostream>
#include <complex>
#include <math.h>
#include <tuple>
#include <stack>
#include "graphics.h"
#include <iostream>

using namespace std;

const int WINDOW_WIDTH = 3600;
const int WINDOW_HEIGHT = 2100;
const double WINDOW_RATIO = (double) WINDOW_HEIGHT / WINDOW_WIDTH;

const int NUM_ITERATIONS = 100;
const int POWER = 3;

struct NumberSpace {
	double xMin, xMax, yMin, yMax;
};

tuple<bool, int> inMandelbrot(double x, double y, int w, int h, int power = POWER, int iterations = NUM_ITERATIONS, double cutoff = 2.0); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
double sigmoid(double a);
void drawMandelbrot(NumberSpace region);
NumberSpace selectRegion(NumberSpace region);
void screenShot(string fileName);

int main() {

	Window& window = Window::getInstance(3600, 2100, false);
	SDL_Delay(100);

	window.flip();

	NumberSpace region = { 0, window.getWidth(), 0, window.getHeight() };
	for (Input input = getInputState(); !input.BUTTON_ESC; input = getInputState()) {
		window.refreshBackground();
		drawMandelbrot(region);
		window.flip();

		cout << "Done rendering" << endl;

		SDL_Delay(3000);

		region = selectRegion(region);

		cout << "New region: " << region.xMin << ", " << region.xMax << " ; " << region.yMin << ", " << region.yMax << endl;
	}

	return 0;
}

tuple<bool, int> inMandelbrot(double x, double y, int w, int h, int power, int iterations, double cutoff) { // @suppress("Type cannot be resolved") // @suppress("Symbol is not resolved")

	complex<double> z(0, 0);
	complex<double> c(x / w * 3.5 - 2.5, (y / h - 0.5) * 2);

	for (int i = 0; i < iterations; i++) {
		if (abs(z) >= cutoff) {
			return tuple<bool, int>(false, i);  // @suppress("Symbol is not resolved") @suppress("Function cannot be resolved")
		}

		z = pow(z, 3) + pow(z, 2) + c;
	}

	return tuple<bool, int>(true, iterations); // @suppress("Symbol is not resolved") @suppress("Function cannot be resolved")
}

double sigmoid(double a) {
	const int E = 2.718281828459;
	return 1.0 / (1.0 + pow(E, -a));
}

void drawMandelbrot(NumberSpace region) {
	static Window& window = Window::getInstance();

	static const Color BLUE = { 64, 0, 255 };
	static const Color RED = { 255, 0, 0 };
	static const Color YELLOW = { 255, 255, 0 };
	static const Color WHITE = { 255, 255, 255 };
	static const Color ORANGE = { 255, 200, 0 };
	static const Color BLACK = { 0, 0, 0 };

	const int windowWidth = window.getWidth();
	const int windowHeight = window.getHeight();
	const double regionWidth = region.xMax - region.xMin;
	const double regionHeight = region.yMax - region.yMin;

	int numIterations = log(NUM_ITERATIONS * windowWidth / regionWidth) * 25;

	cout << "num iter: " << numIterations << endl;

	for (int i = 0; i < windowWidth; i++) {
		for (int j = 0; j < windowHeight; j++) {
			double x = region.xMin + (double) i / windowWidth * regionWidth;
			double y = region.yMin + (double) j / windowHeight * regionHeight;

			tuple<bool, int> result = inMandelbrot(x, y, windowWidth, windowHeight, POWER, numIterations); // @suppress("Type cannot be resolved") // @suppress("Symbol is not resolved")

			int iterNum = get < 1 > (result);
			if (0 < iterNum && iterNum < numIterations / 5) {
				window.render(BLACK, { i, j, 1, 1 });
			} else if (iterNum < numIterations / 4) {
				window.render(WHITE, { i, j, 1, 1 });
			} else if (iterNum < numIterations / 3) {
				window.render(YELLOW, { i, j, 1, 1 });
			} else if (iterNum < numIterations / 2.2) {
				window.render(ORANGE, { i, j, 1, 1 });
			} else if (iterNum < numIterations) {
				window.render(RED, { i, j, 1, 1 });
			} else if (iterNum == numIterations) {
				window.render(BLACK, { i, j, 1, 1 });
			}

//			if (get < 0 > (result)) { // @suppress("Method cannot be resolved") // @suppress("Symbol is not resolved")
//				window.render((Color) {128, 0, 255}, {i, j, 1, 1}); // @suppress("Method cannot be resolved") // @suppress("Invalid arguments")
//			}
		}
	}
}

NumberSpace selectRegion(NumberSpace region) {
	static stack<NumberSpace> regionHistory;
	static Window& window = Window::getInstance();

	const int VOID = -69;
	const Color GREEN = { 0, 255, 0 };
	const Color GREY = { 128, 128, 128 };

	bool selectorEngaged = false;
	int lowestSelX = VOID, highestSelX = VOID, lowestSelY = VOID, highestSelY = VOID;
	Rect selectorPosition = { window.getWidth() / 2, window.getHeight() / 2, window.getWidth() / 500, window.getWidth() / 500 }; //Start selector in the middle of the screen
	const int SELECTOR_GRANULARITY = 8;

	SDL_Delay(500);
	for (Input input = getInputState(); !input.BUTTON_3; input = getInputState()) {

		if (input.BUTTON_2) { //If button two is pressed, return to the previous region
			NumberSpace output = regionHistory.top(); //pop the last region off the stack
			regionHistory.pop();
			return output;
		} else if (input.BUTTON_1) {
			selectorEngaged = !selectorEngaged; //toggle selector activity with button 1
			SDL_Delay(100);
		} else if (input.BUTTON_4) {
			string fileName = "Screenshots/" + to_string(POWER) + "__" + to_string(region.xMin) + "-" + to_string(region.xMax) + "__" + to_string(region.yMin) + "-" + to_string(region.yMax) + ".bmp";
			screenShot(fileName);
			cout << "Screen shot saved at " << fileName << endl;
		}

		bool moved = false;
		if (input.UP && selectorPosition.y > 0) {
			selectorPosition.y -= SELECTOR_GRANULARITY;
			if (selectorEngaged && (selectorPosition.y < lowestSelY || lowestSelY == VOID)) {
				lowestSelY = selectorPosition.y;
			}
			moved = true;
		} else if (input.DOWN && selectorPosition.y < window.getHeight()) {
			selectorPosition.y += SELECTOR_GRANULARITY;
			if (selectorEngaged && (selectorPosition.y > highestSelY || highestSelY == VOID)) {
				highestSelY = selectorPosition.y;
			}
			moved = true;
		} else if (input.LEFT && selectorPosition.x > 0) {
			selectorPosition.x -= SELECTOR_GRANULARITY;
			if (selectorEngaged && (selectorPosition.x < lowestSelX || lowestSelX == VOID)) {
				lowestSelX = selectorPosition.x;
			}
			moved = true;
		} else if (input.RIGHT && selectorPosition.x < window.getWidth()) {
			selectorPosition.x += SELECTOR_GRANULARITY;
			if (selectorEngaged && (selectorPosition.x > highestSelX || highestSelX == VOID)) {
				highestSelX = selectorPosition.x;
			}
			moved = true;
		}

		if (moved) {
			window.render(selectorEngaged ? GREEN : GREY, selectorPosition);
			window.flip();
		}

		SDL_Delay(30);
	}

	regionHistory.push(region);

	double pixelSpacingX = (region.xMax - region.xMin) / window.getWidth();
	double pixelSpacingY = (region.yMax - region.yMin) / window.getHeight();

	NumberSpace output;
	output.xMin = region.xMin + (lowestSelX * pixelSpacingX);
	output.xMax = region.xMin + (highestSelX * pixelSpacingX);
	output.yMin = region.yMin + (lowestSelY * pixelSpacingX);
	output.yMax = region.yMin + (highestSelY * pixelSpacingX);

	return output;
}

void screenShot(string fileName) {
	static Window& window = Window::getInstance();

	// Create an empty RGB surface that will be used to create the screenshot bmp file
	SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	if (pScreenShot) {
		// Read the pixels from the current render target and save them onto the surface
		SDL_RenderReadPixels(window.renderer, NULL, SDL_GetWindowPixelFormat(window.window), pScreenShot->pixels, pScreenShot->pitch);

		// Create the bmp screenshot file
		SDL_SaveBMP(pScreenShot, fileName.c_str());

		// Destroy the screenshot surface
		SDL_FreeSurface(pScreenShot);
	}
}
