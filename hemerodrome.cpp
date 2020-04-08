#include "hemerodrome.h"

#include <iostream>
#include <system_error>
#include <thread>

// platform dependent
#include <stdlib.h>

namespace hemerodrome {

    namespace {
        void throw_errno(std::string const &context) {
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, context);
        }
    }

    Process::Process(std::string const& exeName, std::vector<std::string> args, ForkBehaviour forkBehaviour )
    : m_pid(forkBehaviour == ForkBehaviour::Fork ? fork() : 0 )
    {
        if( m_pid == 0 ) {
            // child process or unforked main process

            // Copy char* pointers to std::string args for system call
            std::vector<char*> cargs;
            cargs.reserve( args.size()+1 );
            cargs.push_back( const_cast<char*>( exeName.c_str() ) );
            for( auto const& arg : args )
                cargs.push_back( const_cast<char*>( arg.c_str() ) );
            cargs.push_back(nullptr );

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

    auto Process::wait_for( std::chrono::milliseconds timeout ) -> std::optional<int> {
        auto start = std::chrono::high_resolution_clock::now();
        while( true ) {
            int childExitStatus;
            pid_t ws = waitpid( m_pid, &childExitStatus, WNOHANG );

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

} // namespace hemerodrome
