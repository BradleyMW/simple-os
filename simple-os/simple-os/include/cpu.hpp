//
//  cpu.hpp
//  simple-os
//
//  Created by Bradley Wersterfer on 9/29/21.
//

#ifndef cpu_hpp
#define cpu_hpp

#include <stdio.h>
#include "common_data.hpp"

enum EXECUTION_MODE {KERNEL, USER};

class CPU
{
private:
    // These represent the registers that the CPU will use.
    int _PC,     // Program Counter (Starts at Beginning of User Memory)
        _SP,     // Stack Pointer (Starts at End of User Memory)
        _IR,     // Instruction Register
        _AC,
        _X,
        _Y;
    
    bool _mode;
    
    // Handles for each communication pipe.
    int _in,     // memory to cpu
        _out;    // cpu to memory
    
    // Processes the instruction currently stored in the IR.
    void _process();
    
    /**
     * Fetch the next value from memory.
     * @return: The next load value.
     */
    int _read_operand();
    
    /**
     * Fetch the value in memory at a specific address.
     * @arg addr: The numerical address to check (0-1999).
     * @return: The value in memory at that address.
     */
    int _read_address(int addr);
    
    /**
     * Save the given value into this address.
     * @arg addr: The address to save to.
     * @arg val: The value to store.
     */
    void _write(int addr, int val);
    
    /**
     * Given an integer, push it onto the top of the stack in user memory.
     * @arg number: The value to save into the stack. Could be an address or data.
     */
    void _push(int number);
    
    /**
     * Fetch the top value from the stack in user memory.
     * @return: The most recent number that was stored in the stack.
     */
    int _pop();
    
public:
    /**
     * The CPU will be initialized to contain all 0's upon creation.
     * @arg input_pipe: The mem_to_cpu read end of a pipe [0]
     * @arg output_pipe: The cpu_to_mem write end of a pipe [1]
     */
    CPU(int input_pipe, int output_pipe);
    
    // Perform the instruction currently located on the Program Counter.
    void execute();
};

#endif /* cpu_hpp */
