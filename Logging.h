/* Logging.h
*
* Simple logging mutex wrapper. */

#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <mutex>

class Logging
{
public:
	Logging();
	~Logging();

	void write(std::string input);
	void lockMutex();
	void unlockMutex();

private:
	static bool m_logOpened;
	std::mutex m_logMutex;
	std::ofstream* m_log;

};

#endif // LOGGING_H