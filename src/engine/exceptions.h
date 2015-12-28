#ifndef _HAVE_EXCEPTIONS_H
#define _HAVE_EXCEPTIONS_H

#include <exception>

#define EXCEPTION(exception_name, exception_string)        \
    class exception_name: public std::exception            \
    {                                                      \
        public:                                            \
            virtual const char* what() const throw()       \
            {                                              \
                return exception_string;                   \
            }                                              \
    };
  
#endif // _HAVE_EXCEPTIONS_H
