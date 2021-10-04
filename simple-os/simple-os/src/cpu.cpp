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
    _SP = 1000;  // User Memory covers indices 0-999, and the stack stretches down
    _AC = 0;
    _X = 0;
    _Y = 0;
    
    // Attach the pipe handles to this class.
    _in = input_pipe;
    _out = output_pipe;
    
    // Set a random seed for number generation and begin in user mode.
    srand(time(NULL));
    _mode = USER;
}

void CPU::execute()
{
    while(_IR != End)
    {
        // Read in the instruction stored at that address.
        _IR = _read_address(_PC);
        if(_IR==End)
            printf("_PC addr %d read in %d to _IR. Terminating\n", _PC, _IR);
        
        // Once the instruction has been retrieved, process it.
        _PC++;
        _process();
    }
}

int CPU::_read_address(int addr)
{
    // Check to ensure that the user does not write in system memory (only kernel can).
    if(addr >= 1000 && _mode != KERNEL)
    {
        // Notify the user that the process failed and exit to avoid damaging memory.
        printf("ERROR: User attempted to access address %d from system memory!\n", addr);
        exit(1);
    }
    
    // Notify that a read op will occur and then request an address.
    CMD read_next = READ;
    write(_out, &read_next, sizeof(CMD));
    
    write(_out, &addr, sizeof(int));
    
    // Read in the value that is stored at that address.
    int val;
    read(_in, &val, sizeof(int));
    return(val);
}

void CPU::_write(int addr, int val)
{
    // Check to ensure that the user does not write in system memory (only kernel can).
    if(addr >= 1000 && _mode != KERNEL)
    {
        // Notify the user that the process failed and exit to avoid damaging memory.
        printf("ERROR: User attempted to access address %d from system memory!\n", addr);
        CMD term = TERMINATE;
        write(_out, &term, sizeof(CMD));
        exit(1);
    }
    
    // Notify that next op will be a write, then send over the address.
    CMD write_next = WRITE;
    write(_out, &write_next, sizeof(CMD));
    
    write(_out, &addr, sizeof(int));
    
    // Now, send in the value for memory to save.
    write(_out, &val, sizeof(int));
}

void CPU::_push(int number)
{
    // First, we store the number in the next available stack spot, and then move pointer
    _SP--;
    _write(_SP, number);
}

int CPU::_pop()
{
    // Go back to the last element from the stack, and then return value stored there
    int val = _read_address(_SP);
    _SP++;
    return val;
}

void CPU::_process()
{
    switch(_IR)
    {
        // Load the next line's value into the AC.
        case Load_Val:
        {
            _AC = _read_address(_PC);   // Directly store the next line
            _PC++;                      // Advance the Program Counter after
            break;
        }
        
        // Load the value from the address on the next line into the AC.
        case Load_Addr:
        {
            int addr = _read_address(_PC);  // We then store the value from that address
            _AC = _read_address(addr);
            _PC++;
            break;
        }
            
        // Load the value from the address found in the given address into the AC
        // (for example, if LoadInd 500, and 500 contains 100, then load from 100).
        case LoadInd_Addr:
        {
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
            break;
        }
            
        // Store the value in the AC into the address
        case Store_Addr:
        {
            int addr = _read_address(_PC);
            _PC++;
            _write(addr, _AC);
            break;
        }
            
        // Gets a random int from 1 to 100 into the AC
        case Get:
        {
            _AC = (rand() % 100) + 1;
            //printf("Fetching a random number %d\n", _AC);
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
                printf("%d", _AC);
            else if(port == 2)
                printf("%c", _AC);
            
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
            
        // Copy the value in the AC to X
        case CopyToX:
        {
            _X = _AC;
            break;
        }
            
        // Copy the value in X to the AC
        case CopyFromX:
        {
            _AC = _X;
            break;
        }
            
        // Copy the value in the AC to Y
        case CopyToY:
        {
            _Y = _AC;
            break;
        }
                
        // Copy the value in Y to the AC
        case CopyFromY:
        {
            _AC = _Y;
            break;
        }
            
        // Copy the value in AC to the SP
        case CopyToSp:
        {
            _SP = _AC;
            break;
        }
            
        // Copy the value in SP to the AC
        case CopyFromSp:
        {
            _AC = _SP;
            break;
        }
           
        // Jump to the given address
        case Jump_Addr:
        {
            // We can jump directly to an address by updating the PC
            _PC = _read_address(_PC);
            break;
        }
            
        // Jump to the address only if the value in the AC is zero
        case JumpIfEqual_Addr:
        {
            // Read in the given address and advance PC in case we don't jump
            int addr = _read_address(_PC);
            _PC++;
            
            // If AC contains 0, then we go ahead with the jump
            if(_AC == 0)
                _PC = addr;
            break;
        }
        
        // Jump to the address only if the value in the AC is not zero
        case JumpIfNotEqual_Addr:
        {
            // Read in the given address and advance PC in case we don't jump
            int addr = _read_address(_PC);
            _PC++;
            
            // If AC is not 0, then we go ahead with the jump
            if(_AC != 0)
                _PC = addr;
            break;
        }
            
        // Push return address onto stack, then jump to the address
        case Call_Addr:
        {
            // First, fetch the address.
            int addr = _read_address(_PC);
            _PC++;
            
            // Once the address is fetched, we push current position and then jump.
            _push(_PC);
            _PC = addr;         // Fulfill the jump to function
            break;
        }
          
        // Pop return address from the stack, jump to the address
        case Ret:
        {
            _PC = _pop();
            break;
        }
            
        // Simply increment the value in X
        case IncX:
        {
            _X++;
            break;
        }
            
        // Simply decrement the value in X
        case DecX:
        {
            _X--;
            break;
        }
            
        // Push AC onto stack
        case Push:
        {
            _push(_AC);
            break;
        }
           
        // Pop from stack into AC
        case Pop:
        {
            _AC = _pop();
            break;
        }
            
        // Perform system call: switch modes, store SP and PC registers on system stack,
        // switch to use system stack instead of user one (handler may push/pop additional
        // values), and begin executing user code at address 1500.
        case Int:
        {
            // Nested interrupts are disabled during system calls or vice versa.
            if(_mode != KERNEL)
            {
                _mode = KERNEL;
                int old_sp = _SP;
                _SP = 2000;         // End of system memory is where system stack begins
                _push(old_sp);      // Store the current user memory stack pointer
                _push(_PC);         // along with the current instruction in user memory
                
                // Finally, set current location to interrupt handler (line 1500)
                _PC = 1500;
            }
            break;
        }
            
        // Return from system call. Need to undo previous pushes.
        case IRet:
        {
            // Ensure that we were actually in a system call before trying to read values.
            if(_mode == KERNEL)
            {
                _PC = _pop();   // PC was pushed second, so it is left on top
                _SP = _pop();   // SP was pushed first, so it should be the bottom
                
                // Switch back to user execution now that the stack and PC are restored.
                _mode = USER;
            }
            break;
        }

        // This command simply ends the program. There is no shutdown processing needed.
        case End:
        {
            CMD term = TERMINATE;
            write(_out, &term, sizeof(CMD));
            break;
        }
    }
}
