#include "Renderer.h"

// Destructor, releases all objects and device contexts.
Renderer::~Renderer()
{
	SelectObject(m_backHdc, m_oldFrontBitmap);
	DeleteDC(m_backHdc);
	DeleteDC(m_bitmapHdc);
	ReleaseDC(m_hwnd, m_frontHdc);
}


// Initializes the device contexts, and maps them to the screen.
// This must be called before using the class!
void Renderer::init()
{
	// Creates rect based on window client area
	GetClientRect(m_hwnd, &m_screenRect);	

	// Initializes front buffer device context (window)
	m_frontHdc = GetDC(m_hwnd);	

	// Sets up back buffer to be compatible with the front
	m_backHdc = CreateCompatibleDC(m_frontHdc);

	// And the bitmap buffer to be compatible with the back buffer
	m_bitmapHdc = CreateCompatibleDC(m_backHdc);

	// Creates a bitmap compatible with the front buffer
	m_oldFrontBitmap = CreateCompatibleBitmap(m_frontHdc, m_screenRect.right, 
											  m_screenRect.bottom);

	// Creates a bitmap compatible with the back buffer
    m_oldFrontBitmap = (HBITMAP)SelectObject(m_backHdc, m_oldFrontBitmap);

	// Clear the back buffer
	FillRect(m_backHdc, &m_screenRect, (HBRUSH)GetStockObject(0));

	// Find the actual screen display width/height
	m_frameWidth = m_screenRect.right;
	m_frameHeight = m_screenRect.bottom;

	m_bitmapInfo.bmiHeader.biSize = sizeof (m_bitmapInfo.bmiHeader);
	m_bitmapInfo.bmiHeader.biWidth = m_frameWidth;
	m_bitmapInfo.bmiHeader.biHeight = -m_frameHeight;
	m_bitmapInfo.bmiHeader.biPlanes = 1;
	m_bitmapInfo.bmiHeader.biBitCount = 24; // 24 bits per pixel - one unsigned char for each pixel
	m_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	m_bitmapInfo.bmiHeader.biSizeImage = m_bitmapInfo.bmiHeader.biWidth * m_bitmapInfo.bmiHeader.biHeight;
}


// Returns the width of a frame.
int Renderer::getFrameWidth()
{
	return m_frameWidth;
}


// Returns the height of a frame.
int Renderer::getFrameHeight()
{
	return m_frameHeight;
}


// Returns a pointer to the back buffer.
HDC* Renderer::getBackHdc()
{
	return &m_backHdc;
}


// Returns a pointer to the front buffer.
HDC* Renderer::getFrontHdc()
{
	return &m_frontHdc;
}


// Returns a pointer to the bitmap buffer.
HDC* Renderer::getBitmapHdc()
{
	return &m_bitmapHdc;
}


// Returns a pointer to the window handle.
HWND* Renderer::getHandle()
{
	return &m_hwnd;
}


// Returns a pointer to the screen rect.
RECT* Renderer::getScreenRect()
{
	return &m_screenRect;
}

// Returns a pointer to the screen rect.
BITMAPINFO* Renderer::getBitmapInfo()
{
	return &m_bitmapInfo;
}

// Swap the front and back buffers, thus displaying
// anything in the back buffer on screen.
void Renderer::push()
{
	// Copy the back buffer's contents to the front buffer
	BitBlt(m_frontHdc, m_screenRect.left, m_screenRect.top,
		   m_screenRect.right, m_screenRect.bottom, m_backHdc, 
		   0, 0, SRCCOPY);

	// Clear the back buffer
	FillRect(m_backHdc, &m_screenRect, (HBRUSH)GetStockObject(0));	
}
