/*
	Created by David Jurík (Reye) 2016
		Website: http://reye.me/
*/

#include "CAG.h"
#include <iostream>
#include <memory.h>
#include <windows.h>
#include <cmath>

GRAPHICS::GRAPHICS(){
	GRAPHICS::is_CREATED = false;
	//GRAPHICS::buffer = NULL;
	GRAPHICS::buf_res = {0,0};

	GRAPHICS::wnd = NULL;
}

GRAPHICS::~GRAPHICS(){
	GRAPHICS::dispose();
}

bool GRAPHICS::initialize(HANDLE console, SIZE resolution){
	if(resolution.cx < 1 || resolution.cy < 1 || GRAPHICS::is_CREATED) return false;

	SetConsoleOutputCP(CP_UTF8);

	GRAPHICS::wnd = console;
	GRAPHICS::buf_res = resolution;

	/*screen buffer*/
	GRAPHICS::buffer = new byte*[GRAPHICS::buf_res.cy];
	for (int i = 0; i < GRAPHICS::buf_res.cy; i++)
	{
		GRAPHICS::buffer[i] = new byte[GRAPHICS::buf_res.cx];
		
		for (int x = 0; x < GRAPHICS::buf_res.cx; x++) {
			GRAPHICS::buffer[i][x] = '0x0';
		}
	}

	/*set default pixel size*/
	GRAPHICS::setPointSize(4,6);

	/*set default palette*/
	GRAPHICS::setPalette(NULL);

	/*console window*/
	SMALL_RECT conSize;
	conSize.Left = 0;
	conSize.Top = 0;
	conSize.Right = GRAPHICS::buf_res.cx-1;
	conSize.Bottom = (GRAPHICS::buf_res.cy/2)-1;

	COORD conBufSize = { GRAPHICS::buf_res.cx , (GRAPHICS::buf_res.cy/2) };

	bool ok = SetConsoleScreenBufferSize(GRAPHICS::wnd, conBufSize);
	SetConsoleWindowInfo(GRAPHICS::wnd, ok, &conSize);

	/*set cursor parameters*/
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(GRAPHICS::wnd, &cursorInfo);
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GRAPHICS::wnd, &cursorInfo);

	GRAPHICS::is_CREATED = true;
	return true;
}
bool GRAPHICS::dispose(){
	if (!GRAPHICS::is_CREATED) return false;
	GRAPHICS::is_CREATED = false;

	for (int i = 0; i < GRAPHICS::buf_res.cy; i++)
	{
		delete[] GRAPHICS::buffer[i];
	}
	delete[] GRAPHICS::buffer;

	GRAPHICS::buf_res = { 0,0 };

	GRAPHICS::wnd = NULL;

	return true;
}

void GRAPHICS::setPalette(COLORREF *palette){
	if(palette == NULL){
		/*default*/
		palette = new COLORREF[16]
		{
			0x00000000, 0x00800000, 0x00008000, 0x00808000,
			0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
			0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00,
			0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff
		};
	}

	CONSOLE_SCREEN_BUFFER_INFOEX cir;
	cir.cbSize = sizeof(cir);
	GetConsoleScreenBufferInfoEx(GRAPHICS::wnd, &cir);
	
	SMALL_RECT conSize;
	conSize.Left = 0;
	conSize.Top = 0;
	conSize.Right = GRAPHICS::buf_res.cx - 1;
	conSize.Bottom = (GRAPHICS::buf_res.cy/2) - 1;

	cir.srWindow = conSize;

	for(int i = 0; i < 16; i++) cir.ColorTable[i] = palette[i];

	bool ok = SetConsoleScreenBufferInfoEx(GRAPHICS::wnd, &cir);
	SetConsoleWindowInfo(GRAPHICS::wnd, ok, &conSize);
}
void GRAPHICS::setPointSize(unsigned short width, unsigned short height){
	CONSOLE_FONT_INFOEX cir;
	cir.cbSize = sizeof(cir);
	GetCurrentConsoleFontEx(GRAPHICS::wnd,true,&cir);
	
	cir.dwFontSize.Y = height;
	cir.dwFontSize.X = width;

	bool set = false;
	set = SetCurrentConsoleFontEx(GRAPHICS::wnd,false,&cir);
	if(!set)throw set;
}

void GRAPHICS::setPoint(int x, int y, byte ColorID){
	if ((x >= 0 && x < GRAPHICS::buf_res.cx) &&
		(y >= 0 && y < GRAPHICS::buf_res.cy) &&
		(ColorID <= 0xF)) {
		GRAPHICS::buffer[y][x] = ColorID;
	}
}
byte GRAPHICS::getPoint(int x, int y){
	if ((x >= 0 && x < GRAPHICS::buf_res.cx) &&
		(y >= 0 && y < GRAPHICS::buf_res.cy)) {
		return GRAPHICS::buffer[y][x];
	}
}

CHAR_INFO GRAPHICS::fromTable(byte ColorID, byte ColorID2){
	
	WORD colorTableTop[16] = {
		0, 
		BACKGROUND_BLUE, 
		BACKGROUND_GREEN,
		BACKGROUND_GREEN | BACKGROUND_BLUE,
		BACKGROUND_RED,
		BACKGROUND_RED | BACKGROUND_BLUE,
		BACKGROUND_RED | BACKGROUND_GREEN,
		BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
		BACKGROUND_INTENSITY,
		BACKGROUND_INTENSITY | BACKGROUND_BLUE,
		BACKGROUND_INTENSITY | BACKGROUND_GREEN,
		BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
		BACKGROUND_INTENSITY | BACKGROUND_RED,
		BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
		BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
		BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
	};

	WORD colorTableBottom[16] = {
		0,
		FOREGROUND_BLUE,
		FOREGROUND_GREEN,
		FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_RED,
		FOREGROUND_RED | FOREGROUND_BLUE,
		FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY,
		FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	};

	CHAR_INFO tableInfo;
		tableInfo.Char.UnicodeChar = 223;
		tableInfo.Attributes = colorTableBottom[ColorID] | colorTableTop[ColorID2];

	return tableInfo;
}

void GRAPHICS::plot(){
	if(!GRAPHICS::is_CREATED) return;

	/*prepare screen buffer*/
	COORD bufSize = { GRAPHICS::buf_res.cx, GRAPHICS::buf_res.cy };
	COORD bufPos = { 0, 0 };

	SMALL_RECT conSize;
	conSize.Left = 0;
	conSize.Top = 0;
	conSize.Right = GRAPHICS::buf_res.cx - 1;
	conSize.Bottom = (GRAPHICS::buf_res.cy/2) - 1;

	CHAR_INFO *scr = new CHAR_INFO[GRAPHICS::buf_res.cx * (GRAPHICS::buf_res.cy/2)];

	/*construct screen buffer */
	int y2 = 0;
	for(int y = 0; y < GRAPHICS::buf_res.cy-1; y+=2){
		for(int x = 0; x < GRAPHICS::buf_res.cx; x++)
			{
				scr[x + GRAPHICS::buf_res.cx * y2] = GRAPHICS::fromTable(GRAPHICS::buffer[y][x], GRAPHICS::buffer[y+1][x]);
			}
		y2++;
		}
	
	/*draw screen buffer*/
	WriteConsoleOutput(GRAPHICS::wnd, scr, bufSize, bufPos, &conSize);

	/*free memory*/
	free(scr);
}
void GRAPHICS::clear(){
	if (!GRAPHICS::is_CREATED) return;
	for (int y = 0; y < GRAPHICS::buf_res.cy; y++)
		for (int x = 0; x < GRAPHICS::buf_res.cx; x++)
			GRAPHICS::buffer[y][x] = 0;
}