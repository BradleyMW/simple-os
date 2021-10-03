//
//  cpu.cpp
//  simple-os
//
//  Created by Bradley Wersterfer on 9/29/21.
//

#include "cpu.hpp"

#include <unistd.h>
#include <time.h>
#include <cstdlib>  // Used for std::rand

#include <string>

CPU::CPU(int input_pipe, int output_pipe)
{
    // Initialize all of the registers
    _PC = 0;    // Begin running user program from 0 in main memory
    _IR = -1;   // Initialize instruction to -1 to ensure that it is fetched from memory
    _SP = 999;  // User Memory covers indices 0-999, and the stack stretches down
    _AC = 0;
    _X = 0;
    _Y = 0;
    
    // Attach the pipe handles to this class.
    _in = input_pipe;
    _out = output_pipe;
    
    // Set a random seed for number generation.
    srand(time(NULL));
}

void CPU::execute()
{
    while(_IR != End)
    {
        /*bool read_next = true;
        
        
        // First, send the PC to the memory module as the instruction address.
        const int addr = _PC;
        write(_out, &addr, sizeof(int));
        printf("CPU requested %d\n", addr);
        
        // Read in the instruction stored at that address.
        int instr;
        read(_in, &instr, sizeof(int));
        _IR = instr;   // Store the instruction in the IR register
        */
        
        _IR = _read_address(_PC);
        
        printf("CPU read %d\n", _IR);
        
        
        // Once the instruction has been retrieved, process it.
        _PC++;
        _process();
        
        printf("CPU completed line #%d\n", _PC);
    }
}

int CPU::_read_operand()
{
    // Read in the next value from memory.
    char operand[sizeof(int)];
    read(_in, operand, sizeof(int));
    return(atoi(operand));
}

int CPU::_read_address(int addr)
{
    bool read_next = true;
    write(_out, &read_next, sizeof(bool));
    //read(_in, &read_next, sizeof(bool));    // Wait for acknowledgment that pipe was read
    
    write(_out, &addr, sizeof(int));
    
    // Read in the value that is stored at that address.
    int val;
    read(_in, &val, sizeof(int));
    return(val);
}

void CPU::_write(int addr, int val)
{
    bool read_next = false;
    write(_out, &read_next, sizeof(bool));
    //read(_in, &read_next, sizeof(bool));    // Wait for acknowledgment that pipe was read
    
    // First write out the address, then wait for memory to verify it's ready.
    write(_out, &addr, sizeof(int));
    //bool ack;
    //read(_in, &ack, sizeof(bool));
    
    // Now, send in the value for memory to save. Make sure memory is ready after.
    write(_out, &val, sizeof(int));
    //read(_in, &ack, sizeof(bool));
}

void CPU::_process()
{
    switch(_IR)
    {
        // Load the next line's value into the AC.
        case Load_Val:
        {
            _AC = _read_address(_PC);   // Directly store the next line
            printf("=Loaded Val %d into _AC\n", _AC);
            _PC++;                      // Advance the Program Counter after
            break;
        }
        
        // Load the value from the address on the next line into the AC.
        case Load_Addr:
        {
            printf("==Load_Addr\n");
            int addr = _read_address(_PC);  // We then store the value from that address
            _AC = _read_address(addr);
            _PC++;
            break;
        }
            
        // Load the value from the address found in the given address into the AC
        // (for example, if LoadInd 500, and 500 contains 100, then load from 100).
        case LoadInd_Addr:
        {
            printf("===LoadInd_Addr\n");
            int ind = _read_address(_PC);   // Address from the next line
            _PC++;                          // Advance PC to the line afterward
            int addr = _read_address(ind);  // Address at that address
            _AC = _read_address(addr);      // Load the value there into AC
            break;
        }
          
        // Load the value at (address+X) into the AC
        // (for example, if LoadIdxX 500, and X contains 10, then load from 510).
        case LoadIdxX_Addr:
        {
            int addr = _read_address(_PC);  // Address from the next line
            _PC++;                          // Advance the PC past that line
            _AC = _read_address(addr + _X); // Load from that address + X
            break;
        }
            
        // Load the value at (address+Y) into the AC
        // (for example, if LoadIdxY 500, and Y contains 10, then load from 510).
        case LoadIdxY_Addr:
        {
            int addr = _read_address(_PC);  // Address from the next line
            _PC++;                          // Advance the PC past that line
            _AC = _read_address(addr + _Y); // Load from that address + Y
            break;
        }
            
        // Load from (Sp+X) into the AC (if SP is 990, and X is 1, load from 991).
        case LoadSpX:
        {
            _AC = _read_address(_SP + _X);
            _PC++;
            break;
        }
            
        // Store the value in the AC into the address
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
        case Store_Addr:
        {
            printf("=============SAVING A VALUE===============\n");
            int addr = _read_address(_PC);
            _PC++;
            _write(addr, _AC);
            break;
        }
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
            /* = = = = = = = = = = = = = = = = = */
            
        // Gets a random int from 1 to 100 into the AC
        case Get:
        {
            _AC = (rand() % 100) + 1;
            printf("Fetching a random number %d\n", _AC);
            break;
        }
        
        // Print the AC to screen depending on given Port argument
        case Put_Port:
        {
            int port = _read_address(_PC);
            _PC++;
            
            // If port=1, writes AC as an int to the screen
            // If port=2, writes AC as a char to the screen
            if(port == 1)
                printf("%d\n", _AC);
            else if(port == 2)
                printf("%c\n", _AC);
            
            break;
        }
            
        // Add the value in X to the AC
        case AddX:
        {
            _AC += _X;
            break;
        }
            
        // Add the value in Y to the AC
        case AddY:
        {
            _AC += _Y;
            break;
        }
            
        // Subtract the value in X from the AC
        case SubX:
        {
            _AC -= _X;
            break;
        }
            
        // Subtract the value in Y from the AC
        case SubY:
        {
            _AC -= _Y;
            break;
        }
            
        case End:
            
            break;
    }
}
