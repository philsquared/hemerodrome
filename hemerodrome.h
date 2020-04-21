#ifndef HEMERODROME_H
#define HEMERODROME_H

#include <string>
#include <vector>
#include <optional>
#include <chrono>

#ifdef __APPLE__
# include <TargetConditionals.h>
# if TARGET_OS_OSX == 1
#  define HEMERODROME_PLATFORM_MAC
# elif TARGET_OS_IPHONE == 1
#  define HEMERODROME_PLATFORM_IPHONE
# endif

# define HEMERODROME_PLATFORM_POSIX

#elif defined(linux) || defined(__linux) || defined(__linux__)
# define HEMERODROME_PLATFORM_LINUX
# define HEMERODROME_PLATFORM_POSIX

#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
# define HEMERODROME_PLATFORM_WINDOWS
#endif

// platform dependent (!TDB will be conditionally compiled)
#ifdef HEMERODROME_PLATFORM_POSIX
#include <unistd.h>
namespace hemerodrome {
   using Pid = pid_t;
}
#elif defined(HEMERODROME_PLATFORM_WINDOWS)
#include "windows.h" // fwd declare DWORD, so we don't need windows.h in the header
namespace hemerodrome {
    using Pid = DWORD;
}
#else
namespace hemerodrome {
    using Pid = int;
}
static_assert(false, "Only Windows, Linux and Mac supported");
#endif


// platform independent
namespace hemerodrome {

    class Process {
        Pid m_pid;
#if defined(HEMERODROME_PLATFORM_WINDOWS)
        PROCESS_INFORMATION m_procInfo;
#endif
    public:
        Process( std::string const& exeName, std::vector<std::string> args );
        ~Process();

        auto wait_for( std::chrono::milliseconds timeout ) -> std::optional<int>;
        void terminate();

        auto pid() const -> Pid { return m_pid; }
    };

} // namespace hemerodrome

#endif // HEMERODROME_H
