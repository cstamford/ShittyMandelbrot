/* MandelbrotViewer.h
 * 
 * Controller class for the MandelbrotViewer.
 * Contains the majority of MandelbrotViewer logic. */

#ifndef MANDELBROTVIEWER_H
#define MANDELBROTVIEWER_H

#include "Renderer.h"
#include "InputManager.h"
#include "Logging.h"

#include "windows.h"

#include <thread>
#include <ctime>
#include <vector>


class MandelbrotViewer
{
public:
	enum State
	{
		INIT_STATE,
		GENERATING_STATE,
		COMPLETE_STATE
	};
	
	// Viewer domensions
	static const int SCREEN_WIDTH = 1024;
	static const int SCREEN_HEIGHT = 768;

	MandelbrotViewer(HWND handle);
	~MandelbrotViewer();

	void init();
	void run();

	void onWinEvent(HWND hwnd, UINT message, 
					WPARAM wParam, LPARAM lParam);

	bool isQuitting();
	void setQuitting(bool state);

	Renderer* getRenderer();

private:
	static const bool BENCHMARK = true;
	static const unsigned int THREAD_COUNT_X = 3;
	static const unsigned int THREAD_COUNT_Y = 3;
	static const unsigned int UPDATE_DELAY = 50;
	static const unsigned int RENDER_DELAY = 50;

	bool m_quitting;
	bool m_needRedraw;
	bool m_renderThreadInterrupt;
	bool m_updateThreadInterrupt;
	bool m_computeThreadInterrupt;

	int m_maxIterations;

	double m_leftSetValue;
	double m_rightSetValue;
	double m_topSetValue;
	double m_bottomSetValue;
	double m_zoomFactor;

	State m_state;
	Renderer m_renderer;
	InputManager m_inputMgr;
	Logging m_log;

	clock_t m_computeTimer;
	clock_t m_renderTimer;
	clock_t m_updateTimer;

	unsigned char* m_rawImageData;
	std::vector<std::thread*> m_computeThreads;
	std::thread* m_renderThread;
	std::thread* m_updateThread;

	void startComputeThreads();
	void joinComputeThreads();
	void update();
	void computeMandelbrotSet(int sliceId, int sliceIdX);
	void render();
};

#endif // MANDELBROTVIEWER_H