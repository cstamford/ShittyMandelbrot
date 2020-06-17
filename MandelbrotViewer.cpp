#include "MandelbrotViewer.h"
#include "Helpers.h"

#include <complex>
#include <cmath>

MandelbrotViewer::MandelbrotViewer(HWND handle) : m_renderer(handle) { }

MandelbrotViewer::~MandelbrotViewer()
{
	delete m_rawImageData;

	m_computeThreadInterrupt = true;
	joinComputeThreads();

	if (m_renderThread != nullptr)
	{
		m_renderThreadInterrupt = true;
		m_renderThread->join();
		delete m_renderThread;
	}

	if (m_updateThread != nullptr)
	{
		m_updateThreadInterrupt = true;
		m_updateThread->join();
		delete m_updateThread;
	}
}

// Initializes various manager classes, loads textures, and sets up
// the player sprite and tick timer.
// Call this before calling run()!
void MandelbrotViewer::init()
{
	m_state = INIT_STATE;
	m_maxIterations = 768;
	m_needRedraw = false;
	m_quitting = false;
	m_renderThreadInterrupt = false;
	m_updateThreadInterrupt = false;
	m_computeThreadInterrupt = false;

	if (BENCHMARK)
	{
		// These coordinates have been chosen as they represent
		// a relatively even distribution of work between threads.

		m_topSetValue = 0.14905;
		m_bottomSetValue = 0.14695;
		m_leftSetValue = -0.7454;
		m_rightSetValue = -0.7426;
		m_zoomFactor = 0.0001;
	}
	else
	{
		m_leftSetValue = -2.0;
		m_rightSetValue = 1.0;
		m_topSetValue = 1.125;
		m_bottomSetValue = -1.125; 
		m_zoomFactor = 0.1;
	}

	// Initialize the helpers
	m_renderer.init();
	m_inputMgr.init();

	// Initialise the array for pixel data based on screen size
	m_rawImageData = new unsigned char[m_renderer.getFrameWidth() * m_renderer.getFrameHeight() * 3];

	// Start a thread to render the set
	m_renderThread = new std::thread(&MandelbrotViewer::render, this);

	m_log.lockMutex();
	m_log.write("\nRender thread started");
	m_log.unlockMutex();

	// Start a thread to handle user input
	m_updateThread = new std::thread(&MandelbrotViewer::update, this);

	m_log.lockMutex();
	m_log.write("\nUpdate thread started");
	m_log.unlockMutex();
}

// MandelbrotViewer loop.
// Call this repeatedly to run the MandelbrotViewer.
// Handles the MandelbrotViewer state, and the transition between each state.
void MandelbrotViewer::run()
{
	switch (m_state)
	{
	case INIT_STATE:

		m_needRedraw = false;
		m_computeThreadInterrupt = false;
		memset(m_rawImageData, 0, m_renderer.getFrameWidth() * m_renderer.getFrameHeight() * 3);

		m_log.lockMutex();
		m_log.write("\nPixel data cleared, drawing the set anew");
		m_log.unlockMutex();

		m_computeTimer = clock();
		startComputeThreads();
		m_state = GENERATING_STATE;

		break;

	case GENERATING_STATE:

		joinComputeThreads();
		m_computeThreads.clear();

		m_log.lockMutex();
		m_log.write("\nSet complete, set took ");
		m_log.write(Helpers::toString(clock() - m_computeTimer));
		m_log.write(" ms total");
		m_log.unlockMutex();

		if (m_computeThreadInterrupt && !m_quitting)
			m_state = INIT_STATE;
		else
			m_state = COMPLETE_STATE;

		break;

	case COMPLETE_STATE:

		if (m_needRedraw)
			m_state = INIT_STATE;

		break;
	}
}


// Responsible for receiving windows events.
// Forwards the events to the input manager.
void MandelbrotViewer::onWinEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DESTROY)
		PostQuitMessage(0);
	else
		m_inputMgr.onWinEvent(message, wParam, lParam);
}


bool MandelbrotViewer::isQuitting()
{
	return m_quitting;
}


void MandelbrotViewer::setQuitting(bool state)
{
	if (state)
	{
		m_computeThreadInterrupt = true;
		m_renderThreadInterrupt = true;
		m_updateThreadInterrupt = true;

		m_quitting = state;
	}
}


// Returns a pointer to the renderer.
Renderer* MandelbrotViewer::getRenderer()
{
	return &m_renderer;
}


// Starts the computation threads to generate the set
void MandelbrotViewer::startComputeThreads()
{
	if (THREAD_COUNT_X == 0 || THREAD_COUNT_Y == 0)
	{
		m_log.write("\nThread bounds invalid, stopping execution");
		setQuitting(true);
		return;
	}

	// Start threads to compute the set
	for (int x = 0; x < THREAD_COUNT_X; ++x)
	{
		for (int y = 0; y < THREAD_COUNT_Y; ++y)
		{
			m_log.lockMutex();
			m_log.write("\nStarting thread ");
			m_log.write(Helpers::toString(x));
			m_log.write("-");
			m_log.write(Helpers::toString(y));
			m_log.unlockMutex();

			m_computeThreads.push_back(new std::thread(&MandelbrotViewer::computeMandelbrotSet, this, x, y));
		}
	}
}

void MandelbrotViewer::joinComputeThreads()
{
	for (std::vector<std::thread*>::iterator iter = m_computeThreads.begin(); 
		iter != m_computeThreads.end(); ++iter)
	{
		(*iter)->join();
		delete (*iter);
	}
}

void MandelbrotViewer::update()
{
	while (!m_quitting)
	{
		if (m_updateThreadInterrupt)
			return;

		// Get the current clock time.
		clock_t time = clock();

		// Only update the MandelbrotViewer logic every UPDATE_DELAY.
		if (time - m_updateTimer > UPDATE_DELAY)
		{
			// This stops the image skewing as the view is zoomed in/out.
			// It is determined by abs(top) + abs(bottom) / abs(left) + abs(right).
			const double magicRatio = 0.75;

			if (m_inputMgr.isKeyDown(Keys::UP_ARROW))
			{
				if(m_rightSetValue - m_leftSetValue < m_zoomFactor * 5)
				{
					m_zoomFactor *= 0.1;
				}

				m_topSetValue -= m_zoomFactor * magicRatio;
				m_leftSetValue += m_zoomFactor;
				m_bottomSetValue += m_zoomFactor * magicRatio;
				m_rightSetValue -= m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::DOWN_ARROW))
			{
				if(m_rightSetValue - m_leftSetValue > m_zoomFactor * 50)
				{
					m_zoomFactor *= 10;
				}

				m_topSetValue += m_zoomFactor * magicRatio;
				m_leftSetValue -= m_zoomFactor;
				m_bottomSetValue -= m_zoomFactor * magicRatio;
				m_rightSetValue += m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::SPACEBAR))
			{
				std::string snapshot("\set snapshot: [" +
					Helpers::toString(m_zoomFactor) +
					"] " + Helpers::toString(m_topSetValue) + 
					" " + Helpers::toString(m_bottomSetValue) + 
					" " + Helpers::toString(m_leftSetValue) +
					" " + Helpers::toString(m_rightSetValue));

				m_log.lockMutex();
				m_log.write(snapshot);
				m_log.unlockMutex();
			}


			if (m_inputMgr.isKeyDown(Keys::W))
			{
				m_topSetValue += m_zoomFactor;
				m_bottomSetValue += m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::A))
			{
				m_leftSetValue -= m_zoomFactor;
				m_rightSetValue -= m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::S))
			{
				m_topSetValue -= m_zoomFactor;
				m_bottomSetValue -= m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::D))
			{
				m_leftSetValue += m_zoomFactor;
				m_rightSetValue += m_zoomFactor;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::ADD))
			{
				m_maxIterations += 8;
				m_needRedraw = true;
			}

			if (m_inputMgr.isKeyDown(Keys::SUBTRACT))
			{
				m_maxIterations -= 8;
				m_needRedraw = true;
			}

			if (m_needRedraw && m_state == GENERATING_STATE)
				m_computeThreadInterrupt = true;

			m_updateTimer = time;
		}
	}
}

void MandelbrotViewer::computeMandelbrotSet(int sliceIdX, int sliceIdY)
{
	clock_t startTime = clock();

	const int width = m_renderer.getFrameWidth();
	const int height = m_renderer.getFrameHeight();

	const double dSliceIdX = (double) sliceIdX;
	const double dSliceIdY = (double) sliceIdY;

	double sliceFactorX = (double) width / (double) THREAD_COUNT_X;
	double sliceFactorY = (double) height / (double) THREAD_COUNT_Y;

	const int lowBoundX = (int) ceil(dSliceIdX * sliceFactorX);
	const int lowBoundY = (int) ceil(dSliceIdY * sliceFactorY);
	const int highBoundX = (int) ceil((dSliceIdX + 1.0) * sliceFactorX);
	const int highBoundY = (int) ceil((dSliceIdY + 1.0) * sliceFactorY);

	m_log.lockMutex();
	m_log.write("\nThread ");
	m_log.write(Helpers::toString(sliceIdX));
	m_log.write("-");
	m_log.write(Helpers::toString(sliceIdY));
	m_log.write(" working from x: ");
	m_log.write(Helpers::toString(lowBoundX));
	m_log.write(" to ");
	m_log.write(Helpers::toString(highBoundX));
	m_log.write(", y: ");
	m_log.write(Helpers::toString(lowBoundY));
	m_log.write(" to ");
	m_log.write(Helpers::toString(highBoundY));
	m_log.unlockMutex();

	int highest;

	if (width > height)
		highest = width;
	else
		highest = height;

	for (int y = lowBoundY; y < highBoundY; ++y)
	{
		for (int x = lowBoundX; x < highBoundX; ++x)
		{
			if (m_computeThreadInterrupt)
				return;

			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			std::complex<double> c(m_leftSetValue + (x * (m_rightSetValue - m_leftSetValue) / width),
				m_topSetValue + (y * (m_bottomSetValue - m_topSetValue) / height));

			// Start off z at (0, 0).
			std::complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;

			while (abs(z) < 2.0 && iterations < m_maxIterations) 
			{
				z = (z * z) + c;
				++iterations;
			}

			m_rawImageData[(x * 3) + ((y * 3) * highest)] = abs(iterations - m_maxIterations);
			m_rawImageData[(x * 3) + ((y * 3) * highest) + 1] = abs(iterations - m_maxIterations / 2);
			m_rawImageData[(x * 3) + ((y * 3) * highest) + 2] = abs(iterations - m_maxIterations / 3);
		}
	}

	clock_t endTime = clock();

	m_log.lockMutex();
	m_log.write("\nThread ");
	m_log.write(Helpers::toString(sliceIdX));
	m_log.write("-");
	m_log.write(Helpers::toString(sliceIdY));
	m_log.write(" finished in ");
	m_log.write(Helpers::toString(endTime - startTime));
	m_log.write(" ms ");
	m_log.unlockMutex();
}

void MandelbrotViewer::render()
{	
	while (!m_quitting)
	{
		if (m_renderThreadInterrupt)
			return;

		// Get the current clock time.
		clock_t time = clock();

		// Only update the MandelbrotViewer logic every UPDATE_DELAY.
		if (time - m_renderTimer > RENDER_DELAY)
		{
			// This really shouldn't be called without pausing all of the computation threads (or having them write to a back buffer)
			// But corruption isn't really visible in the viewer so it doesn't matter
			SetDIBitsToDevice(*(m_renderer.getBackHdc()), 0, 0, m_renderer.getFrameWidth(), m_renderer.getFrameHeight(), 
				0, 0, 0, m_renderer.getFrameHeight(), m_rawImageData, m_renderer.getBitmapInfo(), DIB_RGB_COLORS);

			std::string output("Max Iterations: " + Helpers::toString(m_maxIterations));
			TextOut(*m_renderer.getBackHdc(), 50, 50, output.c_str(), output.size());

			m_renderer.push();

			m_renderTimer = time;
		}
	}
}