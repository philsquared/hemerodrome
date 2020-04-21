#include "hemerodrome.h"

#include <iostream>

// This test front end just executes the process supplied as the first argument,
// forwarding any subsequent arguments to it
int main( int argc, char* argv[] ) {
//int main() {
//    int argc = 2;
//    std::vector<char*> argv = { "", "C:\\Users\\phil\\source\\repos\\LazyTest\\Debug\\LazyTest.exe" };

    try {
        if( argc < 2 )
            throw std::domain_error( "No process name supplied" );

        // Copy args into vector
        std::vector<std::string> args;
        for( int i = 2; i < argc; ++i )
            args.push_back( argv[i] );

        // Launch the child process
        std::cout << "Executing process: " << argv[1] << "\n";
        hemerodrome::Process p( argv[1], args );

        // If we got here with no exceptions then we have a child process running
        std::cout << "child process started with ID: " << p.pid() << "\n";

        int killAfter = 5;

        // Now wait, in a sleepy loop, for the child to finish
        while( true ) {
            auto result = p.wait_for( std::chrono::seconds(1) );
            if( result ) {
                std::cout << "Child process exited with code: " << *result << "\n";
                break;
            }
            else {
                // Our wait timed out - but we're going to try again anyway.
                std::cout << "Timed out\n";
            }
            if( --killAfter == 0 ) {
                std::cout << "Terminating...\n";
                p.terminate();
            }

        }
    }
    catch( std::exception& e ) {
        // Any errors from the system calls will have been mapped to std::error_codes and wrapped in std::system_error
        std::cout << "Failure: " << e.what() << "\n";
    }
}
