/* Renderer.h
 * 
 * Controls the windows device contexts. */

#ifndef RENDERER_H
#define RENDERER_H

#include "windows.h"

class Renderer
{
public:
	Renderer(HWND handle) : m_hwnd(handle) { }
	~Renderer();

	void init();

	int getFrameWidth();
	int getFrameHeight();

	HDC* getBackHdc();
	HDC* getFrontHdc();
	HDC* getBitmapHdc();
	HWND* getHandle();
	RECT* getScreenRect();
	BITMAPINFO* getBitmapInfo();

	void push();

private:
	// Width and height of a frame.
	// This excludes the window borders,
	int		m_frameWidth, m_frameHeight;

	// Contexts for double buffering.
	HDC		m_backHdc, m_frontHdc, m_bitmapHdc;

	// Handle to the window.
	HWND	m_hwnd;

	// Rectangle describing the screen.
	RECT	m_screenRect;

	// Old bitmaps.
	HBITMAP m_oldFrontBitmap, oldBackBitmap;

	BITMAPINFO m_bitmapInfo;
};

#endif // RENDERER_H