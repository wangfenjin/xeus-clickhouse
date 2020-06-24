#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <sstream>

/* consider adding boost thread id since we'll want to know whose writting and
 * won't want to repeat it for every single call */

/* consider adding policy class to allow users to redirect logging to specific
 * files via the command line
 */

// clang-format off
// https://stackoverflow.com/a/6168353/1203241

enum loglevel_e
    {logERROR, logWARNING, logINFO, logDEBUG};

class logIt
{
public:
    logIt(loglevel_e _loglevel = logERROR) {
        _buffer << _loglevel << " :" 
            << std::string(
                _loglevel > logDEBUG 
                ? (_loglevel - logDEBUG) * 4 
                : 1
                , ' ');
    }

    template <typename T>
    logIt & operator<<(T const & value)
    {
        _buffer << value;
        return *this;
    }

    ~logIt()
    {
        _buffer << std::endl;
        // This is atomic according to the POSIX standard
        // http://www.gnu.org/s/libc/manual/html_node/Streams-and-Threads.html
        std::cerr << _buffer.str();
    }

private:
    std::ostringstream _buffer;
};

extern loglevel_e loglevel;

#define log(level) \
if (level > loglevel) ; \
else logIt(level)

#endif
// clang-format on
