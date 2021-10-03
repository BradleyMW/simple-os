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
            else
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
        bool read_next;
        read(in, &read_next, sizeof(bool));
        //write(out, &read_next, sizeof(bool));  // Send an acknowledgment of read flag
        if(read_next)
        {
            // The memory component will wait to receive an address from the CPU.
            int address;
            read(in, &address, sizeof(int));
            
            // Once the address is requested, it will return the data stored at that address.
            const int instr = main_mem[address];
            write(out, &instr, sizeof(int));
            printf("Mem. wrote %d\n", instr);
            
            // Once the CPU has accepted the instruction, we check to see if the command
            // was to shut down the system. If so, then the Memory can exit.
            if(instr == End) {
                printf("Mem. ending now.\n");
                break;
            }
        }
        else
        {
            // Prepare the memory to save values instead of sending them.
            int address;
            read(in, &address, sizeof(int));
            
            // Send an acknowledgment that the address has been received.
            //bool ack = 1;
            //write(out, &ack, sizeof(bool));
            
            // Now accept the value to store.
            printf("MainMem[%d] was %d, ", address, main_mem[address]);
            int val;
            read(in, &val, sizeof(int));
            main_mem[address] = val;
            printf("now it's %d\n", val);
            
            // Send an acknowledgement that the save has been completed.
            //write(out, &ack, sizeof(bool));
            
        } // end else write
    } // end while true
}
