#ifndef _HAVE_EXCEPTIONS_H
#define _HAVE_EXCEPTIONS_H

#include <exception>

class ChannelAlreadyPatchedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Attempting to patch channel to dimmer, but channel already patched to same dimmer";
        }
};

class AlreadyPatchedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
            return "Attempting to patch channel to dimmer, but dimmer already patched";
        }
};

class NotPatchedException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
             return "Attempting to unpatch channel from dimmer, but channel not patched";
        }
};

class ChannelInputSourceAlreadyRegisteredException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
             return "Attempting to register duplicate input source to channel";
        }
};

class ChannelNotInSceneException: public std::exception
{
    public:
        virtual const char* what() const throw()
        {
             return "Attempting to access a channel that is not part of the scene";
        }
};

#endif // _HAVE_EXCEPTIONS_H
