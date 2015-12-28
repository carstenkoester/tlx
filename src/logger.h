#ifndef _HAVE_LOGGER_H
#define _HAVE_LOGGER_H

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

// Define static logger variable
static LoggerPtr loggerMain(Logger::getLogger("main"));

#define LOG_DEBUG(x) LOG4CXX_DEBUG(loggerMain, x)
#define LOG_INFO(x) LOG4CXX_INFO(loggerMain, x)
#define LOG_WARN(x) LOG4CXX_WARN(loggerMain, x)
#define LOG_ERROR(x) LOG4CXX_ERROR(loggerMain, x)
#define LOG_FATAL(x) LOG4CXX_FATAL(loggerMain, x)

#endif // _HAVE_LOGGER_H
