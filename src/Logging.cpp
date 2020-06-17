#include "Logging.h"

bool Logging::m_logOpened = false;

Logging::Logging()
{
	if (m_logOpened)
		return;

	m_log = new std::ofstream("log.txt");
	m_logOpened = true;

	write("Log opened");
}

Logging::~Logging()
{
	if (!m_logOpened)
		return;

	m_log->close();
}

void Logging::write(std::string input)
{
	if (!m_logOpened)
		return;

	(*m_log) << input.c_str();
	m_log->flush();
}

void Logging::lockMutex()
{
	m_logMutex.lock();
}

void Logging::unlockMutex()
{
	m_logMutex.unlock();
}