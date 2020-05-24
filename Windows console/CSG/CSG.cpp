// This magical thing was written by Reye
// Such a fun thing

#include "CSG.h"
#include <iostream>
#include <memory.h>
#include <windows.h>
#include <cmath>

GRAPHICS::GRAPHICS(void){}
GRAPHICS::~GRAPHICS(void){}

void GRAPHICS::initialize(int width, int height){
	if(width <= 0 || height <= 0) return;

	int i,x;
	if(GRAPHICS::buffer_width > 0 || GRAPHICS::buffer_height > 0){
		/*free before reinicialize*/

		for (i = 0; i<height; i++) {
			delete[] GRAPHICS::buffer[i];
		}
		delete[] GRAPHICS::buffer;
	} 

	/*Setup screen buffer*/
	SMALL_RECT wndSize = {0,0,width-1,height-1};
	_COORD bufSize;
	bufSize.X = width;
	bufSize.Y = height;

	HANDLE wnd = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleWindowInfo(wnd, true, &wndSize);
	SetConsoleScreenBufferSize(wnd, bufSize);
	SetConsoleOutputCP(CP_UTF8);

	/*Wait a bit for magic to happen*/
	Sleep(200);

	/*set cursor parameters*/
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(wnd, &cursorInfo);
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(wnd, &cursorInfo);

	Sleep(200);

	/*cursor size*/
	CONSOLE_FONT_INFOEX cir = { 0 };
	cir.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	wcscpy_s(cir.FaceName, L"Terminal");
	cir.dwFontSize = { 16, 12 };

	bool set = false;
	set = SetCurrentConsoleFontEx(wnd, false, &cir);
	if (!set)throw GetLastError();
	
	/*Setup virtual buffer*/
	GRAPHICS::buffer = new CHAR_INFO*[height];
	
	CHAR_INFO empty;
		empty.Char.UnicodeChar = L' ';
		empty.Attributes = 0;

	for(i = 0;i<height;i++){
		GRAPHICS::buffer[i] = new CHAR_INFO[width];
		
		for(x = 0; x < width; x++){
			GRAPHICS::buffer[i][x] = empty;
		}
	}

	GRAPHICS::buffer_width = width;
	GRAPHICS::buffer_height = height;
}

void GRAPHICS::setPoint(int x, int y, CHAR_INFO symbol){
	if( (x >= 0 && x < GRAPHICS::buffer_width) &&
		(y >= 0 && y < GRAPHICS::buffer_height)){
		GRAPHICS::buffer[y][x] = symbol;
	}
}

CHAR_INFO GRAPHICS::getPoint(int x, int y){
	if( (x >= 0 && x < GRAPHICS::buffer_width) &&
		(y >= 0 && y < GRAPHICS::buffer_height)){
		return GRAPHICS::buffer[y][x];
	}
	CHAR_INFO empty;
		empty.Char.UnicodeChar = L' ';
		empty.Attributes = 0;
	return empty;
}

void GRAPHICS::drawRectangle(int x, int y, int width, int height, CHAR_INFO symbol, bool fill){
	int i,j;

	if(fill){
		for(i = 0; i<width; i++){
			for(j = 0; j<height; j++){
				GRAPHICS::setPoint(i+x,j+y,symbol);
			}
		}

	}else{
		/*outline*/
		for(i = 0; i<width; i++){
			GRAPHICS::setPoint(x+i,y,symbol);
			GRAPHICS::setPoint(x+i,y+height-1,symbol);
		}

		for(i = 0; i<height; i++){
			GRAPHICS::setPoint(x,y+i,symbol);
			GRAPHICS::setPoint(x+width-1,y+i,symbol);
		}
	}
}

void GRAPHICS::drawLine(int x1, int y1, int x2, int y2, CHAR_INFO symbol){
	int dx = x2 - x1;
	int dy = y2 - y1;
	float steps = 0;

	if(dx > dy){
		steps = abs(dx);
	}else{
		steps = abs(dy);
	}

	float Xinc = dx / steps;
	float Yinc = dy / steps;

	float x = (float)x1,y = (float)y1;
	int i;

	for(i = 0; i < steps; i++){
		GRAPHICS::setPoint((int)x,(int)y,symbol);
		x += Xinc;
		y += Yinc;
	}
}

void GRAPHICS::drawElipse(int cx, int cy, int width, int height, CHAR_INFO symbol, bool fill){
	if(width <= 0 || height <= 0) return;
	int hh = height*height;
	int ww = width*width;

	if(fill){
		int hhww = hh*ww;
		int x0 = width;
		int dx = 0;
		int x,y;

		/*draw horizontal*/
		for(x = -width; x <= width; x++)
			GRAPHICS::setPoint(cx+x,cy,symbol);
		

		for(y = 1; y <= height; y++){
			int x1 = x0 - (dx - 1);

			for(; x1 > 0; x1--)
				if(x1*x1*hh + y*y*ww <= hhww)
					break;

			dx = x0 - x1;
			x0 = x1;
					
			/*draw everithing else*/
			for(x = -x0; x <= x0; x++){
				GRAPHICS::setPoint(cx+x,cy-y,symbol);
				GRAPHICS::setPoint(cx+x,cy+y,symbol);
			}
		}
	}else{
		int fa2 = 4 * ww, fb2 = 4 * hh;
		int x, y, sigma;

		/*first half*/
		for(x = 0, y = height, sigma = 2*hh+ww*(1-2*height);hh*x <= ww*y;x++){
			GRAPHICS::setPoint(cx + x, cy + y, symbol);
			GRAPHICS::setPoint(cx - x, cy + y, symbol);
			GRAPHICS::setPoint(cx + x, cy - y, symbol);
			GRAPHICS::setPoint(cx - x, cy - y, symbol);

			if(sigma >= 0){
				sigma += fa2 * (1 - y);
				y--;
			}
			sigma += hh * ((4 * x) + 6);
		}
		
		/*second half*/
		for(x = width, y = 0, sigma = 2*ww+hh*(1-2*width);ww*y <= hh*x;y++){
			GRAPHICS::setPoint(cx + x, cy + y, symbol);
			GRAPHICS::setPoint(cx - x, cy + y, symbol);
			GRAPHICS::setPoint(cx + x, cy - y, symbol);
			GRAPHICS::setPoint(cx - x, cy - y, symbol);

			if(sigma >= 0){
				sigma += fb2 * (1 - x);
				x--;
			}
			sigma += ww * ((4 * y) + 6);
		}

	}
}

void GRAPHICS::drawString(int x, int y,std::string text, unsigned short Attributes, bool isUnicode){
	int i, bx = x;
	const char *str = text.c_str();
	for(i = 0; i < text.length(); i++){
		const char symbol = str[i];

		CHAR_INFO toWrite;
			toWrite.Attributes = Attributes;
			if(isUnicode) toWrite.Char.UnicodeChar = symbol;
			else toWrite.Char.UnicodeChar = symbol;

		switch(symbol){
		/*forbidden or not implemented*/
		case '\a':
		case '\b':
		case '\f':
		case '\t':
		case '\v':
			break;
		/*text control*/
		case '\r':
			x = bx;
			break;
		case '\n':
			x = bx;
			y++;
			break;
		/*write char*/
		default:
			GRAPHICS::setPoint(x,y,toWrite);
			x++;
			break;
		}
	}
}

void GRAPHICS::plot(){
	int y,x;
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	
	/*Draw on screen*/
	COORD bufSize = {GRAPHICS::buffer_width,GRAPHICS::buffer_height};
	COORD bufPos = {0,0};
	SMALL_RECT bufScreen = {0,0,GRAPHICS::buffer_width-1,GRAPHICS::buffer_height-1};
	
	/*Construct screen buffer data*/
	CHAR_INFO *scrBuffer = new CHAR_INFO[GRAPHICS::buffer_width * GRAPHICS::buffer_height];

	for(y = 0; y < GRAPHICS::buffer_height; y++){
		for(x = 0; x < GRAPHICS::buffer_width; x++){
			scrBuffer[x + GRAPHICS::buffer_width * y] = GRAPHICS::buffer[y][x];
		}
	}

	WriteConsoleOutput(out, scrBuffer, bufSize, bufPos, &bufScreen);
	free(scrBuffer);

	/*show cursor*/
	GRAPHICS::showCursor(true);
	
	/*Clear buffer*/
	CHAR_INFO empty;
		empty.Char.UnicodeChar = L' ';
		empty.Attributes = 0;

	for(y = 0; y < GRAPHICS::buffer_height; y++){
		for(x = 0; x < GRAPHICS::buffer_width; x++){
			GRAPHICS::buffer[y][x] = empty;
		}
	}
}

void GRAPHICS::showCursor(bool visible){
	/*Get console wnd handle*/
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;

	/*set cursor parameters*/
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(out, &cursorInfo);
}

int GRAPHICS::width(){
	return GRAPHICS::buffer_width;
}

int GRAPHICS::height(){
	return GRAPHICS::buffer_width;
}