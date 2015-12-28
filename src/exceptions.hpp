//
// Copyright 2014 Carsten Koester <carsten@ckoester.net>
//
// This file is part of Foobar.
//
// Foobar is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Foobar is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar. If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _EXCEPTIONS_HPP
#define _EXCEPTIONS_HPP

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

#endif // _EXCEPTIONS_HPP
