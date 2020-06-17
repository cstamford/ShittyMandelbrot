#include "InputManager.h"

// Initialise the two bool arrays to false.
void InputManager::init()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keysDown[i] = false;
		m_keysDownOnce[i] = false;
	}
}


// Called to update the InputManager.
// 
// Parameters:
// [UINT] msg: the WM_ message code.
// [WPARAM] wParam: contains part of the windows event info.
// [LPARAM] lParam: contains part of the windows event info.
void InputManager::onWinEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		m_keysDown[wParam] = true;
		break;

	case WM_KEYUP:
		m_keysDown[wParam] = false;
		m_keysDownOnce[wParam] = true;
		break;

	case WM_MOUSEMOVE:
		m_mouseX = LOWORD(lParam);
		m_mouseY = HIWORD(lParam);
		break;

	case WM_MOUSEWHEEL:
		m_mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	}
}


// Returns true if the key is pressed, false if not.
//  
// Parameters:
// [Keys::Keys] key: the enumerated type key to check for
bool InputManager::isKeyDown(Keys key)
{
	if (m_keysDown[key])
		m_keysDownOnce[key] = false;

	return m_keysDown[key];
}


// Returns true if the key was released since last call, false if not.
//  
// Parameters:
// [Keys::Keys] key: the enumerated type key to check for
bool InputManager::isKeyDownOnce(Keys key)
{
	bool keyDownOnce = m_keysDownOnce[key];

	if (keyDownOnce)
		m_keysDownOnce[key] = false;

	return keyDownOnce;
}


// Returns the mouse x-coord.
int InputManager::getMouseX()
{
	return m_mouseX;
}


// Returns the mouse y-coord.
int InputManager::getMouseY()
{
	return m_mouseY;
}

int InputManager::getMouseWheelDelta()
{
	return m_mouseWheelDelta;
}