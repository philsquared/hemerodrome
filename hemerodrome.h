#ifndef HEMERODROME_H
#define HEMERODROME_H

#include <string>
#include <vector>
#include <optional>
#include <chrono>

// platform dependent (!TDB will be conditionally compiled)
#include <unistd.h>
namespace hemerodrome {
   using Pid = pid_t;
}

// platform independent
namespace hemerodrome {

    enum class ForkBehaviour { Fork, DontFork };

    class Process {
        Pid m_pid;

    public:
        Process( std::string const& exeName, std::vector<std::string> args, ForkBehaviour forkBehaviour = ForkBehaviour::Fork );

        auto wait_for( std::chrono::milliseconds timeout ) -> std::optional<int>;

        auto pid() const -> Pid { return m_pid; }
    };

} // namespace hemerodrome

#endif // HEMERODROME_H
