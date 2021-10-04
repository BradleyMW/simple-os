//
//  common_data.hpp
//  simple-os
//
//  Created by Bradley Wersterfer on 10/3/21.
//

#ifndef common_data_h
#define common_data_h

// This is the list of instructions provided in the assignment.
enum INSTR {Load_Val=1, Load_Addr=2, LoadInd_Addr=3, LoadIdxX_Addr=4, LoadIdxY_Addr=5,
    LoadSpX=6, Store_Addr=7, Get=8, Put_Port=9, AddX=10, AddY=11, SubX=12, SubY=13,
    CopyToX=14, CopyFromX=15, CopyToY=16, CopyFromY=17, CopyToSp=18, CopyFromSp=19,
    Jump_Addr=20, JumpIfEqual_Addr=21, JumpIfNotEqual_Addr=22, Call_Addr=23, Ret=24,
    IncX=25, DecX=26, Push=27, Pop=28, Int=29, IRet=30, End=50};

// A sequence of flags that can be sent to memory to command termination.
enum CMD { READ=0, WRITE=1, TERMINATE=2};

#endif /* common_data_h */
