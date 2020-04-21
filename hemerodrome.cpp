#include "hemerodrome.h"

#include <iostream>
#include <system_error>
#include <thread>
#include <numeric>

// platform dependent
#ifdef HEMERODROME_PLATFORM_POSIX

#include <stdlib.h>

namespace {
    [[noreturn]]
    void throw_errno(std::string const& context) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, context);
    }
}

namespace hemerodrome {
    Process::Process( std::string const& exeName, std::vector<std::string> args )
    : m_pid( fork() )
    {
        if( m_pid == 0 ) {
            // child process or unforked main process

            // Copy char* pointers to std::string args for system call
            std::vector<char*> cargs;
            cargs.reserve( args.size()+1 );
            cargs.push_back( const_cast<char*>( exeName.c_str() ) );
            for( auto const& arg : args )
                cargs.push_back( const_cast<char*>( arg.c_str() ) );
            cargs.push_back( nullptr );

            execv( exeName.c_str(), &cargs[0] );

            // If we got here then execv failed (otherwise it would have replaced the process image)
            std::error_code ec( errno, std::system_category() );
            std::cerr << "Unable to create child process from " + exeName + ": " + ec.message() << std::endl;
            exit(1);
        }
        else if( m_pid < 0 ) {
            throw_errno( "Error forking process" );
        }
    }
    Process::~Process() = default;

    auto Process::wait_for( std::chrono::milliseconds timeout ) -> std::optional<int> {
        auto start = std::chrono::high_resolution_clock::now();
        while( true ) {
            int childExitStatus;
            auto ws = waitpid( m_pid, &childExitStatus, WNOHANG );

            // child process exited normally
            if( ws != 0 )
                return { WEXITSTATUS( childExitStatus ) };

            // have we timed out?
            auto now = std::chrono::high_resolution_clock::now();
            if( now - start >= timeout )
                return {};

            // if not, sleep a little before trying again
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        };
    }

    void Process::terminate() {
        throw std::logic_error("not implemented");
    }

} // namespace hemerodrome

#elif defined (HEMERODROME_PLATFORM_WINDOWS)

namespace {
    [[noreturn]]
    void throw_last_error( std::string const& context ) {
        std::error_code ec( GetLastError(), std::system_category() );
        throw std::system_error( ec, context );
    }
}

namespace hemerodrome {
    Process::Process( std::string const& exeName, std::vector<std::string> args )
    {
        auto argSize = std::accumulate( args.begin(), args.end(), size_t(), []( size_t size, std::string const& arg ) { return size + arg.size() + 1; } );

        std::vector<char> commandLine( exeName.size() + 1 + argSize, 0 );
        strncpy_s( commandLine.data(), commandLine.size(), exeName.c_str(), exeName.size() );
        STARTUPINFOA startupInfo{ 0 };
        startupInfo.cb = sizeof(STARTUPINFO);

        if( !CreateProcessA(
            exeName.c_str(),
            commandLine.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &startupInfo,
            &m_procInfo)) {

            throw_last_error( "creating process" );
        }

        m_pid = m_procInfo.dwProcessId;
    }
    Process::~Process() {
        CloseHandle( m_procInfo.hProcess );
        CloseHandle( m_procInfo.hThread );
    }

    auto Process::wait_for( std::chrono::milliseconds timeout ) -> std::optional<int> {
        auto t = timeout.count();
        WaitForSingleObject( m_procInfo.hProcess, timeout.count() );
        DWORD exitCode;
        GetExitCodeProcess( m_procInfo.hProcess, &exitCode );
        // !TBD: check return code for failure?
        if( exitCode == STILL_ACTIVE )
            return {};
        else
            return { static_cast<int>( exitCode ) };
    }

    void Process::terminate() {
        TerminateProcess( m_procInfo.hProcess, 255 );
        // !TBD: check function return code?
        // !TDB: wait for exit?
    }

} // namespace hemerodrome

#endif
