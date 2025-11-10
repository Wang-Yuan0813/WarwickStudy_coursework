#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "GamesEngineeringBase.h"
#include <chrono>
#include <thread>
#include <math.h>
#include <queue>
#include <fstream>
#include <time.h>
#define DEBUG_MODE 0;
void timingus(size_t us) {
	auto start = std::chrono::system_clock::now();
	while (true) {
		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		if (duration.count() > us) break;
	}
}
template <typename T>
class Vector2D {
public:
	T x;
	T y;
	Vector2D(T _x = 0, T _y = 0) : x(_x), y(_y) {}
	Vector2D operator+(const Vector2D<T>& a)const {
		return Vector2D<T>(x + a.x, y + a.y);
	}
	Vector2D operator-(const Vector2D<T>& a)const {
		return Vector2D<T>(x - a.x, y - a.y);
	}
	Vector2D operator/(const T a)const {
		return Vector2D<T>(x / a, y / a);
	}
	Vector2D operator*(const T a)const {
		return Vector2D<T>(x * a, y * a);
	}
	Vector2D<int> toInt() {
		return Vector2D<int>(static_cast<int>(x), static_cast<int>(y));
	}
	Vector2D<float> toFloat() {
		return Vector2D<float>(static_cast<float>(x), static_cast<float>(y));
	}
};
Vector2D<int> camera_offset(0, 0);
Vector2D<int> canvasSize(0, 0);
Vector2D<int> cameratoEgde(400, 300);
Vector2D<int> worldSize(0, 0);
Vector2D<int> worldTopLeft(0, 0);
class CameraEffect {
private:

	int cameraShakingDuration = 0;
	int cameraShakingLevel = 0;
	Vector2D<int> cameraShakingOffset;
public:
	void shakeCamera(int level, int _duration) {
		cameraShakingDuration = _duration;
		cameraShakingLevel = level;
	}

	void shakeCameraFrameEnd() {
		if (cameraShakingDuration <= 0)	return;
		cameraShakingDuration--;
		camera_offset = camera_offset - cameraShakingOffset;
		cameraShakingOffset = Vector2D<int>(0, 0);
	}
	void cameraEffect() {
		if (cameraShakingDuration == 0)	return;
		else {
			cameraShakingOffset.x = rand() % cameraShakingLevel - cameraShakingLevel;
			cameraShakingOffset.y = rand() % cameraShakingLevel - cameraShakingLevel;
			camera_offset = camera_offset + cameraShakingOffset;
			return;
		}
	}
	bool cameraEdgeCheck_H(Vector2D<int> pos) {//return true if camera can move
		if (abs(pos.x - (worldTopLeft.x - 16)) <= cameratoEgde.x || abs(pos.x - (worldTopLeft.x + worldSize.x - 16)) <= cameratoEgde.x) {//horizontal
			return true;
		}
		return false;
	}bool cameraEdgeCheck_V(Vector2D<int> pos) {//return true if camera can move
		if (abs(pos.y - (worldTopLeft.y + worldSize.y - 16)) <= cameratoEgde.y) {//horizontal
			return true;
		}
		return false;
	}
};
class Move {
public:
	static Vector2D<float> playerMove(int horizon, int vertical, float speed) {

		if (horizon != 0 && vertical != 0) speed = speed / sqrt(2);//diagonal movement adjust
		Vector2D<float> movement(horizon * speed, vertical * speed);
		return movement;
	}
	static Vector2D<float> moveDirection(const Vector2D<float>& src, const Vector2D<float>& dst) {
		float x = dst.x - src.x;
		float y = dst.y - src.y;
		float a = sqrt((x * x + y * y));
		return Vector2D<float>(x / a, y / a);
	}
	static Vector2D<float> moveDirection(int angle) {
		return Vector2D<float>(cos(angle), sin(angle));
	}
};

#define CapitalSpacing 6
#define LowercaseSpacing 4
#define NumberSpacing 5
#define Spacing 3
GamesEngineeringBase::Image fonts[71];
GamesEngineeringBase::Image playerIcon;
GamesEngineeringBase::Image bar[3];
GamesEngineeringBase::Image barDecorations[3];
GamesEngineeringBase::Image buttonImage[6];
GamesEngineeringBase::Image menu;
GamesEngineeringBase::Image panel;
class Draw {
private:
	//undone
	static unsigned char* sumAlpha(float alpha, unsigned char* a, unsigned char* b) {
		unsigned char res[3] = { 0, 0, 0 };
		for (int i = 0; i < 3; i++) {
			res[i] = a[i] * (1 - alpha) + b[i] * alpha;
		}
		//std::cout << (int)res[0] << (int)res[1] << (int)res[2] << std::endl;
		return res;
	}
public:
	//GamesEngineeringBase::Image fonts[66];//fonts now we have 66 pics
	static void fontsInit(std::string folderpath) {
		for (int i = 'A'; i <= 'Z'; i++) {
			std::string filename = folderpath + '/';
			filename += i;
			filename += ".png";
			std::cout << "Loading font: " << filename << std::endl;
			fonts[i - 'A'].load(filename);
		}
		for (int i = 'a'; i <= 'z'; i++) {
			std::string filename = folderpath + "/_";
			filename += i;
			filename += ".png";
			std::cout << "Loading font: " << filename << std::endl;
			fonts[i - 'a' + 26].load(filename);
		}
		for (int i = '0'; i <= '9'; i++) {
			std::string filename = folderpath + '/';
			filename += i;
			filename += ".png";
			std::cout << "Loading font: " << filename << std::endl;
			fonts[i - '0' + 52].load(filename);
		}
		//load special characters,0 - ',' | 1 - '.' | 2 - '!' | 3 - '?' | 4 - '-' | 5 - '+' | 6 - '(' | 7 - ')'
		for (int i = '0'; i <= '8'; i++) {
			std::string filename = folderpath + "/_";
			filename += i;
			filename += ".png";
			std::cout << "Loading font: " << filename << std::endl;
			fonts[i - '0' + 62].load(filename);
		}
	}
	static void uiInit(std::string folderpath) {
		folderpath = folderpath + '/';
		std::string filename;
		for (int i = 0; i < 3; i++) {
			filename = folderpath + "bar" + std::to_string(i) + ".png";
			bar[i].load(filename);
			std::cout << "Loading ui: " << filename << std::endl;
			filename = folderpath + "bardecoration" + std::to_string(i) + ".png";
			barDecorations[i].load(folderpath + "bardecoration" + std::to_string(i) + ".png");
			std::cout << "Loading ui: " << filename << std::endl;
		}
		filename = folderpath + "playericon.png";
		playerIcon.load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "back.png";
		buttonImage[0].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "continue.png";
		buttonImage[1].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "mainmenu.png";
		buttonImage[2].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "newgame.png";
		buttonImage[3].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "quit.png";
		buttonImage[4].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "resume.png";
		buttonImage[5].load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "menu.png";
		menu.load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		filename = folderpath + "panel.png";
		panel.load(filename);
		std::cout << "Loading ui: " << filename << std::endl;

		return;
	}
	static void drawImage(GamesEngineeringBase::Window& canvas, const GamesEngineeringBase::Image& image,
		Vector2D<int> pos, unsigned int scale = 1, bool horizonfilp = false) {
		int offset_x, offset_y;
		offset_x = pos.x - image.width * scale / 2;
		offset_y = pos.y - image.height * scale / 2;
		for (unsigned int i = 0; i < image.height * scale; i++) // Loop through the image height
		{
			for (unsigned int j = 0; j < image.width * scale; j++) { // Loop through the image width
				unsigned int n = j;
				if (horizonfilp)	n = image.width * scale - j;
				if ((offset_x + j) >= 0 && (offset_x + j) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
					if (image.alphaAt(n / scale, i / scale) > 1)
						canvas.draw(offset_x + j, offset_y + i, image.at(n / scale, i / scale));
			}

		}
	}
	static void drawImage_Screen(GamesEngineeringBase::Window& canvas, const GamesEngineeringBase::Image& image,
		Vector2D<int> pos, int scale = 1, float alpha = 1.f, bool horizonfilp = false) {
		unsigned char* imageBuffer = canvas.backBuffer();
		int index = 0;
		for (int i = 0; i <= image.height * scale; i++)
			for (int j = 0; j <= image.width * scale; j++) {
				if (alpha == 1) {
					int n = j;
					if (horizonfilp)	n = image.width * scale - j;
					if ((pos.x - static_cast<int>(image.width * scale / 2) + j) >= 0 && (pos.x - static_cast<int>(image.width * scale / 2) + j) < canvasSize.x && (pos.y - static_cast<int>(image.height * scale / 2) + i) >= 0 && (pos.y - static_cast<int>(image.height * scale / 2) + i) < canvasSize.y)
						if (image.alphaAt(n / scale, i / scale) > 1)
							canvas.draw(pos.x - static_cast<int>(image.width * scale / 2) + j, pos.y - static_cast<int>(image.height * scale / 2) + i, image.at(n / scale, i / scale));
				}
				else if (alpha == 0) {
					return;
				}
				else {
					int n = j;
					if (horizonfilp)	n = image.width * scale - j;
					//index = ((i * canvasSize.x) + n) * 3;
					index = (((pos.y - static_cast<int>(image.height * scale / 2) + i) * canvasSize.x) + (pos.x - static_cast<int>(image.width * scale / 2) + j)) * 3;
					if ((pos.x - static_cast<int>(image.width * scale / 2) + j) >= 0 && (pos.x - static_cast<int>(image.width * scale / 2) + j) < canvasSize.x && (pos.y - static_cast<int>(image.height * scale / 2) + i) >= 0 && (pos.y - static_cast<int>(image.height * scale / 2) + i) < canvasSize.y)
						if (image.alphaAt(n / scale, i / scale) > 1)
							canvas.draw(pos.x - static_cast<int>(image.width * scale / 2) + j, pos.y - static_cast<int>(image.height * scale / 2) + i,
								*(imageBuffer + index) * (1 - alpha) + *image.at(n / scale, i / scale) * alpha, *(imageBuffer + index + 1) * (1 - alpha) + *(image.at(n / scale, i / scale) + 1) * alpha, *(imageBuffer + index + 2) * (1 - alpha) + *(image.at(n / scale, i / scale) + 2) * alpha);
				}
			}
	}
	static void drawWhiteImage(GamesEngineeringBase::Window& canvas, const GamesEngineeringBase::Image& image,
		Vector2D<int> pos, unsigned int scale = 1, bool horizonfilp = false) {
		int offset_x, offset_y;
		offset_x = pos.x - image.width * scale / 2;
		offset_y = pos.y - image.height * scale / 2;
		for (unsigned int i = 0; i < image.height * scale; i++) // Loop through the image height
		{
			for (unsigned int j = 0; j < image.width * scale; j++) { // Loop through the image width
				unsigned int n = j;
				if (horizonfilp)	n = image.width * scale - j;
				if ((offset_x + j) >= 0 && (offset_x + j) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
					if (image.alphaAt(n / scale, i / scale) > 1)
						canvas.draw(offset_x + j, offset_y + i, 255, 255, 255); // Draw white pixel
			}

		}
	}
	//draw text at screen (x,y) using loaded fonts
	static void drawText(GamesEngineeringBase::Window& canvas, std::string text, Vector2D<int> pos, unsigned int scale = 1,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) {
		int offset_x = pos.x;
		int offset_y = pos.y;
		for (char& c : text) {
			if (c >= 'A' && c <= 'Z') {
				for (unsigned int i = 0; i < fonts[c - 'A'].height * scale; i++) // Loop through the image height
				{
					for (unsigned int n = 0; n < fonts[c - 'A'].width * scale; n++) // Loop through the image width
					{
						// Ensure drawing stays within canvas bounds
						if ((offset_x + n) >= 0 && (offset_x + n) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
						{
							if (fonts[c - 'A'].alphaAt(n / scale, i / scale) > 100)
							{
								canvas.draw(offset_x + n, offset_y + i, r, g, b); // Draw the plane's pixel onto the canvas
							}
						}
					}
				}
				offset_x += CapitalSpacing * scale;
			}
			else if (c >= 'a' && c <= 'z') {
				for (unsigned int i = 0; i < fonts[c - 'a' + 26].height * scale; i++) // Loop through the image height
				{
					for (unsigned int n = 0; n < fonts[c - 'a' + 26].width * scale; n++) // Loop through the image width
					{
						// Ensure drawing stays within canvas bounds
						if ((offset_x + n) >= 0 && (offset_x + n) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
						{
							if (fonts[c - 'a' + 26].alphaAt(n / scale, i / scale) > 100)
							{
								canvas.draw(offset_x + n, offset_y + i, r, g, b); // Draw the plane's pixel onto the canvas
							}
						}
					}
				}
				offset_x += LowercaseSpacing * scale;
			}
			else if (c >= '0' && c <= '9') {
				for (unsigned int i = 0; i < fonts[c - '0' + 52].height * scale; i++) // Loop through the image height
				{
					for (unsigned int n = 0; n < fonts[c - '0' + 52].width * scale; n++) // Loop through the image width
					{
						// Ensure drawing stays within canvas bounds
						if ((offset_x + n) >= 0 && (offset_x + n) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
						{
							if (fonts[c - '0' + 52].alphaAt(n / scale, i / scale) > 100)
							{
								canvas.draw(offset_x + n, offset_y + i, r, g, b); // Draw the plane's pixel onto the canvas
							}
						}
					}
				}
				offset_x += NumberSpacing * scale;
			}
			else {
				bool spaceChar = false;
				switch (c) {
				case ',':
					c = 62;
					break;
				case '.':
					c = 63;
					break;
				case '!':
					c = 64;
					break;
				case '?':
					c = 65;
					break;
				case '-':
					c = 66;
					break;
				case '+':
					c = 67;
					break;
				case '(':
					c = 68;
					break;
				case ')':
					c = 69;
					break;
				case ':':
					c = 70;
					break;
				case ' ':
					spaceChar = true;
					offset_x += Spacing * scale; //space width
					break;
				default:
					c = 65; //default to '?'
					break;
				}
				if (!spaceChar) {
					for (unsigned int i = 0; i < fonts[c].height * scale; i++) // Loop through the image height
					{
						for (unsigned int n = 0; n < fonts[c].width * scale; n++) // Loop through the image width
						{
							// Ensure drawing stays within canvas bounds
							if ((offset_x + n) >= 0 && (offset_x + n) < canvasSize.x && (offset_y + i) >= 0 && (offset_y + i) < canvasSize.y)
							{
								if (fonts[c].alphaAt(n / scale, i / scale) > 100)
								{
									canvas.draw(offset_x + n, offset_y + i, r, g, b); // Draw the plane's pixel onto the canvas
								}
							}
						}
					}
					offset_x += NumberSpacing * scale; //special character width
				}

			}
		}
	}
	static void drawCrosshair(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, unsigned int size = 5,
		unsigned char r = 255, unsigned char g = 0, unsigned char b = 0) {
		pos = pos + canvasSize / 2 - camera_offset;
		for (int i = -static_cast<int>(size); i <= static_cast<int>(size); i++) {
			if ((pos.x + i) >= 0 && (pos.x + i) < canvasSize.x && (pos.y) >= 0 && (pos.y) < canvasSize.y)
				canvas.draw(pos.x + i, pos.y, r, g, b);
			if ((pos.x) >= 0 && (pos.x) < canvasSize.x && (pos.y + i) >= 0 && (pos.y + i) < canvasSize.y)
				canvas.draw(pos.x, pos.y + i, r, g, b);
		}
	}
	static void drawSquare(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, unsigned int size = 4,
		unsigned char r = 255, unsigned char g = 255, unsigned char b = 0) {
		pos = pos + canvasSize / 2 - camera_offset;
		for (int i = -static_cast<int>(size / 2); i <= static_cast<int>(size / 2); i++)
			for (int j = -static_cast<int>(size / 2); j <= static_cast<int>(size / 2); j++) {
				if ((pos.x + i) >= 0 && (pos.x + i) < canvasSize.x && (pos.y + j) >= 0 && (pos.y + j) < canvasSize.y)
					canvas.draw(pos.x + i, pos.y + j, r, g, b);
			}
	}
	static void drawSquare(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, Vector2D<int> size,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) {
		pos = pos + canvasSize / 2 - camera_offset;
		for (int i = -static_cast<int>(size.x / 2); i <= static_cast<int>(size.x / 2); i++)
			for (int j = -static_cast<int>(size.y / 2); j <= static_cast<int>(size.y / 2); j++) {
				if ((pos.x + i) >= 0 && (pos.x + i) < canvasSize.x && (pos.y + j) >= 0 && (pos.y + j) < canvasSize.y)
					canvas.draw(pos.x + i, pos.y + j, r, g, b);
			}
	}
	static void drawOval(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, Vector2D<int> size,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, float alpha = 1.f) {
		pos = pos + canvasSize / 2 - camera_offset;
		unsigned char* imageBuffer = canvas.backBuffer();
		int index;
		for (int i = -size.y / 2; i <= size.y / 2; i++) {
			for (int j = -size.x / 2; j <= size.x / 2; j++) {
				if (alpha == 1) {
					if (j * j * size.y * size.y / 4 + i * i * size.x * size.x / 4 <= size.x * size.x * size.y * size.y / 16) {
						if ((pos.x + j) >= 0 && (pos.x + j) < canvasSize.x && (pos.y + i) >= 0 && (pos.y + i) < canvasSize.y)
							canvas.draw(pos.x + j, pos.y + i, r, g, b);
					}
				}
				else if (alpha == 0) {
					return;
				}
				else {
					if (j * j * size.y * size.y / 4 + i * i * size.x * size.x / 4 <= size.x * size.x * size.y * size.y / 16) {
						if ((pos.x + j) >= 0 && (pos.x + j) < canvasSize.x && (pos.y + i) >= 0 && (pos.y + i) < canvasSize.y) {
							//canvas.draw(pos.x + j, pos.y + i, r, g, b);
							index = (((pos.y + i) * canvasSize.x) + pos.x + j) * 3;
							canvas.draw(pos.x + j, pos.y + i, *(imageBuffer + index) * (1 - alpha) + r * alpha, *(imageBuffer + index + 1) * (1 - alpha) + g * alpha, *(imageBuffer + index + 2) * (1 - alpha) + b * alpha);

						}
					}
				}
			}
		}
	}
	static void drawBar_Screen(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, Vector2D<int> size, float percentage = 50,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, float alpha = 1.f) {
		int startLeft = pos.x - size.x / 2;
		int width = size.x * percentage / 100;
		unsigned char* imageBuffer = canvas.backBuffer();
		int index;
		for (int i = startLeft; i <= startLeft + width; i++)
			for (int j = pos.y - size.y / 2; j <= pos.y + size.y / 2; j++) {
				if (i >= 0 && i < canvasSize.x && j >= 0 && j < canvasSize.y) {
					index = ((j * canvasSize.x) + i) * 3;
					canvas.draw(i, j, *(imageBuffer + index) * (1 - alpha) + r * alpha, *(imageBuffer + index + 1) * (1 - alpha) + g * alpha, *(imageBuffer + index + 2) * (1 - alpha) + b * alpha);
					//canvas.draw(i, j, r, g, b);
				}
			}
	}
	static void drawSquare_Screen(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, Vector2D<int> size,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) {
		for (int i = pos.x - size.x / 2; i <= pos.x + size.x / 2; i++)
			for (int j = pos.y - size.y / 2; j <= pos.y + size.y / 2; j++) {
				if (i == pos.x - size.x / 2 || i == pos.x + size.x / 2 || j == pos.y - size.y / 2 || j == pos.y + size.y / 2) {
					canvas.draw(i, j, r, g, b);
				}
			}
	}
	static void drawBlock_Screen(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, Vector2D<int> size,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, float alpha = 1.f) {
		unsigned char* imageBuffer = canvas.backBuffer();
		int index;
		for (int i = pos.x - size.x / 2; i <= pos.x + size.x / 2; i++)
			for (int j = pos.y - size.y / 2; j <= pos.y + size.y / 2; j++) {
				if (i >= pos.x - size.x / 2 && i <= pos.x + size.x / 2 && j >= pos.y - size.y / 2 && j <= pos.y + size.y / 2) {
					index = ((j * canvasSize.x) + i) * 3;
					canvas.draw(i, j, *(imageBuffer + index) * (1 - alpha) + r * alpha, *(imageBuffer + index + 1) * (1 - alpha) + g * alpha, *(imageBuffer + index + 2) * (1 - alpha) + b * alpha);
				}
			}
	}
	static void drawLine(GamesEngineeringBase::Window& canvas, Vector2D<int> pos1, Vector2D<int> pos2, int width,
		unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, float alpha = 1.f) {
		if (alpha <= 0)		return;
		if (alpha > 1.f)	alpha = 1.f;
		unsigned char* imageBuffer = canvas.backBuffer();
		pos1 = pos1 + canvasSize / 2 - camera_offset;
		pos2 = pos2 + canvasSize / 2 - camera_offset;
		int half = width / 2;
		// Bresenham's line algorithm (integer arithmetic)
		int x0 = pos1.x;
		int y0 = pos1.y;
		int x1 = pos2.x;
		int y1 = pos2.y;

		int dx = std::abs(x1 - x0);
		int sx = x0 < x1 ? 1 : -1;
		int dy = -std::abs(y1 - y0);
		int sy = y0 < y1 ? 1 : -1;
		int err = dx + dy;

		while (true) {
			// draw a filled square of size 'width' centered at (x0,y0)
			for (int oy = -half; oy <= half; ++oy) {
				for (int ox = -half; ox <= half; ++ox) {
					int tx = x0 + ox;
					int ty = y0 + oy;
					if (tx < 0 || tx >= canvasSize.x || ty < 0 || ty >= canvasSize.y) continue;
					int index = ((ty * canvasSize.x) + tx) * 3;
					unsigned char br = *(imageBuffer + index);
					unsigned char bg = *(imageBuffer + index + 1);
					unsigned char bb = *(imageBuffer + index + 2);
					unsigned char outR = static_cast<unsigned char>(br * (1.f - alpha) + r * alpha);
					unsigned char outG = static_cast<unsigned char>(bg * (1.f - alpha) + g * alpha);
					unsigned char outB = static_cast<unsigned char>(bb * (1.f - alpha) + b * alpha);
					canvas.draw(tx, ty, outR, outG, outB);
				}
			}

			if (x0 == x1 && y0 == y1) break;
			int e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; }
			if (e2 <= dx) { err += dx; y0 += sy; }
		}
	}
	//DEBUG TOOLS
#if DEBUG_MODE
	static void screenPositionDisplay(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, unsigned int scale = 1) {
		pos = pos + canvasSize / 2;
		std::string posText = '(' + std::to_string(pos.x) + "," + std::to_string(pos.y) + ')';
		if (pos.x >= 0 && pos.y >= 0 && pos.x <= canvasSize.x && pos.y <= canvasSize.y)
			drawText(canvas, posText, pos - Vector2D<int>(0, 20), scale, 255, 255, 255);
		else
			drawText(canvas, "OUT OF CAMERA", pos - Vector2D<int>(0, 20), scale, 255, 255, 255);
	}
	static void worldPositionDisplay(GamesEngineeringBase::Window& canvas, Vector2D<int> worldpos,
		Vector2D<int> screenpos, unsigned int scale = 1) {
		std::string posText = '(' + std::to_string(worldpos.x) + "," + std::to_string(worldpos.y) + ')';
		screenpos = screenpos + canvasSize / 2;
		drawText(canvas, posText, screenpos, scale, 255, 255, 255);
	}
#endif
};
class Animation {
private:
	//player
	static const unsigned int PlayerIdleFrameCount = 7;
	static const unsigned int PlayerMoveFrameCount = 8;
	static const unsigned int PlayerDeathFrameCount = 6;
	static const unsigned int PlayerDodgeFrameCount = 7;
	static const unsigned int PlayerSpecialFrameCount = 5;
	GamesEngineeringBase::Image animations_idle[PlayerIdleFrameCount];//animation frames
	GamesEngineeringBase::Image animations_move[PlayerMoveFrameCount];
	GamesEngineeringBase::Image animations_death[PlayerDeathFrameCount];
	GamesEngineeringBase::Image animations_dodge[PlayerDodgeFrameCount];
	GamesEngineeringBase::Image animations_special[PlayerSpecialFrameCount];
	//flower enemy
	static const unsigned int FlowerMoveFrameCount = 5;
	static const unsigned int FlowerAttackFrameCount = 12;
	static const unsigned int FlowerDeathFrameCount = 4;
	GamesEngineeringBase::Image flowerAnimations_move[FlowerMoveFrameCount];
	GamesEngineeringBase::Image flowerAnimations_attack[FlowerAttackFrameCount];
	GamesEngineeringBase::Image flowerAnimations_death[FlowerDeathFrameCount];
	//spider enemy
	static const unsigned int SpiderMoveFrameCount = 6;
	static const unsigned int SpiderDeathFrameCount = 6;
	static const unsigned int SpiderAttackFrameCount = 4;
	GamesEngineeringBase::Image spiderAnimations_move[SpiderMoveFrameCount];
	GamesEngineeringBase::Image spiderAnimations_death[SpiderDeathFrameCount];
	GamesEngineeringBase::Image spiderAnimations_attack[SpiderAttackFrameCount];
	//knife enemy
	static const unsigned int KnifeMoveFrameCount = 8;
	static const unsigned int KnifeAttackFrameCount = 6;
	static const unsigned int KnifeDeathFrameCount = 6;
	GamesEngineeringBase::Image knifeAnimations_move[KnifeMoveFrameCount];
	GamesEngineeringBase::Image knifeAnimations_death[KnifeDeathFrameCount];
	GamesEngineeringBase::Image knifeAnimations_attack[KnifeAttackFrameCount];
	//gun enemy
	static const unsigned int GunMoveFrameCount = 8;
	static const unsigned int GunAttackFrameCount = 4;
	static const unsigned int GunDeathFrameCount = 6;
	GamesEngineeringBase::Image gunAnimations_move[GunMoveFrameCount];
	GamesEngineeringBase::Image gunAnimations_death[GunDeathFrameCount];
	GamesEngineeringBase::Image gunAnimations_attack[GunAttackFrameCount];
	//FX
	static const unsigned int BoomFrameCount = 6;
	GamesEngineeringBase::Image boomAnimation[BoomFrameCount];
public:
	Animation() {
		loadPlayerAnimtion("Resources/Sprites/Player");
		loadFlowerAnimation("Resources/Sprites/FlowerEnemy");
		loadSpiderAnimation("Resources/Sprites/SpiderEnemy");
		loadKnifeAnimation("Resources/Sprites/KnifeEnemy");
		loadGunAnimation("Resources/Sprites/GunEnemy");
		//FX
		loadFXAnimation("Resources/FX");
	}
	void loadPlayerAnimtion(std::string filepath) {
		for (unsigned int i = 0; i < PlayerIdleFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "playeridle";
			filename += std::to_string(i);
			filename += ".png";
			animations_idle[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < PlayerMoveFrameCount; i++) {
			GamesEngineeringBase::Image frame;
			std::string filename = filepath + '/';
			filename += "playerwalk";
			filename += std::to_string(i);
			filename += ".png";
			animations_move[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < PlayerDodgeFrameCount; i++) {
			GamesEngineeringBase::Image frame;
			std::string filename = filepath + '/';
			filename += "playerdodge";
			filename += std::to_string(i);
			filename += ".png";
			animations_dodge[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < PlayerDeathFrameCount; i++) {
			GamesEngineeringBase::Image frame;
			std::string filename = filepath + '/';
			filename += "playerdeath";
			filename += std::to_string(i);
			filename += ".png";
			animations_death[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < PlayerSpecialFrameCount; i++) {
			GamesEngineeringBase::Image frame;
			std::string filename = filepath + '/';
			filename += "playerspecial";
			filename += std::to_string(i);
			filename += ".png";
			animations_special[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	void loadFlowerAnimation(std::string filepath) {
		for (unsigned int i = 0; i < FlowerMoveFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "flowermove";
			filename += std::to_string(i);
			filename += ".png";
			flowerAnimations_move[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < FlowerAttackFrameCount; i++) {
			GamesEngineeringBase::Image frame;
			std::string filename = filepath + '/';
			filename += "flowerattack";
			filename += std::to_string(i);
			filename += ".png";
			flowerAnimations_attack[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < FlowerDeathFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "flowerdeath";
			filename += std::to_string(i);
			filename += ".png";
			flowerAnimations_death[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	void loadSpiderAnimation(std::string filepath) {
		for (unsigned int i = 0; i < SpiderMoveFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "spiderwalk";
			filename += std::to_string(i);
			filename += ".png";
			spiderAnimations_move[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < SpiderAttackFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "spiderattack";
			filename += std::to_string(i);
			filename += ".png";
			spiderAnimations_attack[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < SpiderDeathFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "spiderdeath";
			filename += std::to_string(i);
			filename += ".png";
			spiderAnimations_death[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	void loadKnifeAnimation(std::string filepath) {
		for (unsigned int i = 0; i < KnifeMoveFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "knifemove";
			filename += std::to_string(i);
			filename += ".png";
			knifeAnimations_move[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < KnifeAttackFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "knifeattack";
			filename += std::to_string(i);
			filename += ".png";
			knifeAnimations_attack[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < KnifeDeathFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "knifedeath";
			filename += std::to_string(i);
			filename += ".png";
			knifeAnimations_death[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	void loadGunAnimation(std::string filepath) {
		for (unsigned int i = 0; i < GunMoveFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "gunmove";
			filename += std::to_string(i);
			filename += ".png";
			gunAnimations_move[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < GunAttackFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "gunattack";
			filename += std::to_string(i);
			filename += ".png";
			gunAnimations_attack[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
		for (unsigned int i = 0; i < GunDeathFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "gundeath";
			filename += std::to_string(i);
			filename += ".png";
			gunAnimations_death[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	void loadFXAnimation(std::string filepath) {
		for (unsigned int i = 0; i < BoomFrameCount; i++) {
			std::string filename = filepath + '/';
			filename += "boom";
			filename += std::to_string(i);
			filename += ".png";
			boomAnimation[i].load(filename);
			std::cout << "Loaded: " << filename << std::endl;
		}
	}
	GamesEngineeringBase::Image* playerAnimation(int statu, int& frameCount) {
		if (statu == 0) {
			frameCount = PlayerIdleFrameCount;
			return animations_idle;
		}
		if (statu == 1) {
			frameCount = PlayerMoveFrameCount;
			return animations_move;
		}
		if (statu == 2) {
			frameCount = PlayerDeathFrameCount;
			return animations_death;
		}
		if (statu == 3) {
			frameCount = PlayerSpecialFrameCount;
			return animations_special;
		}
		return nullptr;
	}
	GamesEngineeringBase::Image* enemyAnimation(int statu, int& frameCount, int enemyType) {
		switch (enemyType) {
			case 0:
				if (statu == 0) {
					frameCount = FlowerMoveFrameCount;
					return flowerAnimations_move;
				}
				if (statu == 1) {
					frameCount = FlowerAttackFrameCount;
					return flowerAnimations_attack;
				}
				if (statu == 2) {
					frameCount = FlowerDeathFrameCount;
					return flowerAnimations_death;
				}
			case 1:
				if (statu == 0) {
					frameCount = SpiderMoveFrameCount;
					return spiderAnimations_move;
				}
				if (statu == 1) {
					frameCount = SpiderAttackFrameCount;
					return spiderAnimations_attack;
				}
				if (statu == 2) {
					frameCount = SpiderDeathFrameCount;
					return spiderAnimations_death;
				}
			case 2:
				if (statu == 0) {
					frameCount = KnifeMoveFrameCount;
					return knifeAnimations_move;
				}
				if (statu == 1) {
					frameCount = KnifeAttackFrameCount;
					return knifeAnimations_attack;
				}
				if (statu == 2) {
					frameCount = KnifeDeathFrameCount;
					return knifeAnimations_death;
				}
			case 3:
				if (statu == 0) {
					frameCount = GunMoveFrameCount;
					return gunAnimations_move;
				}
				if (statu == 1) {
					frameCount = GunAttackFrameCount;
					return gunAnimations_attack;
				}
				if (statu == 2) {
					frameCount = GunDeathFrameCount;
					return gunAnimations_death;
				}
			default:
				break;
		}
		return nullptr;
	}
	GamesEngineeringBase::Image* FXAnimation(int& frameCount) {
		frameCount = BoomFrameCount;
		return boomAnimation;
	}
	~Animation() {
		delete[] animations_idle;
		delete[] animations_move;
		delete[] animations_death;
		delete[] animations_dodge;
		delete[] animations_special;

		delete[] flowerAnimations_move;
		delete[] flowerAnimations_attack;
		delete[] flowerAnimations_death;

		delete[] spiderAnimations_attack;
		delete[] spiderAnimations_move;
		delete[] spiderAnimations_death;
	}
};

class Background {
private:
	GamesEngineeringBase::Image* background = nullptr;
	GamesEngineeringBase::Image city[3];
	GamesEngineeringBase::Image castle[3];
	int level = 1;

public:
	Background(std::string foldername) {
		loadBackground(foldername);
	}
	void chooseBg(int num) {
		level = num;
		if (num == 1) {
			background = city;
		}
		else {
			background = castle;
		}
	}
	void loadBackground(std::string foldername) {
		foldername += '/';
		for (int i = 0; i < 3; i++) {
			std::string filename1 = foldername + "cityfar";
			std::string filename2 = foldername + "castlefar";
			filename1 += std::to_string(i);
			filename1 += ".png";
			filename2 += std::to_string(i);
			filename2 += ".png";
			city[i].load(filename1);
			std::cout << "load: " << filename1 << std::endl;
			castle[i].load(filename2);
			std::cout << "load: " << filename2 << std::endl;
		}
	}
	void drawThread_D(Vector2D<int> pos, unsigned char* tBuffer, int& bottomY, int startY, int endY) {
		bottomY = 0;
		for (int i = 0; i < canvasSize.x; i++) {
			for (int j = startY; j < endY; j++) {
				int index = ((j * canvasSize.x) + i) * 3;
				if (level == 1) {
					tBuffer[index] = 199;
					tBuffer[index + 1] = 228;
					tBuffer[index + 2] = 225;
				}
				else {
					tBuffer[index] = 66;
					tBuffer[index + 1] = 65;
					tBuffer[index + 2] = 68;
				}
			}
		}
		int bgNum = 0;
		int off;
		//pos = pos - camera_offset;
		for (bgNum; bgNum < 3; bgNum++) {
			if (bgNum == 0)	off = 0;
			else if (bgNum == 1)	off = camera_offset.x / 3;
			else if (bgNum == 2)		off = camera_offset.x / 2;
			for (int x = 0; x < canvasSize.x; x++)
				for (int y = startY; y < endY; y++) {//make it infinite
					if (y + camera_offset.y - pos.y >= 0 && y + camera_offset.y - pos.y < background[0].height) {
						int index = (y * canvasSize.x + x) * 3;
						if (background[bgNum].alphaAt((x + camera_offset.x - off - pos.x) % background[0].width, y + camera_offset.y - pos.y) > 1 &&
							((tBuffer[index] == 199 && tBuffer[index + 1] == 228 && tBuffer[index + 2] == 225) ||
								(tBuffer[index] == 39 && tBuffer[index + 1] == 39 && tBuffer[index + 2] == 39) ||
								(tBuffer[index] == 66 && tBuffer[index + 1] == 65 && tBuffer[index + 2] == 68))) {
							tBuffer[index] = background[bgNum].at((x + camera_offset.x - off - pos.x) % background[0].width, y + camera_offset.y - pos.y, 0);
							tBuffer[index + 1] = background[bgNum].at((x + camera_offset.x - off - pos.x) % background[0].width, y + camera_offset.y - pos.y, 1);
							tBuffer[index + 2] = background[bgNum].at((x + camera_offset.x - off - pos.x) % background[0].width, y + camera_offset.y - pos.y, 2);
						}
					}

				}
		}
		float glassAlpha = 0.5f;
		for (int y = startY; y < endY; y++)
			for (int x = 0; x < canvasSize.x; x++) {
				if (y - canvasSize.y / 2 + camera_offset.y <= worldTopLeft.y - 16 && y - canvasSize.y / 2 + camera_offset.y >= worldTopLeft.y - 200) {//draw glass
					int index = ((y * canvasSize.x) + x) * 3;
					unsigned char br = *(tBuffer + index);
					unsigned char bg = *(tBuffer + index + 1);
					unsigned char bb = *(tBuffer + index + 2);
					unsigned char outR = static_cast<unsigned char>(br * (1.f - glassAlpha) + 78 * glassAlpha);//(78,78,78)
					unsigned char outG = static_cast<unsigned char>(bg * (1.f - glassAlpha) + 78 * glassAlpha);
					unsigned char outB = static_cast<unsigned char>(bb * (1.f - glassAlpha) + 78 * glassAlpha);
					tBuffer[index] = outR;
					tBuffer[index + 1] = outG;
					tBuffer[index + 2] = outB;
				}
				//draw glass edge
				if ((y - canvasSize.y / 2 + camera_offset.y <= worldTopLeft.y - 14 && y - canvasSize.y / 2 + camera_offset.y >= worldTopLeft.y - 18)
					|| (y - canvasSize.y / 2 + camera_offset.y <= worldTopLeft.y - 198 && y - canvasSize.y / 2 + camera_offset.y >= worldTopLeft.y - 202)) {
					int index = ((y * canvasSize.x) + x) * 3;
					tBuffer[index] = 38;
					tBuffer[index + 1] = 38;
					tBuffer[index + 2] = 38;
					bottomY = y;
				}
			}
	}
};
class Tile {
private:
	GamesEngineeringBase::Image image;
public:
	Tile() {}
	void load(std::string filename) {
		image.load(filename);
	}
	void draw(GamesEngineeringBase::Window& canvas, Vector2D<int> pos) {
		Draw::drawImage(canvas, image, pos );
		//Draw::drawImage(canvas, image, pos.toInt() + canvasSize / 2);
	}
	unsigned int getHeight() { return image.height; }
	unsigned int getWidth() { return image.width; }
	GamesEngineeringBase::Image* getImage() { return &image; }
};
const int tileNum = 28;
class TileSet {
private:
	Tile t[tileNum];
	unsigned int size = tileNum;
public:
	TileSet(std::string folderpath, bool isRGB = false) {
		for (unsigned int i = 0; i < tileNum; i++) {
			std::string filename = folderpath + '/' + std::to_string(i) + ".png";
			if (isRGB) {
				if (i == 0)	filename = folderpath + '/' + "white" + ".png";//can pass
				else if (i >= 1 && i <= 9)	filename = folderpath + '/' + "green" + ".png";//speed up
				else if (i >= 10 && i <= 18)	filename = folderpath + '/' + "blue" + ".png";//speed down
				else filename = folderpath + '/' + "red" + ".png";//cant pass
			}
			t[i].load(filename);
			std::cout << "loaded " + filename << std::endl;
		}
	}
	Tile& operator[](unsigned int index) { return t[index]; }
};
class World {
private:
	TileSet ts;
	TileSet rgbts;
	int* tsArray = nullptr;
	int width = 0;
	int height = 0;
public:
	int level = 1;
	World(std::string folderpath) :ts(folderpath), rgbts(folderpath, true) {}//test
	~World() {
		delete[] tsArray;
	}
	void createWorld(std::string filename) {
		std::ifstream file(filename);
		std::string num;
		file >> num;
		width = std::stoi(num);
		file >> num;
		height = std::stoi(num);
		tsArray = (int*)malloc(sizeof(*tsArray) * width * height);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				file >> num;
				tsArray[i * width + j] = std::stoi(num);
			}
		}
		file.close();
		std::cout << "read map info from" + filename << std::endl;
		worldSize.x = width * 32;
		worldSize.y = height * 32;
		std::cout << "map pixel size:" << '(' << worldSize.x << ',' << worldSize.y << ')' << std::endl;
		worldTopLeft.x = -(width / 2) * 32;
		worldTopLeft.y = -(height / 2) * 32;
		std::cout << "map top left:" << '(' << worldTopLeft.x << ',' << worldTopLeft.y << ')' << std::endl;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				std::cout << tsArray[i * width + j] << " ";
			}
			std::cout << "\n";
		}
	}
	int getwidth() {
		return width;
	}
	int getHeight() {
		return height;
	}
	Vector2D<int> getSize() {
		return worldSize;
	}
	void draw(GamesEngineeringBase::Window& canvas) {//draw world by using camera_Offset
		//camera_offset, 32
		Vector2D<int> drawPos = Vector2D<int>(-width / 2 * 32, -height / 2 * 32);//middle of map
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (tsArray[y * width + x] == 0) continue;//dont draw 0
				Vector2D<int> tileCenterPos = drawPos + Vector2D<int>(x * 32, y * 32) + canvasSize / 2 - camera_offset;
				if (worldSize.x < (camera_offset.x + canvasSize.x / 2 + 16) - (worldTopLeft.x + 32 * x)) {
					tileCenterPos.x = tileCenterPos.x % worldSize.x + worldSize.x;
				}
				else if (worldSize.x < (worldTopLeft.x + 32 * x) - (camera_offset.x - canvasSize.x / 2 - 16)) {
					tileCenterPos.x = tileCenterPos.x % worldSize.x;
				}
				ts[tsArray[y * width + x]].draw(canvas, tileCenterPos);
			}
		}
	}
	void drawAlpha(GamesEngineeringBase::Window& canvas) {
		Vector2D<int> drawPos = Vector2D<int>(-width / 2 * 32, -height / 2 * 32);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (tsArray[i * width + j] == 0) rgbts[0].draw(canvas, drawPos + Vector2D<int>(j * 32, i * 32));
				rgbts[tsArray[i * width + j]].draw(canvas, drawPos + Vector2D<int>(j * 32, i * 32));
			}
		}
	}
	//0 for can pass; 1 for cant pass; 2 for speed up; 3 for speed down
	int collisionType(GamesEngineeringBase::Window& canvas, Vector2D<float> pos) {
		if (pos.y > (worldTopLeft.y + 32 * height - 16))				return 1;
		if ((pos.x < (worldTopLeft.x - 16) || pos.x >(worldTopLeft.x + 32 * width - 16))) {
			if (level == 1)	return 1;
		}
		int x = (((static_cast<int>(pos.x) - worldTopLeft.x) + 16) / 32) % width;
		if (x < 0)	x = x + (width - 1);
		int y = ((static_cast<int>(pos.y) - worldTopLeft.y) + 16) / 32;
		unsigned char r = *(rgbts[tsArray[y * width + x]].getImage()->at(0, 0) + 0);
		unsigned char g = *(rgbts[tsArray[y * width + x]].getImage()->at(0, 0) + 1);
		unsigned char b = *(rgbts[tsArray[y * width + x]].getImage()->at(0, 0) + 2);
		if (r == 255 && g == 255 && b == 255) return 0;
		if (r == 255) return 1;
		if (g == 255) return 2;
		if (b == 255) return 3;
		return 0;
	}
};
const unsigned int maxEnemySize = 40;
const unsigned int maxBulletNum = 100;
enum EnemyType { flower , spider , knife, gun};
class Entity {
public:
	bool active = false;
	bool faceRight = true;
	bool idling = false;
	bool moving = false;
	bool isHurt = false;
	bool isDying = false;
	int maxHurtDuration = 5;
	int hurtDuration = maxHurtDuration;
	int maxAttackCD = 50;
	int attackCD = 0;
	int attackStartFrame = 0;
	int deadStartFrame = 0;
	GamesEngineeringBase::Image* image = nullptr;//entity image pointer
	Vector2D<float> pos;//pic world position
	unsigned int existFrame = 0;
	unsigned int collisionRadius;//entity collision radius
	Vector2D<float> collisionOffset;
	bool collidable = false;
	float speed;
	int drawScale = 0;
	int maxhp = 10;
	int hp = 0;
	int atk = 1;
	//bool isInvincible = false;
	Entity(int startX = 0, int startY = 0, unsigned int objectCollisionRadius = 0, int _speed = 5) : pos(startX, startY), collisionRadius(objectCollisionRadius), speed(_speed) {
		//image.load(filename);
	}
	void setCollisionRadius(unsigned int radius) {
		if(radius == 0)	collidable = false;
		else {
			collidable = true;
			collisionRadius = radius;
		}
	}
	void drawShadow(GamesEngineeringBase::Window& canvas, Vector2D<int>size) {
		Draw::drawOval(canvas, getEntityCenter().toInt(), size, 50, 50, 50, 0.6f);
		//Draw::drawSquare(canvas, getEntityCenter().toInt() + Vector2D<int>(0, 20), Vector2D<int>(25, 5), 40, 40, 40);
	}
	
	void draw(GamesEngineeringBase::Window& canvas, int scale = 1) {
		Draw::drawImage(canvas, *image, pos.toInt() + canvasSize / 2 - camera_offset, scale);
#if DEBUG_MODE
		//display position
		Draw::screenPositionDisplay(canvas, pos.toInt() - camera_offset, scale);
		Draw::worldPositionDisplay(canvas, pos.toInt(), pos.toInt() - camera_offset, scale);
#endif
	}
	void update(Vector2D<float> _pos) {
		if (active)	existFrame++;
		pos = pos + _pos;
	}

	virtual bool getHit(Vector2D<float> _pos, int damage) {
		if ((abs(_pos.x - getEntityCenter().x) <= collisionRadius) && (abs(_pos.y - getEntityCenter().y) <= collisionRadius)) {
			hurt(damage);
			return true;
		}
		else
			return false;
	}
	virtual void hurt(int damage) {
		hp -= damage;
		isHurt = true;
		hurtDuration = maxHurtDuration;
		if (hp <= 0) {
			active = false;
		}
	}
	virtual void hurtEnd() {
		isHurt = false;
	}
	int getY() {
		return pos.y;
	}
	Vector2D<float> getEntityCenter() {
		return pos + collisionOffset;
	}
};

class Player : public Entity {
private:
	CameraEffect* camEf = nullptr;
public:
	int weapon = 1;
	int playerScore;
	int playerLevel;
	int playerEx;
	int playerMaxEx;//level up
	bool isInvincible;
	int special = 0;//special skill type
	bool isSpecial = false;
	int specialCD = 0;
	int specialmaxCD = 0;
	int specialStartFrame = 0;
	bool quit = false;
	int level = 1;
	Player() {
		maxhp = 4;
		hp = maxhp;
		playerScore = 0;
		playerLevel = 1;
		playerEx = 0;
		playerMaxEx = 50;
		drawScale = 3;
		collisionOffset = Vector2D<float>(0, 10 * drawScale);
		setCollisionRadius(15);
		isInvincible = false;
		maxAttackCD = 30;
		attackCD = 0;
		speed = 5.f;
		maxHurtDuration = 30;
		special = 0;
		isSpecial = false;
		specialmaxCD = 100;
		specialCD = specialmaxCD;
		specialStartFrame = 0;
	}
	void setCameraEffect(CameraEffect* _camEf) {
		camEf = _camEf;
	}
	void setWeapon(int i) {
		if (i == 1) {
			weapon = 1;
			maxAttackCD = 30;
			atk = 1;
		}
		else {
			weapon = 2;
			maxAttackCD = 60;
			atk = 2;
		}
	}
	void draw(GamesEngineeringBase::Window& canvas, Animation& ani) {
		int frameDuration = 5; // Number of game frames each animation frame lasts
		int totalFrameCount = 0;
		drawShadow(canvas, Vector2D<int>(30, 15));
		if (idling && !moving)		image = ani.playerAnimation(0, totalFrameCount);
		else if (!idling && moving) image = ani.playerAnimation(1, totalFrameCount);
		if (isSpecial)				image = ani.playerAnimation(3, totalFrameCount);
		if(isDying)					image = ani.playerAnimation(2, totalFrameCount);
		unsigned int frameIndex = (existFrame / frameDuration) % totalFrameCount;
		if (isDying) {
			frameIndex = ((existFrame - deadStartFrame) / (frameDuration + 6)) % totalFrameCount;
			if (frameIndex == totalFrameCount - 1)	deadEnd();
		}
		if (isSpecial) {
			frameIndex = ((existFrame - specialStartFrame) / (frameDuration)) % totalFrameCount;
			if ((existFrame - specialStartFrame) == totalFrameCount * frameDuration - 1)	specialEnd();
		}
		if (isHurt && !isDying)
			Draw::drawWhiteImage(canvas, image[frameIndex], pos.toInt() + canvasSize / 2 - camera_offset, drawScale, !faceRight);
		else
			Draw::drawImage(canvas, image[frameIndex], pos.toInt() + canvasSize / 2 - camera_offset, drawScale, !faceRight);
#if DEBUG_MODE
		//display position
		Draw::screenPositionDisplay(canvas, pos.toInt() - camera_offset, 1);
		Draw::worldPositionDisplay(canvas, pos.toInt(), pos.toInt() - camera_offset, 1);
#endif
	}
	bool getHit(Vector2D<float> _pos, int damage) override {
		if ( !isInvincible && (abs(_pos.x - getEntityCenter().x) <= collisionRadius) && (abs(_pos.y - getEntityCenter().y) <= collisionRadius)) {
			hurt(damage);
			return true;
		}
		else
			return false;
	}
	void hurt(int damage) override {
		hp -= damage;
		isHurt = true;
		hurtDuration = maxHurtDuration;
		camEf->shakeCamera(10, maxHurtDuration / 6);
		isInvincible = true;
		if (hp <= 0) {
			dead();
		}
	}
	void dead() {
		isDying = true;
		deadStartFrame = existFrame;
	}
	void deadEnd() {
		//dead end
		quit = true;
		std::cout << "player died! game over" << std::endl;
	}
	void hurtEnd() override {
		isHurt = false;
		isInvincible = false;
	}
	void levelUP() {
		playerLevel++;
		playerEx -= playerMaxEx;
		maxAttackCD -= 2;
		specialmaxCD -= 2;
		switch (playerLevel) {
			case 1:
				break;
			case 2:
				playerMaxEx += 50;
				break;
			case 3:
				maxhp += 4;
				playerMaxEx += 50;
				break;
			case 4:
				atk++;
				playerMaxEx += 50;
				break;
			case 5:
				maxhp += 4;
				playerMaxEx += 50;
				break;
		}
		hp = maxhp;

	}
	void getScoreandEx(int ex) {
		//std::cout << "get ex = " << ex << std::endl;
		playerEx += ex;
		playerScore += ex;
		if (playerEx >= playerMaxEx) {
			levelUP();
		}
		return;
	}
	bool specialStart() {
		if (specialCD == 0) {
			isSpecial = true;
			specialStartFrame = existFrame;
			camEf->shakeCamera(10, 10);
			specialCD = specialmaxCD;
			return true;
		}
		return false;
	}
	void specialEnd() {
		isSpecial = false;
	}
	void update(int _x, int _y) {
		if(!isDying){
			if (--specialCD <= 0)	specialCD = 0;
			if (isHurt) {
				if (--hurtDuration <= 0)	hurtEnd();
			}
			if (isSpecial) {
				_x = 0;
				_y = 0;
			}

			if (_x == 0 && _y == 0) {
				idling = true;
				moving = false;
			}
			else {
				idling = false;
				moving = true;
				pos.x += _x;
				pos.y += _y;
				if (level == 2 || !camEf->cameraEdgeCheck_H(pos.toInt()))
					camera_offset.x = pos.x;
				if (!camEf->cameraEdgeCheck_V(pos.toInt()))
					camera_offset.y = pos.y;
			}
		}
		existFrame++;
	}
};
class Enemy : public Entity {
private:
	Player* playerptr = nullptr;
public:
	int ex = 0;
	EnemyType type = flower;
	bool isAttack = false;
	bool isShooting = false;
	float attackRange = 20;
	float stopRange = 10;
	
	Enemy(Player* _playerptr) {
		playerptr = _playerptr;
	}
	void activeEnemy(Vector2D<float> _pos, EnemyType _type) {
		switch (_type) {
			case flower:
				maxHurtDuration = 10;
				ex = 5;
				type = _type;
				maxhp = 3;
				atk = 1;
				drawScale = 3;
				speed = 1.5;
				collisionOffset = Vector2D<float>(0, 16 * drawScale);
				attackRange = 60;
				stopRange = 30;
				moving = true;
				maxAttackCD = 50;
				setCollisionRadius(30);
				break;
			case spider:
				maxHurtDuration = 10;
				ex = 5;
				type = _type;
				maxhp = 2;
				atk = 1;
				drawScale = 3;
				speed = 1.5;
				collisionOffset = Vector2D<float>(0, 16 * drawScale);
				attackRange = 300;
				stopRange = 250;
				moving = true;
				maxAttackCD = 50;
				setCollisionRadius(30);
				break;
			case knife:
				maxHurtDuration = 10;
				ex = 10;
				type = _type;
				maxhp = 10;
				atk = 1;
				drawScale = 3;
				speed = 2;
				collisionOffset = Vector2D<float>(0, 20 * drawScale);
				attackRange = 80;
				stopRange = 50;
				moving = true;
				maxAttackCD = 30;
				setCollisionRadius(40);
				break;
			case gun:
				maxHurtDuration = 10;
				ex = 10;
				type = _type;
				maxhp = 5;
				atk = 1;
				drawScale = 3;
				speed = 1.5;
				collisionOffset = Vector2D<float>(0, 20 * drawScale);
				attackRange = 300;
				stopRange = 250;
				moving = true;
				maxAttackCD = 30;
				setCollisionRadius(40);
				break;
		}
		hp = maxhp;
		pos = _pos;
		active = true;
	}
	void deactiveEnemy() {
		collidable = false;
		active = false;
		playerptr->getScoreandEx(ex);
	}
	void draw(GamesEngineeringBase::Window& canvas, Animation& ani) {
		int frameDuration = 5; // Number of game frames each animation frame lasts
		int totalFrameCount = 0;
		drawShadow(canvas, Vector2D<int>(40, 10));
		//choose animation
		if(isAttack && !isDying)		image = ani.enemyAnimation(1, totalFrameCount, type);
		else if (isDying)				image = ani.enemyAnimation(2, totalFrameCount, type);
		else							image = ani.enemyAnimation(0, totalFrameCount, type);
		unsigned int frameIndex = (existFrame / frameDuration) % totalFrameCount;;
		if (isDying) {
			frameIndex = ((existFrame - deadStartFrame) / (frameDuration + 2)) % totalFrameCount;
			if (frameIndex == totalFrameCount - 1)	deadEnd();
		}
		else if (isAttack) {//attack should play first image
			frameIndex = ((existFrame - attackStartFrame) / frameDuration) % totalFrameCount;
			if(type == EnemyType::flower)//this attack continue for frameDuration frames
				if (frameIndex == 2)	hit();//make damage in a centain frame
			if (type == EnemyType::knife)
				if (frameIndex == 4)		hit();
			if (type == EnemyType::spider)
				if ((existFrame - attackStartFrame) == 2 * frameDuration)	shoot();
			if (type == EnemyType::gun)
				if ((existFrame - attackStartFrame) == 1 * frameDuration)	shoot();
			if (frameIndex == totalFrameCount - 1)	attackEnd();
		}
		else if(!moving)	frameIndex = 0;
		if (isHurt && !isDying)
			Draw::drawWhiteImage(canvas, image[frameIndex], pos.toInt() + canvasSize / 2 - camera_offset, drawScale, !faceRight);
		else
			Draw::drawImage(canvas, image[frameIndex], pos.toInt() + canvasSize / 2 - camera_offset, drawScale, !faceRight);
		
#if DEBUG_MODE
		//display position
		Draw::screenPositionDisplay(canvas, pos.toInt() - camera_offset, 1);
		Draw::worldPositionDisplay(canvas, pos.toInt(), pos.toInt() - camera_offset, 1);
#endif
	}
	void attack() {
		if (isAttack || attackCD > 0) return;
		attackStartFrame = existFrame;
		isAttack = true;
	}
	void attackEnd() {
		attackCD = maxAttackCD;
		isAttack = false;
	}
	void hit() {//making damage in this frame
		if (!playerptr->isInvincible && abs(getEntityCenter().x - playerptr->getEntityCenter().x) <= attackRange && abs(getEntityCenter().y - playerptr->getEntityCenter().y) <= attackRange) {
			playerptr->hurt(atk);
		}
		return;
	}
	void shoot() {
		isShooting = true;
		//shootEnd();
	}
	void shootEnd() {
		isShooting = false;
	}
	bool inAttackRange() {
		if (abs(getEntityCenter().x - playerptr->getEntityCenter().x) <= stopRange && abs(getEntityCenter().y - playerptr->getEntityCenter().y) <= stopRange) {
			return true;
		}
		return false;
	}
	void hurt(int damage) override {
		hp -= damage;
		isHurt = true;
		hurtDuration = maxHurtDuration;
		if (hp <= 0) {
			isDying = true;
			//deactiveEnemy();
		}
	}
	void dead() {
		setCollisionRadius(0);
		isDying = true;
		isAttack = false;
		deadStartFrame = existFrame;
	}
	void deadEnd() {
		isDying = false;
		deactiveEnemy();
	}
	void update(Vector2D<float> _pos) {
		if (active) {
			if (isHurt) {
				if (--hurtDuration <= 0)	hurtEnd();
			}
			existFrame++;
			if (isAttack) {
				return;
			}
			if (!isAttack)	attackCD--;
			if (inAttackRange()) {//if next to the player, stop moving
				if(attackCD > 0)	faceRight = (_pos.x >= 0) ;
				attack();
				moving = false;
				return;
			}
			moving = true;
			if (!isHurt && !isDying) {
				faceRight = (_pos.x >= 0);
				pos.x += _pos.x;
				pos.y += _pos.y;
			}
		}
		return;

	}
};
enum BulletType { normal, normal_t, enemy_normal, cannon, cannon_t};
class Projectile {
private:
	
public:
	Vector2D<float> pos;
	Vector2D<float> velocity;
	float speed = 10;
	int durationFrame = 90;//test
	int currentFrame = 0;
	int collisionSize = 0;
	bool active = false;
	BulletType type;
	bool isPlayer = false;
	Projectile() {}
	void activate(Vector2D<float> _pos, Vector2D<float> direction, BulletType _type) {
		switch (_type) {
			case BulletType::normal:
				isPlayer = true;
				speed = 10;
				break;
			case BulletType::normal_t:
				isPlayer = true;
				speed = 10;
				break;
			case BulletType::enemy_normal:
				isPlayer = false;
				speed = 5;
				break;
			case BulletType::cannon:
				isPlayer = true;
				speed = 10;
				break;
			case BulletType::cannon_t:
				isPlayer = true;
				speed = 10;
				break;
		}
		pos = _pos;
		setVelocity(direction);
		type = _type;
		active = true;
		currentFrame = durationFrame;
	}
	void deactivate() {
		//speed = 0;
		active = false;
	}
	void draw(GamesEngineeringBase::Window& canvas, int size = 7,
		unsigned char r = 255, unsigned char g = 255, unsigned char b = 0) {
		switch (type) {
			case BulletType::normal:
				Draw::drawSquare(canvas, pos.toInt(), 8, 255, 255, 0);
				break;
			case BulletType::normal_t:
				Draw::drawSquare(canvas, pos.toInt(), 8, 50, 255, 0);
				break;
			case BulletType::enemy_normal:
				Draw::drawSquare(canvas, pos.toInt(), 10, 200, 100, 100);
				break;
			case BulletType::cannon:
				Draw::drawSquare(canvas, pos.toInt(), 10, 200, 200, 200);
				break;
			case BulletType::cannon_t:
				Draw::drawSquare(canvas, pos.toInt(), 10, 200, 200, 200);
				break;
		}
		
	}
	void update() {
		pos.x += velocity.x;
		pos.y += velocity.y;
		currentFrame--;
	}
	void setVelocity(Vector2D<float> direction) {
		velocity = direction * speed;
	}
	bool isExist() {
		return currentFrame > 0;
	}
	bool isActive() {
		return active;
	}
	void setPos(Vector2D<float> _pos) {
		pos = _pos;
	}
	Vector2D<float> getPos() {
		return pos;
	}
	~Projectile() {
	}
};
class Line {
public:
	Vector2D<int> pos1;
	Vector2D<int> pos2;
	int duration = 20;
	int maxDuration = 20;
	bool isExist = false;
	int r = 0;
	int g = 0;
	int b = 0;
	int width = 1;
	float alpha = 1.f;
	bool canFade = false;
	Line(){}
	void activeLine(Vector2D<int> _pos1, Vector2D<int> _pos2, int _width,
		unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, float _alpha = 1.f, bool _canFade = false) {
		isExist = true;
		duration = maxDuration;
		pos1 = _pos1;
		pos2 = _pos2;
		r = _r;
		g = _g;
		b = _b;
		width = _width;
		alpha = _alpha;
		canFade = _canFade;
	}
	void update() {
		if (isExist) {
			duration--;
			if (duration < 0)	isExist = false;
		}
		
	}
	void draw(GamesEngineeringBase::Window& canvas) {
		if (canFade) {
			alpha = static_cast<float>(duration) / static_cast<float>(maxDuration);
		}
		Draw::drawLine(canvas, pos1, pos2, width, r, g, b, alpha);
	}
};

class FX {//only boom
private:
	GamesEngineeringBase::Image* image = nullptr;
	bool isExist = false;
	int startFrame = 0;
	int durationFrame = 2;//
	int existFrame = 0;
	int size = 1;
	Vector2D<int> pos;
public:
	FX() {}
	void activateFX(Vector2D<int> _pos, int _size) {
		isExist = true;
		existFrame = 0;
		startFrame = 0;
		pos = _pos;
		size = _size;
	}
	void update() {
		if (isExist)
			existFrame++;
	}
	const bool isActive() {
		return isExist;
	}
	void draw(GamesEngineeringBase::Window& canvas, Animation& ani) {
		int totalFrameCounts = 0;
		image = ani.FXAnimation(totalFrameCounts);
		int index = (existFrame - startFrame) / durationFrame;
		if (index >= totalFrameCounts) {
			isExist = false;
			return;
		}
		Draw::drawImage(canvas, image[index], pos + canvasSize / 2 - camera_offset, size);
	}
};
const int maxLineNum = 50;
class Manager {
private:
	CameraEffect* camEf = nullptr;
	Player* playerEntity = nullptr;
	Enemy* enemyEntities[maxEnemySize] = { nullptr };//ObjectPool
	Projectile* bullet[maxBulletNum] = { nullptr };
	World* world;
	Background* bg;
	FX* fx[50] = { nullptr };
	GamesEngineeringBase::SoundManager* sd = nullptr;
	Line lines[maxLineNum];
	unsigned char* tBuffer;
	int enemyCreateFrameElapsed = 0;
	int enemyCreateThreshold = 120;
	int level = 1;
	void activeEnemy(GamesEngineeringBase::Window& canvas, int scale = 1) {
		if (enemyCreateFrameElapsed > enemyCreateThreshold) {
			int enemyType = 1;
			if (level == 1) {
				if (playerEntity->playerLevel < 2) {
					enemyType = 1;
				}
				else if (playerEntity->playerLevel < 5) {
					enemyType = 2;
				}
			}
			if (level == 2) {
				if (playerEntity->playerLevel < 2) {
					enemyType = 2;
				}
				else if (playerEntity->playerLevel < 5) {
					enemyType = 3;
				}
			}
			
			for (unsigned int i = 0; i < maxEnemySize; i++) {
				if (!enemyEntities[i]->active) {
					enemyEntities[i]->activeEnemy(getScreenEdge_World(), static_cast<EnemyType>(rand() % enemyType));//0 for flower
					break;
				}
			}
			enemyCreateFrameElapsed = 0;
			enemyCreateThreshold -= 5;
			enemyCreateThreshold = max(enemyCreateThreshold, 30);
		}
	}
	void activePlayerBullet(GamesEngineeringBase::Window& canvas) {
		if(!playerEntity->isDying){
			playerEntity->attackCD++;
			if (playerEntity->attackCD > playerEntity->maxAttackCD) {
				sd->play("Resources/shoot.wav");
				BulletType playerBullet, playerTrackingBullet;
				if (playerEntity->weapon == 1) {
					playerBullet = normal;
					playerTrackingBullet = normal_t;
				}
				else {
					playerBullet = cannon;
					playerTrackingBullet = cannon_t;
				}
				for (unsigned int i = 0; i < maxBulletNum; i++) {
					if (!bullet[i]->isActive()) {
						Vector2D<float> mousePosition(static_cast<float>(canvas.getMouseX()), static_cast<float>(canvas.getMouseY()));
						mousePosition = mousePosition - canvasSize.toFloat() / 2 + camera_offset.toFloat();
						bullet[i]->activate(playerEntity->pos, Move::moveDirection(playerEntity->pos, mousePosition), playerBullet);
						break;
					}
				}
				if(playerEntity->weapon == 1)
					for (int n = 1; n < playerEntity->playerLevel; n++) {
					for (unsigned int i = 0; i < maxBulletNum; i++) {
						if (!bullet[i]->isActive()) {
							Vector2D<float> enemyPos(999999, 999999);//easier way
							float minDistance = abs(enemyPos.x - playerEntity->pos.x) + abs(enemyPos.y - playerEntity->pos.y);
							int distance = 0;
							for (int j = 0; j < maxEnemySize; j++) {
								if (enemyEntities[j]->active) {
									if (n == 1) {
										distance = abs(enemyEntities[j]->getEntityCenter().x - playerEntity->pos.x) + abs(enemyEntities[j]->getEntityCenter().y - playerEntity->pos.y);
										if (minDistance > distance) {
											enemyPos = enemyEntities[j]->getEntityCenter();
											minDistance = distance;
										}
									}
									else {
										enemyPos = enemyEntities[j]->getEntityCenter();
									}
								}
							}
							bullet[i]->activate(playerEntity->pos, Move::moveDirection(playerEntity->pos, enemyPos), playerTrackingBullet);
							break;
						}
					}
				}
				playerEntity->attackCD = 0;
			}
		}
	}
	void activeEnemyBullet() {
		for (int i = 0; i < maxEnemySize; i++) 
			for (int j = 0; j < maxBulletNum; j++) 
				if(!bullet[j]->isActive())
					if (enemyEntities[i]->isShooting && enemyEntities[i]->active && (enemyEntities[i]->type == EnemyType::spider || enemyEntities[i]->type == EnemyType::gun)) {
						bullet[j]->activate(enemyEntities[i]->getEntityCenter() - Vector2D<float>(0, 15 * enemyEntities[i]->drawScale), Move::moveDirection(enemyEntities[i]->getEntityCenter() - Vector2D<float>(0, 15 * enemyEntities[i]->drawScale), playerEntity->getEntityCenter()), BulletType::enemy_normal);
						enemyEntities[i]->shootEnd();
					}
	}
	void activeBullet(GamesEngineeringBase::Window& canvas) {
		activePlayerBullet(canvas);
		activeEnemyBullet();
	}
	void checkDeactiveBullet(GamesEngineeringBase::Window& canvas, unsigned int i) {
		if (!bullet[i]->isExist()) {
			bullet[i]->deactivate();
			return;
		}
		if(bullet[i]->isPlayer)
			for (int n = 0; n < maxEnemySize; n++) {
				if (bullet[i]->type == BulletType::cannon || bullet[i]->type == BulletType::cannon_t) {
					if (enemyEntities[n]->collidable && enemyEntities[n]->getHit(bullet[i]->getPos(), playerEntity->atk)) {//direct hit
						explosionEffect(enemyEntities[n]->getEntityCenter().toInt());
						sd->play("Resources/explosion.wav");
						bullet[i]->deactivate();
					}
				}
				else {
					if (enemyEntities[n]->collidable && enemyEntities[n]->getHit(bullet[i]->getPos(), playerEntity->atk)) {//attack = 1
						bullet[i]->deactivate();
						return;
					}
				}
				
			}
		else {
			if (playerEntity->getHit(bullet[i]->getPos(), 1) && playerEntity->isInvincible) {
				bullet[i]->deactivate();
				return;
			}
		}
	}
	void explosionEffect(Vector2D<int> pos) {

		for (int i = 0; i < 50; i++) {
			if (!fx[i]->isActive()) {
				fx[i]->activateFX(pos, playerEntity->playerLevel / 3 + 2);
				break;
			}
		}
		int explosionRange = 20;
		for (int i = 0; i < maxEnemySize; i++) {
			if(enemyEntities[i]->active && abs(enemyEntities[i]->getEntityCenter().x - pos.x) < playerEntity->playerLevel * explosionRange
				&& abs(enemyEntities[i]->getEntityCenter().y - pos.y) < playerEntity->playerLevel * explosionRange) {
				enemyEntities[i]->getHit(enemyEntities[i]->getEntityCenter(), playerEntity->atk);
			}
		}
	}
	void findMaxHp(int* num, int size) {
		int* hps = new int[size];
		for (int i = 0; i < size; i++) {
			hps[i] = -1;
		}
		for (int i = 0; i < maxEnemySize; i++) {
			if (enemyEntities[i]->active && abs((enemyEntities[i]->getEntityCenter() - playerEntity->getEntityCenter()).x) < 400
				&& abs((enemyEntities[i]->getEntityCenter() - playerEntity->getEntityCenter()).y) < 300) {
				for (int j = 0; j < size; j++) {
					if (enemyEntities[i]->hp >= hps[j]) {
						for (int n = size - 1; n > j; n--) {
							hps[n] = hps[n - 1];
							num[n] = num[n - 1];
						}
						hps[j] = enemyEntities[i]->hp;
						num[j] = i;
						break;
					}
				}
			}
		}
	}
	//get a position out of screen
	static Vector2D<float> getScreenEdge_World() {
		unsigned int i;
		Vector2D<float> pos;
		bool flag = false;
		do{
			i = rand() % 4;
			switch (i) {
			case 0:
				pos.x = camera_offset.x - (canvasSize.x / 2 + 50);
				pos.y = camera_offset.y + (rand() % canvasSize.y - canvasSize.y / 2);
				break;
			case 1:
				pos.x = camera_offset.x + (canvasSize.x / 2 + 50);
				pos.y = camera_offset.y + (rand() % canvasSize.y - canvasSize.y / 2);
				break;
			case 2:
				pos.x = camera_offset.x + (rand() % canvasSize.x - canvasSize.x / 2);
				pos.y = camera_offset.y - (canvasSize.y / 2 + 50);
				break;
			case 3:
				pos.x = camera_offset.x + (rand() % canvasSize.x - canvasSize.x / 2);
				pos.y = camera_offset.y + (canvasSize.y / 2 + 50);
				break;
			}
			flag = false;
			if (pos.y < worldTopLeft.y)
				flag = true;
		} while (flag);
		
		return pos;
	}
	void drawPlayerHp(GamesEngineeringBase::Window& canvas, Vector2D<int> pos, int scale) {// left drawing
		//draw background
		int offset = bar[0].width * scale + 1;
		int num = playerEntity->maxhp / 4 - 1;
		Draw::drawImage_Screen(canvas, bar[0], pos, scale, 0.8);
		int i = 1;
		for (i; i <= num; i++) {
			Draw::drawImage_Screen(canvas, bar[1], Vector2D<int>(pos.x + i * offset, pos.y), scale, 0.8);
		}
		Draw::drawImage_Screen(canvas, bar[2], Vector2D<int>(pos.x + offset * i, pos.y), scale, 0.8);
		//draw current hp
		offset = pos.x + 2 * scale + 1;
		for (i = 0; i < playerEntity->hp; i++) {
			if (playerEntity->hp <= 3)
				Draw::drawBlock_Screen(canvas, Vector2D<int>(offset + i * 4 * scale, pos.y), Vector2D<int>(3 * scale, 6 * scale), 150, 150, 70, 0.8);
			else
				Draw::drawBlock_Screen(canvas, Vector2D<int>(offset + i * 4 * scale, pos.y), Vector2D<int>(3 * scale, 6 * scale), 200, 70, 70, 0.8);
		}
	}
	void drawUI(GamesEngineeringBase::Window& canvas) {
		//up area
		Draw::drawImage_Screen(canvas, barDecorations[0], Vector2D<int>(400, 20), 3, 0.6f);
		if (level == 1) {
			if (playerEntity->playerLevel < 5)
				Draw::drawText(canvas, "LEVEL: " + std::to_string(playerEntity->playerLevel), Vector2D<int>(350, 50), 3, 255, 255, 255);
			if (playerEntity->playerLevel == 5)
				Draw::drawText(canvas, "LAST LEVEL: " + std::to_string(playerEntity->playerLevel), Vector2D<int>(250, 50), 3, 255, 200, 200);
		}
		else {
			Draw::drawText(canvas, "LEVEL: " + std::to_string(playerEntity->playerLevel), Vector2D<int>(350, 50), 3, 255, 255, 255);
		}
		Draw::drawText(canvas, "SCORE: " + std::to_string(playerEntity->playerScore), Vector2D<int>(650, 20), 3, 255, 255, 255);
		Draw::drawText(canvas, "MAP: ", Vector2D<int>(620, 40), 3, 255, 255, 255);
		if (level == 1) {
			Draw::drawText(canvas, "CITY" , Vector2D<int>(690, 40), 3, 0, 255, 100);
		}
		if (level == 2) {
			Draw::drawText(canvas, "CASTLE", Vector2D<int>(690, 40), 3, 255, 100, 100);
		}
		Draw::drawSquare_Screen(canvas, Vector2D<int>(400, 80), Vector2D<int>(605, 15), 0, 0, 0);
		Draw::drawBar_Screen(canvas, Vector2D<int>(400, 80), Vector2D<int>(600, 10), playerEntity->playerEx * 100 / playerEntity->playerMaxEx, 180, 180, 0, 0.7f);
		//bottom area
		Draw::drawBlock_Screen(canvas, Vector2D<int>(200, 559), Vector2D<int>(200, 60), 20, 20, 20, 0.7f);
		Draw::drawImage_Screen(canvas, playerIcon, Vector2D<int>(70, 530), 1, 1.f);
		Draw::drawImage_Screen(canvas, barDecorations[2], Vector2D<int>(210, 580), 2, 0.6f);
		Draw::drawText(canvas, "WEAPON: ", Vector2D<int>(135, 530), 2, 255, 255, 255);
		if (playerEntity->weapon == 1) {
			Draw::drawText(canvas, "RIFLE", Vector2D<int>(155, 550), 3, 255, 255, 0);
		}
		else {
			Draw::drawText(canvas, "CANNON", Vector2D<int>(155, 550), 3, 255, 255, 0);
		}
		drawPlayerHp(canvas, Vector2D<int>(150, 500), 4);
		Draw::drawText(canvas, "HP", Vector2D<int>(135, 450), 4, 255, 255, 255);
		Draw::drawText(canvas, "SPACE", Vector2D<int>(310, 560), 2, 255, 255, 255);
		Draw::drawBar_Screen(canvas, Vector2D<int>(360, 580), Vector2D<int>(100, 15),100 - playerEntity->specialCD * 100 / playerEntity->specialmaxCD,0,150,150);
		Draw::drawSquare_Screen(canvas, Vector2D<int>(360, 580), Vector2D<int>(100, 15));
	}
	void drawLines(GamesEngineeringBase::Window& canvas) {
		for (int i = 0; i < maxLineNum; i++) {
			if (lines[i].isExist) {
				lines[i].draw(canvas);
			}
		}
	}
	void updateLines(GamesEngineeringBase::Window& canvas) {
		for (int i = 0; i < maxLineNum; i++) {
			if (lines[i].isExist) {
				lines[i].update();
			}
		}
	}
	
	void loadSave(std::string filename) {
		std::cout << "load file start!" << std::endl;
		std::string tmp;
		std::ifstream fin("save.txt");
		getline(fin,tmp);
		std::cout << tmp << std::endl;
		//level
		getline(fin, tmp);
		level = stoi(tmp);
		playerEntity->level = level;
		world->level = level;
		bg->chooseBg(level);
		//player
		getline(fin, tmp);
		playerEntity->weapon = stoi(tmp);
		getline(fin, tmp);
		playerEntity->specialmaxCD = stoi(tmp);
		getline(fin, tmp);
		playerEntity->maxAttackCD = stoi(tmp);
		getline(fin, tmp);
		playerEntity->existFrame = stoi(tmp);
		getline(fin, tmp);
		playerEntity->pos.x = stoi(tmp);
		getline(fin, tmp);
		playerEntity->pos.y = stoi(tmp);
		getline(fin, tmp);
		playerEntity->playerEx = stoi(tmp);
		getline(fin, tmp);
		playerEntity->playerLevel = stoi(tmp);
		getline(fin, tmp);
		playerEntity->playerMaxEx = stoi(tmp);
		getline(fin, tmp);
		playerEntity->atk = stoi(tmp);
		getline(fin, tmp);
		playerEntity->hp = stoi(tmp);
		getline(fin, tmp);
		playerEntity->maxhp = stoi(tmp);
		//camera
		getline(fin, tmp);
		camera_offset.x = stoi(tmp);
		getline(fin, tmp);
		camera_offset.y = stoi(tmp);
		//enemy
		for (int i = 0; i < maxEnemySize; i++) {
			getline(fin, tmp);
			enemyEntities[i]->active = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->pos.x = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->pos.y = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->type = static_cast<EnemyType>(stoi(tmp));
			getline(fin, tmp);
			enemyEntities[i]->hp = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->maxhp = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->drawScale = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->attackRange = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->stopRange = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->collidable = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->collisionRadius = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->collisionOffset.x = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->collisionOffset.y = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->atk = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->maxAttackCD = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->attackCD = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->attackStartFrame = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->deadStartFrame = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->speed = stoi(tmp);
			getline(fin, tmp);
			enemyEntities[i]->ex = stoi(tmp);
		}
		//bullets
		for (int i = 0; i < maxBulletNum; i++) {
			getline(fin, tmp);
			bullet[i]->active = stoi(tmp);
			getline(fin, tmp);
			bullet[i]->pos.x = stoi(tmp);
			getline(fin, tmp);
			bullet[i]->pos.y = stoi(tmp);
			getline(fin, tmp);
			bullet[i]->velocity.x = stof(tmp);
			getline(fin, tmp);
			bullet[i]->velocity.y = stof(tmp);
			getline(fin, tmp);
			bullet[i]->isPlayer = stoi(tmp);
			getline(fin, tmp);
			bullet[i]->type = static_cast<BulletType>(stoi(tmp));
			getline(fin, tmp);
			bullet[i]->durationFrame = stoi(tmp);
			getline(fin, tmp);
			bullet[i]->currentFrame = stoi(tmp);
		}
		fin.close();
	}
public:
	Manager() {
		tBuffer = new unsigned char[canvasSize.x * canvasSize.y * 3];
	}
	~Manager() {
		for (unsigned int i = 0; i < maxEnemySize; i++) {
			if (enemyEntities[i]) {
				delete enemyEntities[i];
			}
		}
		for (unsigned int i = 0; i < maxBulletNum; i++) {
			if (bullet[i]) {
				delete bullet[i];
			}
		}
		delete[] tBuffer;
	}
	void saveFile() {
		std::ofstream fout("save.txt", std::ios::out);
		time_t timep;
		time(&timep);
		fout << ctime(&timep);
		//level
		fout << level << std::endl;
		//player
		fout << playerEntity->weapon << std::endl;
		fout << playerEntity->specialmaxCD << std::endl;
		fout << playerEntity->maxAttackCD << std::endl;
		fout << playerEntity->existFrame << std::endl;
		fout << playerEntity->pos.x << std::endl;
		fout << playerEntity->pos.y << std::endl;
		fout << playerEntity->playerEx << std::endl;
		fout << playerEntity->playerLevel << std::endl;
		fout << playerEntity->playerMaxEx << std::endl;
		fout << playerEntity->atk << std::endl;
		fout << playerEntity->hp << std::endl;
		fout << playerEntity->maxhp << std::endl;
		//camera
		fout << camera_offset.x << std::endl;
		fout << camera_offset.y << std::endl;
		//enemy
		for (int i = 0; i < maxEnemySize; i++) {
			fout << enemyEntities[i]->active << std::endl;
			fout << enemyEntities[i]->pos.toInt().x << std::endl;
			fout << enemyEntities[i]->pos.toInt().y << std::endl;
			fout << enemyEntities[i]->type << std::endl;
			fout << enemyEntities[i]->hp << std::endl;
			fout << enemyEntities[i]->maxhp << std::endl;
			fout << enemyEntities[i]->drawScale << std::endl;
			fout << enemyEntities[i]->attackRange << std::endl;
			fout << enemyEntities[i]->stopRange << std::endl;
			fout << enemyEntities[i]->collidable << std::endl;
			fout << enemyEntities[i]->collisionRadius << std::endl;
			fout << enemyEntities[i]->collisionOffset.x << std::endl;
			fout << enemyEntities[i]->collisionOffset.y << std::endl;
			fout << enemyEntities[i]->atk << std::endl;
			fout << enemyEntities[i]->maxAttackCD << std::endl;
			fout << enemyEntities[i]->attackCD << std::endl;
			fout << enemyEntities[i]->attackStartFrame << std::endl;
			fout << enemyEntities[i]->deadStartFrame << std::endl;
			fout << enemyEntities[i]->speed << std::endl;
			fout << enemyEntities[i]->ex << std::endl;
		}
		//bullets
		for (int i = 0; i < maxBulletNum; i++) {
			fout << bullet[i]->active << std::endl;
			fout << bullet[i]->pos.toInt().x << std::endl;
			fout << bullet[i]->pos.toInt().y << std::endl;
			fout << bullet[i]->velocity.x << std::endl;
			fout << bullet[i]->velocity.y << std::endl;
			fout << bullet[i]->isPlayer << std::endl;
			fout << bullet[i]->type << std::endl;
			fout << bullet[i]->durationFrame << std::endl;
			fout << bullet[i]->currentFrame << std::endl;
		}
		fout.close();
		std::cout << "save file end!" << std::endl;
	}
	void loadFile(bool loadTxt = false) {
		if (loadTxt) {
			loadSave("save.txt");
		}

	}
	void setSound(GamesEngineeringBase::SoundManager *_sd) {
		sd = _sd;
	}
	void setLevel(int _level) {
		level = _level;
	}
	void setPlayer(Player* player) {
		playerEntity = player;
	}
	void setWorld(World* _world) {
		world = _world;
		//world->createWorld();
		world->createWorld("Resources/Map/mapinfo.txt");
	}
	void setCameraEffect(CameraEffect* _camEf) {
		camEf = _camEf;
	}
	void setBackground(Background* _bg) {
		bg = _bg;
		bg->chooseBg(level);
	}
	void createEnemyPool(Player* player) {
		for (unsigned int i = 0; i < maxEnemySize; i++) {
			enemyEntities[i] = new Enemy(player);
		}
	}
	void createBulletPool() {
		for (unsigned int i = 0; i < maxBulletNum; i++) {
			bullet[i] = new Projectile();
		}
	}
	void createFXPool() {
		for (unsigned int i = 0; i < 50; i++) {
			fx[i] = new FX();
		}
	}
	bool nextLevel() {
		if (level == 1 && playerEntity->playerLevel == 6)	return true;
		return false;
	}
	void update(GamesEngineeringBase::Window& canvas) {
		enemyCreateFrameElapsed++;
		activeEnemy(canvas, 2);
		for (unsigned int i = 0; i < maxEnemySize; i++) {
			if (enemyEntities[i]->active) {
				if (abs(enemyEntities[i]->getEntityCenter().x - playerEntity->getEntityCenter().x) > canvasSize.x) {//too far from player, teleport
					enemyEntities[i]->pos = getScreenEdge_World();
				}
				else {
					Vector2D<float> direction = Move::moveDirection(enemyEntities[i]->getEntityCenter(), playerEntity->getEntityCenter());
					Vector2D<float> movement = direction * enemyEntities[i]->speed;
					enemyEntities[i]->update(movement);
				}
				
			}
		}
		//bullet
		activeBullet(canvas);
		for (unsigned int i = 0; i < maxBulletNum; i++) {
			if (bullet[i]->isActive()) {
				bullet[i]->update();
				checkDeactiveBullet(canvas, i);
			}
		}
		//FXs
		for (unsigned int i = 0; i < 50; i++) {
			if (fx[i]->isActive()) {
				fx[i]->update();
			}
		}
		//lines
		updateLines(canvas);

		//if (level == 1 && playerEntity->playerLevel == 10)	nextLevel = true;
		
		inputCheck(canvas);
	}
	void inputCheck(GamesEngineeringBase::Window& canvas) {
		int horizontalMove = 0;
		int verticalMove = 0;
		if (canvas.keyPressed(VK_ESCAPE)) {
			//exit game
			std::cout << "Escape key pressed. Exiting game loop." << std::endl;
		}
		if (canvas.keyPressed('W')) verticalMove = -1;// Move up
		if (canvas.keyPressed('S')) verticalMove = 1;// Move down
		if (canvas.keyPressed('A')) horizontalMove = -1;// Move left
		if (canvas.keyPressed('D')) horizontalMove = 1;// Move right
		if (canvas.keyPressed(VK_SPACE)) {
			if (playerEntity->specialStart()) { // using special skill
				int size = 5;
				int* index = new int[size];
				for (int i = 0; i < size; i++)	index[i] = -1;
				findMaxHp(index, size);
				for (int i = 0; i < size; i++) {
					if (index[i] != -1) {
						//active effects
						for (int j = 0; j < maxLineNum; j++) {
							if (!lines[j].isExist) {
								Vector2D<int> pos1, pos2;
								pos1 = enemyEntities[index[i]]->getEntityCenter().toInt();
								pos2 = Vector2D<int>(pos1.x, -1000);
								lines[j].activeLine(pos1, pos2, 10, 255, 255, 255, 1, true);
								enemyEntities[index[i]]->getHit(enemyEntities[index[i]]->getEntityCenter(), playerEntity->atk);
								break;
							}
						}
					}
				}
			}
		}
#if DEBUG_MODE
		if (canvas.keyPressed('T')) saveFile();
		if (canvas.keyPressed('Y')) loadSave("save.txt");
		if (canvas.keyPressed('O')) camEf->shakeCamera(20, 100);//test shake
		if (canvas.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft)) {
			Vector2D<float> mousePosition(static_cast<float>(canvas.getMouseX()), static_cast<float>(canvas.getMouseY()));
			mousePosition = mousePosition - canvasSize.toFloat() / 2 + camera_offset.toFloat();
			world->collisionType(canvas, mousePosition);
			std::cout << "mouse position:" << '(' << mousePosition.toInt().x << ',' << mousePosition.toInt().y << ')' << std::endl;
		}
#endif
		if(!playerEntity->isDying && !playerEntity->isSpecial){
			if (horizontalMove == 1) playerEntity->faceRight = true;
			else if (horizontalMove == -1)	playerEntity->faceRight = false;
		}
		Vector2D<float> movement = Move::playerMove(horizontalMove, verticalMove, playerEntity->speed);
		int colType = world->collisionType(canvas, playerEntity->getEntityCenter() + movement);
		switch (colType) {
		case 0:playerEntity->update(static_cast<int>(movement.x), static_cast<int>(movement.y)); break;
		case 1:
			if (world->collisionType(canvas, playerEntity->getEntityCenter() + Vector2D<float>(0, movement.y)) != 1) {
				playerEntity->update(0, static_cast<int>(movement.y));
				break;
			}
			else if (world->collisionType(canvas, playerEntity->getEntityCenter() + Vector2D<float>(movement.x, 0)) != 1) {
				playerEntity->update(static_cast<int>(movement.x), 0);
				break;
			}
			playerEntity->update(0, 0); break;
		case 2:playerEntity->update(static_cast<int>(movement.x * 1.3), static_cast<int>(movement.y * 1.3)); break;
		case 3:playerEntity->update(static_cast<int>(movement.x / 2), static_cast<int>(movement.y / 2)); break;
		}
	}
	void draw(GamesEngineeringBase::Window& canvas, Animation& ani) {
		camEf->cameraEffect();
		//this part should be optimized by using threads :3
		int bottomY = 0;
		int bottom[4] = { 0 };		
		//because background occupies less than half
		std::thread t1(&Background::drawThread_D, bg, Vector2D<int>(0, worldTopLeft.y - 250), tBuffer, std::ref(bottom[0]), 0, canvasSize.y / 8);
		std::thread t2(&Background::drawThread_D, bg, Vector2D<int>(0, worldTopLeft.y - 250), tBuffer, std::ref(bottom[1]), canvasSize.y / 8, canvasSize.y / 4);
		std::thread t3(&Background::drawThread_D, bg, Vector2D<int>(0, worldTopLeft.y - 250), tBuffer, std::ref(bottom[2]), canvasSize.y / 4, canvasSize.y * 3 / 8);
		std::thread t4(&Background::drawThread_D, bg, Vector2D<int>(0, worldTopLeft.y - 250), tBuffer, std::ref(bottom[3]), canvasSize.y * 3 / 8, canvasSize.y / 2);

		//bg->drawThread_D(Vector2D<int>(0, worldTopLeft.y - 250), tBuffer, std::ref(bottom[0]), 0, canvasSize.y / 8);
		//draw default floor 
		if(level == 1)
			memset(canvas.backBuffer(), 68, canvasSize.x * canvasSize.y * 3 * sizeof(unsigned char));
		else {
			memset(canvas.backBuffer(), 40, canvasSize.x * canvasSize.y * 3 * sizeof(unsigned char));
		}
#if DEBUG_MODE
		world->drawAlpha(canvas);
		//center point draw
		Draw::drawCrosshair(canvas, Vector2D<int>(0, 0), 20, 255, 0, 0);
#else
		world->draw(canvas);
#endif
		//enemy draw 1
		for (unsigned int i = 0; i < maxEnemySize; i++) {
			if (enemyEntities[i]->active && enemyEntities[i]->getEntityCenter().y < playerEntity->getEntityCenter().y) {
				enemyEntities[i]->draw(canvas, ani);
			}
		}
		//player draw
		playerEntity->draw(canvas, ani);
		//enemy draw 2
		for (unsigned int i = 0; i < maxEnemySize; i++) {
			if (enemyEntities[i]->active && enemyEntities[i]->getEntityCenter().y >= playerEntity->getEntityCenter().y) {
				enemyEntities[i]->draw(canvas, ani);
			}
		}
		//FX draw
		for (unsigned int i = 0; i < 50; i++) {
			if (fx[i]->isActive()) {
				fx[i]->draw(canvas, ani);
			}
		}
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		for (int i = 0; i < 4; i++) 
			if (bottomY < bottom[i])	bottomY = bottom[i];
		memcpy(canvas.backBuffer(), tBuffer, canvasSize.x * 3 * bottomY * sizeof(unsigned char));
		//bullet draw
		for (unsigned int i = 0; i < maxBulletNum; i++) {
			if (bullet[i]->isActive()) {
				bullet[i]->draw(canvas);
			}
		}
		camEf->shakeCameraFrameEnd();
		drawUI(canvas);
		//test
		drawLines(canvas);

	}
	bool quitGame(GamesEngineeringBase::Window& canvas) {
		if (playerEntity->quit || canvas.keyPressed(VK_ESCAPE))
			return true;
		return false;
	}

};

enum ButtonType {
	null,
	back,
	_continue,
	mainmenu,
	newgame,
	quit,
	resume
};

class Button {
private:
	GamesEngineeringBase::Image* image = nullptr;
	bool active = false;
	Vector2D<int> pos;
	Vector2D<int> size;
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	int wordSize = 1;
	std::string text = "";
	ButtonType func;
public:
	Button(ButtonType _func, Vector2D<int> _pos, Vector2D<int> _size = Vector2D<int>(0,0), unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, std::string _text = "", int _wordSize = 1)
		:func(_func), pos(_pos), size(_size), r(_r), g(_g), b(_b), text(_text), wordSize(_wordSize) {
		setImage();
	}
	void setImage() {
		image = &buttonImage[static_cast<int>(func) - 1];
	}
	bool checkPos(Vector2D<int> mousePos) {
		if (mousePos.x >= pos.x - image->width / 2 && mousePos.x <= pos.x + image->width / 2
			&& mousePos.y >= pos.y - image->height / 2 && mousePos.y <= pos.y + image->height / 2) {
			return true;
		}
		return false;
	}
	void draw(GamesEngineeringBase::Window& canvas) {
		if(text != "")
			Draw::drawText(canvas, text, pos, 2, 255, 255, 255);
		Draw::drawImage_Screen(canvas, *image, pos);
		
	}
	const bool isActive() {
		return active;
	}
	void setActive(bool f) {
		active = f;
	}
	const ButtonType returnFunc() {
		return func;
	}
	const Vector2D<int> getCenter() {
		return pos;
	}
	Vector2D<int> getImageSize() {
		return Vector2D<int>(image->width, image->height);
	}
};

class ButtonSet {
private:
	std::vector<Button> bts;
	bool isSelect = false;
	Vector2D<int> selectPos;
public:
	ButtonSet() {	}
	~ButtonSet() {
		bts.clear();
	}
	void enqueue(Button bt) {
		bts.push_back(bt);
	}
	ButtonType buttonClickCheck(Vector2D<int> pos) {
		ButtonType res = null;
		for (auto& bt : bts) {
			if (bt.isActive() && bt.checkPos(pos)) {
				res = bt.returnFunc();
			}
		}
		return res;
	}
	void buttonSelectDecoration(Vector2D<int> pos) {
		isSelect = false;
		for (auto& bt : bts) {
			if (bt.isActive() && bt.checkPos(pos)) {
				selectPos = bt.getCenter() + Vector2D<int>(0, bt.getImageSize().y / 2);
				isSelect = true;
				break;
			}
		}
	}
	void setActive(ButtonType func, bool active) {
		for (auto& bt : bts) {
			if (bt.returnFunc() == func) {
				bt.setActive(active);
				return;
			}
		}
	}
	void draw(GamesEngineeringBase::Window& canvas) {
		for (auto& bt : bts) {
			if (bt.isActive()) {
				bt.draw(canvas);
			}
		}
		if(isSelect)
			Draw::drawImage_Screen(canvas, barDecorations[0], selectPos, 2);
	}
	void allSetFalse() {
		for (auto& bt : bts) {
			bt.setActive(false);
		}
	}
	
};
int main()
{
	GamesEngineeringBase::Window canvas;
	GamesEngineeringBase::Timer timer;

	canvas.create(800, 600, "GameWindow");
	canvasSize = Vector2D<int>(canvas.getWidth(), canvas.getHeight());
	//Draw initialize
	Draw::fontsInit("Resources/Fonts");
	Draw::uiInit("Resources/UI");
	//Draw initialize end
	//Camera effect initialize
	CameraEffect camEf;
	// Camera effect initialize end
	//animation initialize
	Animation ani;
	unsigned int frameCount = 0;
	// animation initialize end
	//world initialize
	World world("Resources/Map");
	//world initialize end
	//background initialize
	Background bg("Resources/Map/FarBackground");
	//sounds initialize
	GamesEngineeringBase::SoundManager sounds;
	sounds.loadMusic("Resources/backmusic.wav");
	sounds.playMusic(); 
	sounds.load("Resources/explosion.wav");
	sounds.load("Resources/shoot.wav");
	int level = 1;
	bool running = true;
	//lab
	ButtonSet bts;
	Button _continue(ButtonType::_continue, Vector2D<int>(200, 200));
	Button newgame(ButtonType::newgame, Vector2D<int>(200, 300));
	Button quit(ButtonType::quit, Vector2D<int>(200, 400));
	Button back(ButtonType::back, Vector2D<int>(450, 300));//no using
	Button mainmenu(ButtonType::mainmenu, Vector2D<int>(330, 315));
	Button resume(ButtonType::resume, Vector2D<int>(470, 315));
	bts.enqueue(back);
	bts.enqueue(_continue);
	bts.enqueue(mainmenu);
	bts.enqueue(newgame);
	bts.enqueue(quit);
	bts.enqueue(resume);
	//for test
	bool isMenu = true;
	bool isPlaying = true;
	bool isPausing = false;
	bool loadTxt = false;
	int playerWeapon = 1;
	//lab end
	while (running) {
		
		bts.setActive(ButtonType::newgame, true);
		bts.setActive(ButtonType::quit, true);
		std::ifstream file("save.txt");
		if (file.is_open()) {
			bts.setActive(ButtonType::_continue, true);
		}
		file.close();
		bool chooseContinue = false;
		
		while (isMenu) {
			canvas.checkInput();
			canvas.clear();
			Draw::drawImage_Screen(canvas, menu, Vector2D<int>(canvasSize.x / 2, canvasSize.y / 2));
			Vector2D<int> mousePosition(canvas.getMouseX(), canvas.getMouseY());
			bts.buttonSelectDecoration(mousePosition);
			if (canvas.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft)) {
				
				switch (bts.buttonClickCheck(mousePosition)) {
					case ButtonType::newgame:
						isMenu = false;
						isPlaying = true;
						level = 1;
						loadTxt = false;
						break;
					case ButtonType::_continue:
						isMenu = false;
						isPlaying = true;
						loadTxt = true;
						chooseContinue = true;
						break;
					case ButtonType::quit:
						isMenu = false;
						isPlaying = false;
						running = false;
						break;
					default: break;
				}
				
			}
			bts.draw(canvas);
			canvas.present();
		}
		bts.allSetFalse();
		if (!running)	break;
		//Player initialize
		Player player;
		player.setCameraEffect(&camEf);
		player.setWeapon(playerWeapon);
		//Player initialize end
		//manager initialize
		Manager manager;//game manager
		manager.setSound(&sounds);
		manager.setLevel(level);//WORLD LEVEL--------------------------
		manager.setWorld(&world);
		manager.setCameraEffect(&camEf);
		manager.setPlayer(&player);
		manager.setBackground(&bg);
		manager.createEnemyPool(&player);
		manager.createBulletPool();
		manager.createFXPool();
		manager.loadFile(loadTxt);
		//manager initialize end
		bool isChoosingWeapon = true;
		
		while (isPlaying) {
			if (frameCount == 20 && !chooseContinue) {//20 choosing weapon
				while (isChoosingWeapon) {
					canvas.checkInput();
					if (canvas.keyPressed('1')) {
						playerWeapon = 1;
						isChoosingWeapon = false;
					}
					if (canvas.keyPressed('2')) {
						playerWeapon = 2;
						isChoosingWeapon = false;
					}
					Draw::drawImage_Screen(canvas, panel, Vector2D<int>(400, 300), 3);
					Draw::drawSquare_Screen(canvas, Vector2D<int>(200, 300), Vector2D<int>(300, 400), 255,255,255);
					Draw::drawSquare_Screen(canvas, Vector2D<int>(600, 300), Vector2D<int>(300, 400), 255, 255, 255);
					Draw::drawText(canvas, "RIFLE", Vector2D<int>(100, 200), 8, 255, 255, 255);
					Draw::drawText(canvas, "UPGRADE TO ", Vector2D<int>(100, 290), 3, 200, 200, 200);
					Draw::drawText(canvas, "AUTO TRACK", Vector2D<int>(100, 320), 3, 200, 200, 200);
					Draw::drawText(canvas, "AFTER LEVEL 2", Vector2D<int>(100, 350), 3, 200, 200, 200);
					Draw::drawText(canvas, "PRESS 1", Vector2D<int>(100, 420), 6, 255, 255, 0);

					Draw::drawText(canvas, "CANNON", Vector2D<int>(470, 200), 7, 255, 255, 255);
					Draw::drawText(canvas, "MAKE LARGER", Vector2D<int>(500, 290), 3, 200, 200, 200);
					Draw::drawText(canvas, "AOE DAMAGE", Vector2D<int>(500, 320), 3, 200, 200, 200);
					Draw::drawText(canvas, "AFTER LEVEL 2", Vector2D<int>(500, 350), 3, 200, 200, 200);
					Draw::drawText(canvas, "PRESS 2", Vector2D<int>(500, 420), 6, 255, 255, 0);
					canvas.present();
				}
				player.setWeapon(playerWeapon);
			}
			float dt = timer.dt();
			canvas.checkInput();
			manager.update(canvas);
			manager.draw(canvas, ani);
			//lab
			Draw::drawText(canvas, "FPS " + std::to_string(1.0f / dt), Vector2D<int>(10, 10), 2);//FPS display
			//adb end
			canvas.present();
			frameCount++;
			
			timingus(11000);//make sure the frame per second 60
			//if (dt > 0.03f)	std::cout << "Delta Time too long: " << dt << " seconds\n";
			isPausing = manager.quitGame(canvas);
			while (isPausing) {
				int canSave = false;
				if (player.hp > 0)//dead
					canSave = true;
				bts.setActive(ButtonType::mainmenu, true);
				if(canSave)	bts.setActive(ButtonType::resume, true);
				canvas.checkInput();
				Draw::drawImage_Screen(canvas, panel, Vector2D<int>(400, 300));
				Vector2D<int> mousePosition(canvas.getMouseX(), canvas.getMouseY());
				bts.buttonSelectDecoration(mousePosition);
				if (canvas.mouseButtonPressed(GamesEngineeringBase::MouseButton::MouseLeft)) {
					switch (bts.buttonClickCheck(mousePosition)) {
						case ButtonType::mainmenu:
							isPausing = false;
							isPlaying = false;
							isMenu = true;
							if (canSave)	manager.saveFile();
							break;
						case ButtonType::resume:
							isPausing = false;
							break;
					}
				}
				bts.draw(canvas);
				canvas.present();
				bts.allSetFalse();
			}
			if (manager.nextLevel()) {
				Draw::drawImage_Screen(canvas, panel, Vector2D<int>(400, 300), 2);
				Draw::drawText(canvas, "CONGRATULATIONS", Vector2D<int>(140, 220), 6, 255, 255, 0);
				Draw::drawText(canvas, "ENTERing CASTLE...", Vector2D<int>(200, 330), 4, 200, 200, 0);
				canvas.present();
				level = 2;
				isMenu = false;
				std::this_thread::sleep_for(std::chrono::seconds(2));
				break;
			}
		}
		camera_offset = Vector2D<int>(0, 0);
		
	}
	return 0;
}
