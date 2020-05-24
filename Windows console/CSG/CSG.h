#include <Windows.h>
#include <iostream>

class GRAPHICS{
private:
	CHAR_INFO **buffer = NULL;
	int buffer_width = 0;
	int buffer_height = 0;
	void showCursor(bool visible);

public:
	GRAPHICS(void);
	~GRAPHICS(void);
	
	void initialize(int width, int height);

	void setPoint(int x, int y, CHAR_INFO symbol);
	CHAR_INFO getPoint(int x, int y);
	
	void drawRectangle(int x, int y, int width, int height, CHAR_INFO symbol, bool fill);
	void drawLine(int x1, int y1, int x2, int y2, CHAR_INFO symbol);
	void drawElipse(int cx, int cy, int width, int height, CHAR_INFO symbol, bool fill);

	void drawString(int x, int y,std::string text, unsigned short Attributes, bool isUnicode);
	
	int width();
	int height();

	void plot();
};
