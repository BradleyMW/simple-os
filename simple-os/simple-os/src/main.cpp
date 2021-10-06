//
//  main.cpp
//  Created by Bradley Wersterfer on 9/29/21.
//
// This file contains the driver code for the application. It will spawn a
// child process using the UNIX fork() command; the child process
// represents a memory module and the parent process represents the CPU.
// Each process can only communicate with each other via pipes set up by
// this file, and the bulk of processing is handled by the respective
// class files.
//

#include <iostream>
#include <unistd.h>
#include <string>

// Headers for each half of the simulated machine to allow branching.
#include "memory.hpp"
#include "cpu.hpp"

/**
 * Provides a common framework for displaying and handling errors. Additional error-handling logic
 * can be implemented here.
 *
 * @arg msg: The error message to display to the user.
 * @arg code: An integer signal for error type to pass onward.
 */
void logError(std::string msg, int code = -1)
{
    std::cerr << msg << std::endl;
    exit(code);
}

int main(int argc, const char * argv[])
{
    // We know that a timer parameter can be given via the commandline.
    // argv[0] contains the program name, so the timer will be after that.
    int timer = 300;
    if(argc > 2)
        timer = std::atoi(argv[2]);
    
    // Also set up pipes for communicating each direction.
    int mem_to_cpu[2];
    int cpu_to_mem[2];
    
    if(pipe(mem_to_cpu) < 0) {
        logError("Error: A pipe from memory to CPU could not be created!\n");
    }
    if(pipe(cpu_to_mem) < 0) {
        logError("Error: A pipe from CPU to memory could not be created!\n");
    }
    
    // Branch the logic into a separate Memory and CPU handler
    pid_t child_pid = fork();
    if(child_pid < 0)
    {
        logError("Error: The child could not be created!\n");
    }
    // When fork() returns 0, it is the child process, or the MEMORY
    else if(child_pid == 0)
    {
        Memory m(cpu_to_mem[0], mem_to_cpu[1], argv[1]);
        m.Cycle();
    }
    // When pid > 0, it is the parent process, or the CPU
    else
    {
        CPU c(mem_to_cpu[0], cpu_to_mem[1], timer);
        c.execute();
    }
}
