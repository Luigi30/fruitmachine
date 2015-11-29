using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace _6502EmulatorFrontend
{
    public class M6502Model
    {
        public bool FLAG_SIGN = false;          //N
        public bool FLAG_OVERFLOW = false;      //V
                                                //-
        public bool FLAG_BREAKPOINT = false;    //B
        public bool FLAG_DECIMAL = false;       //D
        public bool FLAG_INTERRUPT = false;     //I
        public bool FLAG_ZERO = false;          //Z
        public bool FLAG_CARRY = false;         //C

        public byte accumulator = 0;        //A
        public byte indexX = 0;             //X
        public byte indexY = 0;             //Y
        public byte stackPointer = 0;       //SP
        public ushort programCounter = 0;   //PC
        public ushort oldProgramCounter = 0;//before this opcode ran
        public ushort cycles = 0;           //cycle count of frame

        public String lastOpcode = "";

        public bool loadSuccess;
    }
}
