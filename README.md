# simple-os
This is a simple project for CS-4348. It simulates the interactions between the CPU and Memory via two distinct processes; it should be built using the associated src/*.cpp files with access to the include directory for the preprocessor. It features no other dependencies or libraries.

After building the executable, simply run it and pass it the name of an input file (several examples are provided in the data/ folder) to run as a user program. It can also optionally accept an integer timer value, which will determine the frequency at which timeouts occur.

Input files are assumed to have timeout logic at address 1000 onward (you can denote this with ".1000 // timer comments" on a line) and system call logic at address 1500 onward. Note that only valid instruction lines are counted; i.e., any lines that do not start with an integer and any characters after an integer are ignored by the simulated OS. The sole exception is when a line begins with ".", which is a shorthand for telling the Memory module to skip to that address for the next set of instructions to enter. This is typically used for implementing timer and interrupt logic as described above.
