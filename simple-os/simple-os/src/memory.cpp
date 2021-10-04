//
//  memory.cpp
//  simple-os
//
//  Created by Bradley Wersterfer on 9/29/21.
//

#include "memory.hpp"

#include <unistd.h>
#include <sstream>
#include <fstream>

// A utility integer parsing function for converting C-Strings in our formatting standard to an integer.
int parseInt(std::string buf, bool skip_first_char)
{
    int i = 0;
    if(skip_first_char)
        i = 1;
    
    // Will only continue reading digits until the first non-digit character
    int num = 0;
    while(i < buf.length() && buf[i] >= '0' && buf[i] <= '9') {
        num = num*10 + (buf[i] - '0');
        i++;
    }
        
    return num;
}

Memory::Memory(int input_pipe, int output_pipe, std::string program_file_path)
{
    // When setting up memory, make sure that everything starts empty.
    for(int i = 0; i < MEM_SIZE; i++)
        main_mem[i] = 0;
    
    // Also attach the pipe handles.
    in = input_pipe;
    out = output_pipe;
    
    ReadUserProgram(program_file_path);
}

void Memory::ReadUserProgram(std::string program_file_path)
{
    std::ifstream input;
    input.open(program_file_path);
    if(input.good())
    {
        // The location that this instruction will be stored in memory
        int load_address = 0;
        
        //char buffer[BUF_SIZE];
        std::string buffer;
        while(std::getline(input, buffer))
        {
            bool starts_with_period = buffer[0]=='.';
            if(starts_with_period)
            {
                /* If the instruction began with a . then it is simply
                 * changing the load address that this instruction should
                 * be stored at. */
                load_address = parseInt(buffer, true);
            }
            // Skip any lines that do not contain a valid instruction (must start with int)
            else if(buffer[0] >= '0' && buffer[0] <= '9')
            {
                /* If the line started with a number, it is assumed to
                 * contain a valid instruction. Store it at the current load
                 * address and then advance the load address by one space
                 * in memory. */
                main_mem[load_address] = parseInt(buffer, false);
                load_address++;
            }
        } // end while(getline)
    } // end if input.good()
    input.close();
}

void Memory::Cycle()
{
    // Once the memory is initialized, it will simply cycle waiting for requests from the
    // CPU. When it receives a request, it returns the instruction at that address. If
    // the returned instruction matches the "End" request, then the Memory also exits.
    while(true)
    {
        CMD next_op;
        read(in, &next_op, sizeof(CMD));
        if(next_op == READ)
        {
            // The memory component will wait to receive an address from the CPU.
            int address;
            read(in, &address, sizeof(int));
            
            // Once the address is requested, it will return the data stored at that address.
            const int instr = main_mem[address];
            write(out, &instr, sizeof(int));
        }
        else if (next_op == WRITE)
        {
            // Prepare the memory to save values instead of sending them.
            int address;
            read(in, &address, sizeof(int));
            
            // Now accept the value to store.
            int val;
            read(in, &val, sizeof(int));
            main_mem[address] = val;
            
        } // end else write
        else if (next_op == TERMINATE)
            break;
    } // end while true
}
