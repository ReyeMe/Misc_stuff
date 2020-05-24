/*
	Created by David Jurík (Reye) 2016
		Website: http://reye.me/
*/

#pragma once

#include <Windows.h>
#include <iostream>

class GRAPHICS {
private:
	bool is_CREATED;
	SIZE buf_res;

	byte **buffer;
	HANDLE wnd;

	CHAR_INFO fromTable(byte ColorID, byte ColorID2);

public:
	GRAPHICS(void);
	~GRAPHICS(void);

	bool initialize(HANDLE console, SIZE resolution);
	bool dispose(void);
	
	void setPalette(COLORREF *palette);
	void setPointSize(unsigned short width, unsigned short height);

	void setPoint(int x, int y, byte ColorID);
	byte getPoint(int x, int y);

	void plot();
	void clear();
};