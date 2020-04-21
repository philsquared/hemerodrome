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
#  define HEMERODROME_PLATFORM_POSIX
# endif

#elif defined(linux) || defined(__linux) || defined(__linux__)
# define HEMERODROME_PLATFORM_LINUX
# define HEMERODROME_PLATFORM_POSIX

#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
# define HEMERODROME_PLATFORM_WINDOWS
#else
 static_assert(false, "Only Windows, Linux and Mac supported");
#endif


namespace hemerodrome {

    // Platform dependent fwd decls
#if defined(HEMERODROME_PLATFORM_POSIX)
    class PosixProcess;
    using PlatformProcess = PosixProcess;
#elif defined(HEMERODROME_PLATFORM_WINDOWS)
    class WindowsProcess;
    using PlatformProcess = WindowsProcess;
#endif


    // platform independent wrapper
    class Process {

        std::unique_ptr<PlatformProcess> m_impl;

    public:
        Process( std::string const& exeName, std::vector<std::string> const& args );
        ~Process();

        auto wait_for( std::chrono::milliseconds timeout ) -> std::optional<int>;
        void terminate();

        auto pid() const -> int;
    };


} // namespace hemerodrome

#endif // HEMERODROME_H
