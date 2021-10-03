//
//  memory.hpp
//  simple-os
//
//  Created by Bradley Wersterfer on 9/29/21.
//

#ifndef memory_hpp
#define memory_hpp

#include <stdio.h>
#include <string>

#include "common_data.hpp"

class Memory
{
private:
    // Memory will consist of 2000 integer entries:
    // 0-999 for the user program,
    // 1000-1999 for system code.
    const static int MEM_SIZE = 2000;
    const static int BUF_SIZE = 1024;
    int main_mem[MEM_SIZE];
    
    // Ends of UNIX-style pipes for communication
    int in,     // cpu to memory
        out;    // memory to cpu
    
    /**
     * Read the sequence of user instructions into memory so that the CPU can begin executing them.
     * Note that this is automatically called by the constructor upon initialization.
     * @arg program_file_path: The path to the file containing instructions from the user.
     */
    void ReadUserProgram(std::string program_file_path);
    
public:
    /**
     * The main memory class will be initialized to contain all 0's upon creation.
     * @arg input_pipe: The cpu_to_mem read end of a pipe [0]
     * @arg output_pipe: The mem_to_cpu write end of a pipe [1]
     * @arg program_file_path: The file path to the user's program containing instructions for the CPU.
     */
    Memory(int input_pipe, int output_pipe, std::string program_file_path);
    
    /**
     * Cycle will simply wait until the next instruction is sent by the CPU for either a read or a write.
     */
    void Cycle();
};

#endif /* memory_hpp */
