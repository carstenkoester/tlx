#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#define L(x) LOG4CXX_INFO(loggerFunctionA, x)

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

// Define static logger variable
LoggerPtr loggerMyMain(Logger::getLogger( "main"));
LoggerPtr loggerFunctionA(Logger::getLogger( "functionA"));

void functionA()
{
    LOG4CXX_INFO(loggerFunctionA, "Executing functionA.");
}

int main()
{
    int value = 5;

    // Load XML configuration file using DOMConfigurator
    DOMConfigurator::configure("Log4cxxConfig.xml");

    LOG4CXX_TRACE(loggerMyMain, "this is a debug message for detailed code discovery. Value=" << value);
    LOG4CXX_INFO(loggerFunctionA, "FA this is a debug message for detailed code discovery. Value=" << value);
    L("TEST TEST TEST this is a debug message for detailed code discovery. Value=" << value);
    LOG4CXX_DEBUG(loggerMyMain, "this is a debug message.");
    LOG4CXX_INFO (loggerMyMain, "this is a info message, ignore. Value=" << value);
    LOG4CXX_WARN (loggerMyMain, "this is a warn message, not too bad.");
    LOG4CXX_ERROR(loggerMyMain, "this is a error message, something serious is happening.");
    LOG4CXX_FATAL(loggerMyMain, "this is a fatal message!!!");

    functionA();

    return 0;
}

