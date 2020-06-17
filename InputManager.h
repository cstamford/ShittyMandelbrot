/* InputManager.h
 * 
 * Tracks keyboard and mouse input. */

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "Keys.h"
#include "windows.h"

class InputManager
{
public:
	// Uses an initialization list to set the mouse x- and y-coords.
	InputManager() : m_mouseX(0), m_mouseY(0) { }

	void init();

	// Called on receipt on a windows event
	void onWinEvent(UINT msg, WPARAM wParam, LPARAM lParam);

	bool isKeyDown(Keys key);
	bool isKeyDownOnce(Keys key);

	int getMouseX();
	int getMouseY();

	int getMouseWheelDelta();

private:
	// Bool array containing keydown flags.
	bool m_keysDown[256];

	// Bool array containing keyup flags.
	bool m_keysDownOnce[256];

	// Mouse x- and y-coords.
	int m_mouseX, m_mouseY;

	// 
	int m_mouseWheelDelta;
};

#endif // INPUTMANAGER_H