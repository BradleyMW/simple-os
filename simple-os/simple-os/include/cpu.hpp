//
//  cpu.hpp
//  Created by Bradley Wersterfer on 9/29/21.
//
// Provides the definition for the CPU class. It provides several private
// internal functions such as pushing and popping from a stack or
// read/write requests that send data across the output pipe to the Memory
// (and optionally read back a result). The main public function is
// execute(), which loops through the loaded instructions in memory until
// an End instruction has been reached.
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
    int _timer_val, // The number of instructions that pass without timeout
        _time;      // The current number of instructions that have passed
    
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
     * @arg timer: The number of instructions that can pass before a timer interrupt occurs.
     */
    CPU(int input_pipe, int output_pipe, int timer);
    
    // Perform the instruction currently located on the Program Counter.
    void execute();
};

#endif /* cpu_hpp */
