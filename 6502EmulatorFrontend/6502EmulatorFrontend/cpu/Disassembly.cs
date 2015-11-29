using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace _6502EmulatorFrontend.cpu
{
    public class Disassembly
    {

        public struct DisassembledOpcode
        {
            public ushort Address;
            public byte Opcode;
            public String InstructionName;
            public List<byte> Operands;
            
            public override String ToString()
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(String.Format("{0} - {1}", Address.ToString("X4"), InstructionName));
                Operands.Reverse(); //absolute addresses are out of order
                foreach(var operand in Operands)
                {
                    List<String> branchInstructions = new List<string>() { "BCC", "BCS", "BEQ", "BMI", "BNE", "BPL", "BVC", "BVS" };

                    if (branchInstructions.Contains(InstructionName.Substring(0, 3)))
                    {
                        //if it's a branch instruction, use the operand to calculate the relative address destination
                        if((operand & 0x80) == 0x80)
                        {
                            //Relative branch backward
                            sb.Append(String.Format("{0}", (Address - ~operand - 0xFF).ToString("X2")));
                        }
                        else
                        {
                            //Relative branch forward
                            sb.Append(String.Format("{0}", (Address + operand).ToString("X2")));
                        }
                    } else
                    {
                        //Any other instruction
                        sb.Append(String.Format("{0}", operand.ToString("X2")));
                    }

                }
                return sb.ToString();
            }
        }

        public byte[] debugMemory = new byte[65536];

        public ushort Address;
        public byte Opcode;
        public String InstructionName;
        public List<byte> Operands;
        public ushort NextInstructionAddress;

        public Disassembly(byte[] memory)
        {
            memory.CopyTo(debugMemory, 0);
        }

        public void Begin(ushort address)
        {
            NextInstructionAddress = address;
            Next();
        }

        public void Next()
        {
            //cycle counts are inaccurate

            Operands = new List<byte>();
            Opcode = debugMemory[NextInstructionAddress];
            Address = NextInstructionAddress;
            int operandLength = 0;
            int cycles = 0;

            #region disassembly switch
            switch (Opcode)
            {
                case 0x00: //BRK
                    {
                        cycles += 7;
                        InstructionName = "BRK"; //OK
                        operandLength = 0;
                        break;
                    }

                case 0x01:
                    {
                        cycles += 6;
                        InstructionName = "ORA (ZP,X) $";
                        operandLength = 1;
                        break;
                    }

                case 0x05:
                    {
                        cycles += 2;
                        InstructionName = "ORA $";
                        operandLength = 1;
                        break;
                    }

                case 0x06:
                    {
                        cycles += 5;
                        InstructionName = "ASL $";
                        operandLength = 1;
                        break;
                    }

                case 0x08: //PHP
                    {
                        cycles += 3;
                        InstructionName = "PHP";
                        operandLength = 0;
                        break;
                    }

                case 0x09:
                    {
                        cycles += 2;
                        InstructionName = "ORA #$";
                        operandLength = 1;
                        break;
                    }

                case 0x0A:
                    {
                        cycles += 2;
                        InstructionName = "ASL";
                        operandLength = 0;
                        break;
                    }

                case 0x0D:
                    {
                        cycles += 4;
                        InstructionName = "ORA $";
                        operandLength = 2;
                        break;
                    }

                case 0x0E:
                    {
                        cycles += 6;
                        InstructionName = "ASL $";
                        operandLength = 2;
                        break;
                    }

                case 0x10:
                    {
                        cycles += 2; //2; +1 if dest on same page; +2 if on dest on diff page
                        InstructionName = "BPL $";
                        operandLength = 1;
                        break;
                    }

                case 0x11:
                    {
                        cycles += 5;
                        InstructionName = "ORA (M),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0x15:
                    {
                        cycles += 4;
                        InstructionName = "ORA ZP,X";
                        operandLength = 1;
                        break;
                    }

                case 0x16:
                    {
                        cycles += 6;
                        InstructionName = "ASL ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0x18: //CLC
                    {
                        cycles += 2;
                        InstructionName = "CLC";
                        operandLength = 0;
                        break;
                    }

                case 0x19:
                    {
                        cycles += 4;
                        InstructionName = "ORA M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x1D:
                    {
                        cycles += 4;
                        InstructionName = "ORA M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x1E:
                    {
                        cycles += 7;
                        InstructionName = "ASL M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x20:
                    {
                        cycles += 6;
                        InstructionName = "JSR $";
                        operandLength = 2;
                        break;
                    }

                case 0x21: //AND (M, X)
                    {
                        cycles += 6;
                        InstructionName = "AND (M,X) $";
                        operandLength = 2;
                        break;
                    }

                case 0x24:
                    {
                        cycles += 3;
                        InstructionName = "BIT $";
                        operandLength = 1;
                        break;
                    }

                case 0x25: //AND - AND zeropage
                    {
                        cycles += 2;
                        InstructionName = "AND $";
                        operandLength = 1;
                        break;
                    }

                case 0x26:
                    {
                        cycles += 5;
                        InstructionName = "ROL $";
                        operandLength = 1;
                        break;
                    }

                case 0x28: //PLP
                    {
                        cycles += 4;
                        InstructionName = "PLP";
                        operandLength = 0;
                        break;
                    }

                case 0x29: //AND - AND(M, immediate)
                    {
                        cycles += 2;
                        InstructionName = "AND #$";
                        operandLength = 1;
                        break;
                    }

                case 0x2A:
                    {
                        cycles += 2;
                        InstructionName = "ROL A";
                        operandLength = 0;
                        break;
                    }

                case 0x2C:
                    {
                        cycles += 4;
                        InstructionName = "BIT $";
                        operandLength = 2;
                        break;
                    }

                case 0x2D:
                    {
                        cycles += 4;
                        InstructionName = "AND $";
                        operandLength = 2;
                        break;
                    }

                case 0x2E:
                    {
                        cycles += 6;
                        InstructionName = "ROL $";
                        operandLength = 2;
                        break;
                    }

                case 0x30:
                    {
                        cycles += 2;
                        InstructionName = "BMI $";
                        operandLength = 1;
                        break;
                    }

                case 0x31: //AND (M),Y
                    {
                        cycles += 5;
                        InstructionName = "AND (M),Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x35: //AND,X - AND(A, zeropage+X)
                    {
                        cycles += 3;
                        InstructionName = "AND ZP,X";
                        operandLength = 1;
                        break;
                    }

                case 0x36:
                    {
                        cycles += 6;
                        InstructionName = "ROL ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0x38: //SEC - set carry flag
                    {
                        cycles += 2;
                        InstructionName = "SEC";
                        operandLength = 0;
                        break;
                    }

                case 0x39: //AND M,Y
                    {
                        cycles += 4;
                        InstructionName = "AND M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x3D:
                    {
                        cycles += 4;
                        InstructionName = "AND M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x3E:
                    {
                        cycles += 7;
                        InstructionName = "ROL M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x40:
                    {
                        cycles += 6;
                        InstructionName = "RTI";
                        operandLength = 0;
                        break;
                    }

                case 0x41:
                    {
                        cycles += 6;
                        InstructionName = "EOR (ZP,X) $";
                        operandLength = 1;
                        break;
                    }

                case 0x45:
                    {
                        cycles += 3;
                        InstructionName = "EOR $";
                        operandLength = 1;
                        break;
                    }

                case 0x46:
                    {
                        cycles += 5;
                        InstructionName = "LSR $";
                        operandLength = 1;
                        break;
                    }

                case 0x48:
                    {
                        cycles += 3;
                        InstructionName = "PHA";
                        operandLength = 0;
                        break;
                    }

                case 0x49:
                    {
                        cycles += 2;
                        InstructionName = "EOR #$";
                        operandLength = 1;
                        break;
                    }

                case 0x4A:
                    {
                        cycles += 2;
                        InstructionName = "LSR";
                        operandLength = 0;
                        break;
                    }

                case 0x4C: //JMP - unconditional branch
                    {
                        cycles += 3;
                        InstructionName = "JMP $";
                        operandLength = 2;
                        break;
                    }

                case 0x4D:
                    {
                        cycles += 4;
                        InstructionName = "EOR $";
                        operandLength = 2;
                        break;
                    }

                case 0x4E:
                    {
                        cycles += 6;
                        InstructionName = "LSR $";
                        operandLength = 2;
                        break;
                    }

                case 0x50:
                    {
                        cycles += 2;
                        InstructionName = "BVC";
                        operandLength = 1;
                        break;
                    }

                case 0x51:
                    {
                        cycles += 5;
                        InstructionName = "EOR (ZP),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0x55:
                    {
                        cycles += 4;
                        InstructionName = "EOR ZP,X";
                        operandLength = 1;
                        break;
                    }

                case 0x56:
                    {
                        cycles += 6;
                        InstructionName = "LSR ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0x58: //CLI
                    {
                        cycles += 2;
                        InstructionName = "CLI";
                        operandLength = 0;
                        break;
                    }

                case 0x59:
                    {
                        cycles += 4;
                        InstructionName = "EOR M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x5D:
                    {
                        cycles += 4;
                        InstructionName = "EOR M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x5E:
                    {
                        cycles += 7;
                        InstructionName = "LSR M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x60:
                    {
                        cycles += 6;
                        InstructionName = "RTS";
                        operandLength = 0;
                        break;
                    }

                case 0x61:
                    {
                        cycles += 6;
                        InstructionName = "ADC (ZP,X) $";
                        operandLength = 1;
                        break;
                    }

                case 0x65:
                    {
                        cycles += 3;
                        InstructionName = "ADC ZP $";
                        operandLength = 1;
                        break;
                    }

                case 0x66:
                    {
                        cycles += 5;
                        InstructionName = "ROR $";
                        operandLength = 1;
                        break;
                    }

                case 0x68: //PLA - pop accumulator from stack
                    {
                        cycles += 4;
                        InstructionName = "PLA";
                        operandLength = 0;
                        break;
                    }

                case 0x69:
                    {
                        cycles += 2;
                        InstructionName = "ADC #$";
                        operandLength = 1;
                        break;
                    }

                case 0x6A:
                    {
                        cycles += 2;
                        InstructionName = "ROR A";
                        operandLength = 0;
                        break;
                    }

                case 0x6C: //JMP (M)
                    {
                        cycles += 5;
                        InstructionName = "JMP (M) $";
                        operandLength = 2;
                        break;
                    }

                case 0x6D:
                    {
                        cycles += 4;
                        InstructionName = "ADC $";
                        operandLength = 2;
                        break;
                    }

                case 0x6E:
                    {
                        cycles += 6;
                        InstructionName = "ROR $";
                        operandLength = 2;
                        break;
                    }

                case 0x70:
                    {
                        cycles += 2;
                        InstructionName = "BVS $";
                        operandLength = 1;
                        break;
                    }

                case 0x71:
                    {
                        cycles += 5;
                        InstructionName = "ADC (ZP),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0x75:
                    {
                        cycles += 4;
                        InstructionName = "ADC ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0x76:
                    {
                        cycles += 6;
                        InstructionName = "ROR ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0x78: //SEI - set interrupt flag
                    {
                        cycles += 2;
                        InstructionName = "SEI";
                        operandLength = 0;
                        break;
                    }

                case 0x7E:
                    {
                        cycles += 7;
                        InstructionName = "ROR M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x79:
                    {
                        cycles += 4;
                        InstructionName = "ADC M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x7D:
                    {
                        cycles += 4;
                        InstructionName = "ADC M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0x81:
                    {
                        cycles += 6;
                        InstructionName = "STA (ZP,X)";
                        operandLength = 1;
                        break;
                    }

                case 0x84: //STY - store Y into zeropage memory - OK
                    {
                        cycles += 3;
                        InstructionName = "STY $";
                        operandLength = 1;
                        break;
                    }

                case 0x85: //STA - store A into zeropage memory - OK
                    {
                        cycles += 3;
                        InstructionName = "STA $";
                        operandLength = 1;
                        break;
                    }

                case 0x86: //STX - store X into zeropage memory - OK
                    {
                        cycles += 3;
                        InstructionName = "STX $";
                        operandLength = 1;
                        break;
                    }

                case 0x88: //DEY - OK
                    {
                        cycles += 2;
                        InstructionName = "DEY";
                        operandLength = 0;
                        break;
                    }

                case 0x8A: //TXA - X -> A - OK
                    {
                        cycles += 2;
                        InstructionName = "TXA";
                        operandLength = 0;
                        break;
                    }

                case 0x8C:
                    {
                        cycles += 4;
                        InstructionName = "STY $";
                        operandLength = 2;
                        break;
                    }

                case 0x8D:
                    {
                        cycles += 4;
                        InstructionName = "STA $";
                        operandLength = 2;
                        break;
                    }

                case 0x8E:
                    {
                        cycles += 4;
                        InstructionName = "STX $";
                        operandLength = 2;
                        break;
                    }

                case 0x90:
                    {
                        cycles += 2;
                        InstructionName = "BCC $";
                        operandLength = 1;
                        break;
                    }

                case 0x91:
                    {
                        cycles += 6;
                        InstructionName = "STA (ZP),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0x94:
                    {
                        cycles += 4;
                        InstructionName = "STY ZP,X $"; //OK
                        operandLength = 1;
                        break;
                    }

                case 0x95:
                    {
                        cycles += 4;
                        InstructionName = "STA ZP,X $"; //OK
                        operandLength = 1;
                        break;
                    }

                case 0x96:
                    {
                        cycles += 4;
                        InstructionName = "STX ZP,Y $"; //OK
                        operandLength = 1;
                        break;
                    }

                case 0x98: //TYA - Y -> A - OK
                    {
                        cycles += 2;
                        InstructionName = "TYA";
                        operandLength = 0;
                        break;
                    }

                case 0x99:
                    {
                        cycles += 5;
                        InstructionName = "STA M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0x9A: //TXS - X -> SP - OK
                    {
                        cycles += 2;
                        InstructionName = "TXS";
                        operandLength = 0;
                        break;
                    }

                case 0x9D:
                    {
                        cycles += 5;
                        InstructionName = "STA M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xA0: //LDY - immediate - OK
                    {
                        cycles += 2;
                        InstructionName = "LDY #$";
                        operandLength = 1;
                        break;
                    }

                case 0xA1:
                    {
                        cycles += 6;
                        InstructionName = "LDA (ZP,X)";
                        operandLength = 1;
                        break;
                    }

                case 0xA2: //LDX - immediate - OK
                    {
                        cycles += 2;
                        InstructionName = "LDX #$";
                        operandLength = 1;
                        break;
                    }

                case 0xA4:
                    {
                        cycles += 3;
                        InstructionName = "LDY $";
                        operandLength = 1;
                        break;
                    }

                case 0xA5: //LDA - Zeropage - Load A from zeropage operand.
                    {
                        cycles += 3;
                        InstructionName = "LDA $";
                        operandLength = 1;
                        break;
                    }

                case 0xA6:
                    {
                        cycles += 3;
                        InstructionName = "LDX $";
                        operandLength = 1;
                        break;
                    }

                case 0xA8: //TAY - OK
                    {
                        cycles += 2;
                        InstructionName = "TAY";
                        operandLength = 0;
                        break;
                    }

                case 0xA9: //LDA - Immediate - Load A from immediate value - OK
                    {
                        cycles += 2;
                        InstructionName = "LDA #$";
                        operandLength = 1;
                        break;
                    }

                case 0xAA: //TAX - A -> X - OK
                    {
                        cycles += 2;
                        InstructionName = "TAX";
                        operandLength = 0;
                        break;
                    }

                case 0xAC:
                    {
                        cycles += 4;
                        InstructionName = "LDY $";
                        operandLength = 2;
                        break;
                    }

                case 0xAD:
                    {
                        cycles += 4;
                        InstructionName = "LDA $";
                        operandLength = 2;
                        break;
                    }

                case 0xAE:
                    {
                        cycles += 4;
                        InstructionName = "LDX $";
                        operandLength = 2;
                        break;
                    }

                case 0xB0:
                    {
                        cycles += 2;
                        InstructionName = "BCS $";
                        operandLength = 1;
                        break;
                    }

                case 0xB1:
                    {
                        cycles += 5;
                        InstructionName = "LDA (ZP),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0xB4:
                    {
                        cycles += 4;
                        InstructionName = "LDY ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xB5: //LDA,X - Load A from Zeropage+X value
                    {
                        cycles += 4;
                        InstructionName = "LDA ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xB6:
                    {
                        cycles += 4;
                        InstructionName = "LDX ZP,Y $";
                        operandLength = 1;
                        break;
                    }

                case 0xB8: //CLV - OK
                    {
                        cycles += 2;
                        InstructionName = "CLV";
                        operandLength = 0;
                        break;
                    }

                case 0xB9: //LDA Absolute,Y - OK
                    {
                        cycles += 4;
                        InstructionName = "LDA M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0xBA: //TSX - SP -> X - OK
                    {
                        cycles += 2;
                        InstructionName = "TSX";
                        operandLength = 0;
                        break;
                    }

                case 0xBC:
                    {
                        cycles += 4;
                        InstructionName = "LDY M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xBD: //LDA - Absolute,X - OK
                    {
                        cycles += 4;
                        InstructionName = "LDA M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xBE:
                    {
                        cycles += 4;
                        InstructionName = "LDX M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0xC0:
                    {
                        cycles += 2;
                        InstructionName = "CPY #$";
                        operandLength = 1;
                        break;
                    }

                case 0xC1:
                    {
                        cycles += 6;
                        InstructionName = "CMP (ZP,X)";
                        operandLength = 1;
                        break;
                    }

                case 0xC4:
                    {
                        cycles += 3;
                        InstructionName = "CPY $";
                        operandLength = 1;
                        break;
                    }

                case 0xC5:
                    {
                        cycles += 3;
                        InstructionName = "CMP $";
                        operandLength = 1;
                        break;
                    }

                case 0xC6:
                    {
                        cycles += 5;
                        InstructionName = "DEC $";
                        operandLength = 1;
                        break;
                    }

                case 0xC8: //INY - increment Y - OK
                    {
                        cycles += 2;
                        InstructionName = "INY";
                        operandLength = 0;
                        break;
                    }

                case 0xC9:
                    {
                        cycles += 2;
                        InstructionName = "CMP #$";
                        operandLength = 1;
                        break;
                    }

                case 0xCA: //DEX - OK
                    {
                        cycles += 2;
                        InstructionName = "DEX";
                        operandLength = 0;
                        break;
                    }

                case 0xCC:
                    {
                        cycles += 4;
                        InstructionName = "CPY $";
                        operandLength = 2;
                        break;
                    }

                case 0xCD:
                    {
                        cycles += 4;
                        InstructionName = "CMP $";
                        operandLength = 2;
                        break;
                    }

                case 0xCE:
                    {
                        cycles += 6;
                        InstructionName = "DEC $";
                        operandLength = 2;
                        break;
                    }

                case 0xD0: //BNE
                    {
                        cycles += 2;
                        InstructionName = "BNE $";
                        operandLength = 1;
                        break;
                    }

                case 0xD1:
                    {
                        cycles += 5;
                        InstructionName = "CMP (ZP),Y";
                        operandLength = 1;
                        break;
                    }

                case 0xD5:
                    {
                        cycles += 4;
                        InstructionName = "CMP ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xD6:
                    {
                        cycles += 6;
                        InstructionName = "DEC ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xD8: //CLD - OK
                    {
                        cycles += 2;
                        InstructionName = "CLD";
                        operandLength = 0;
                        break;
                    }

                case 0xD9:
                    {
                        cycles += 4;
                        InstructionName = "CMP M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0xDD:
                    {
                        cycles += 4;
                        InstructionName = "CMP M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xDE:
                    {
                        cycles += 7;
                        InstructionName = "DEC M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xE0:
                    {
                        cycles += 2;
                        InstructionName = "CPX #$";
                        operandLength = 1;
                        break;
                    }

                case 0xE1:
                    {
                        cycles += 6;
                        InstructionName = "SBC (ZP,X) $";
                        operandLength = 1;
                        break;
                    }

                case 0xE4:
                    {
                        cycles += 3;
                        InstructionName = "CPX $";
                        operandLength = 1;
                        break;
                    }

                case 0xE5:
                    {
                        cycles += 3;
                        InstructionName = "SBC $";
                        operandLength = 1;
                        break;
                    }

                case 0xE6: //INC, increment zero-page memory address
                    {
                        cycles += 5;
                        InstructionName = "INC $";
                        operandLength = 1;
                        break;
                    }

                case 0xE8: //INX - increment X - OK
                    {
                        cycles += 2;
                        InstructionName = "INX";
                        operandLength = 0;
                        break;
                    }

                case 0xE9:
                    {
                        cycles += 2;
                        InstructionName = "SBC #$";
                        operandLength = 1;
                        break;
                    }

                case 0xEA: //NOP - OK
                    {
                        cycles += 2;
                        InstructionName = "NOP";
                        break;
                    }

                case 0xEC:
                    {
                        cycles += 4;
                        InstructionName = "CPX $";
                        operandLength = 2;
                        break;
                    }

                case 0xED:
                    {
                        cycles += 4;
                        InstructionName = "SBC $";
                        operandLength = 2;
                        break;
                    }

                case 0xEE:
                    {
                        cycles += 6;
                        InstructionName = "INC $";
                        operandLength = 2;
                        break;
                    }

                case 0xF0:
                    {
                        cycles += 2;
                        InstructionName = "BEQ $";
                        operandLength = 1;
                        break;
                    }

                case 0xF1:
                    {
                        cycles += 5;
                        InstructionName = "SBC (ZP),Y $";
                        operandLength = 1;
                        break;
                    }

                case 0xF5:
                    {
                        cycles += 4;
                        InstructionName = "SBC ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xF6:
                    {
                        cycles += 6;
                        InstructionName = "INC ZP,X $";
                        operandLength = 1;
                        break;
                    }

                case 0xF8: //SED - set decimal flag - OK
                    {
                        cycles += 2;
                        InstructionName = "SED";
                        operandLength = 0;
                        break;
                    }

                case 0xF9:
                    {
                        cycles += 4;
                        InstructionName = "SBC M,Y $";
                        operandLength = 2;
                        break;
                    }

                case 0xFD:
                    {
                        cycles += 4;
                        InstructionName = "SBC M,X $";
                        operandLength = 2;
                        break;
                    }

                case 0xFE:
                    {
                        cycles += 7;
                        InstructionName = "INC M,X $";
                        operandLength = 2;
                        break;
                    }
            }
            #endregion

            NextInstructionAddress++;

            for (int i = 0; i < operandLength; i++)
            {
                Operands.Add(debugMemory[NextInstructionAddress]);
                NextInstructionAddress++;
            }

        }

        public DisassembledOpcode ToDisassembledOpcode()
        {
            DisassembledOpcode dis = new DisassembledOpcode();
            dis.Address = Address;
            dis.InstructionName = InstructionName;
            dis.Opcode = Opcode;
            dis.Operands = Operands;
            return dis;
        }
    }
}
