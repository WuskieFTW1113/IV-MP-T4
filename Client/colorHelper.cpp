#include "colorHelper.h"

unsigned int colorHelper::argb(unsigned int rgba)
{
	return (rgba << 24) | (rgba >> 8);
}

void colorHelper::breakRgba(unsigned int rgba, int& r, int& g, int& b, int& a)
{
	r = (rgba >> 24) & 0xFF;
	g = (rgba >> 16) & 0xFF;
	b = (rgba >> 8) & 0xFF;
	a = rgba & 0xFF;
}