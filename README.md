# simple-os
This is a simple project for CS-4348. It simulates the interactions between the CPU and Memory via two distinct processes; it should be built using the associated src/*.cpp files with access to the include directory for the preprocessor. It features no other dependencies or libraries.

Here is a full list of the files necessary to build the application:
src/main.cpp
src/memory.cpp
src/cpu.cpp

Also ensure that the include folder is on the search path for the preprocessor. In particular, these files must be able to be found:
include/common_data.hpp
include/memory.hpp
include/cpu.hpp

An example command for compilation is below:
g++ src/main.cpp src/cpu.cpp src/memory.cpp -I./include/ -o simpleos

After building the executable, simply run it and pass it the name of an input file (several examples are provided in the data/ folder) to run as a user program. It can also optionally accept an integer timer value, which will determine the frequency at which timeouts occur.

An example run command could be:
./simpleos sample5.txt 30

Input files are assumed to have timeout logic at address 1000 onward (you can denote this with ".1000 // timer comments" on a line) and system call logic at address 1500 onward. Note that only valid instruction lines are counted; i.e., any lines that do not start with an integer and any characters after an integer are ignored by the simulated OS. The sole exception is when a line begins with ".", which is a shorthand for telling the Memory module to skip to that address for the next set of instructions to enter. This is typically used for implementing timer and interrupt logic as described above.

A more detailed breakdown of each file follows below:

main.cpp Is the driver code for the application. It will spawn a child process using the UNIX fork() command; the child process represents a memory module and the parent process represents the CPU. Each process can only communicate with each other via pipes set up by this file, and the bulk of processing is handled by the respective class files.

common_data.hpp Is a simple utility file that contains various shared data definitions, such as an enum for the type of command being sent to memory (READ, WRITE, or TERMINATE) and one for the types of instructions that the CPU supports. It is included by both the memory.hpp and cpu.hpp files.

memory.hpp Contains the class definition of Memory, which centralizes logic for the memory module of the simulated OS. It features one primary front-facing function, Cycle(), which simply puts it into an "infinite" while loop of waiting for requests from an input pipe.

memory.cpp Is the Memory module code. It contains the function implementations for the Memory class from memory.hpp, and Cycle() in particular features the request-fetch loop that the CPU relies upon. It first receives a type of command, then prepares itself to either read or write data. If reading, it will send back the data at the given address; if writing, it will overwrite the data at the given address with a desired value. This file also contains the public constructor for the class, which implements its own ReadUserProgram() function to perform file I/O on the user program file and load valid instructions into its internal memory array.

cpu.hpp Provides the definition for the CPU class. It provides several private internal functions such as pushing and popping from a stack or read/write requests that send data across the output pipe to the Memory (and optionally read back a result). The main public function is execute(), which loops through the loaded instructions in memory until an End instruction has been reached.

cpu.cpp Is the concrete implementations of the above. Execute() simply reads the current instruction at the Program Counter into the Instruction Register and then calls process(), which switches based on the logic in the IR. A sequence of over 30 commands is supported; there are examples of each of these in the data folder.

