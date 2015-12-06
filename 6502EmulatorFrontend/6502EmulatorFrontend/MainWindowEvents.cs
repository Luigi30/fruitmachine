using _6502EmulatorFrontend.cpu;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace _6502EmulatorFrontend
{
    partial class MainWindow
    {
        public class RomPathEventArgs : EventArgs
        {
            public readonly string MonitorPath;
            public readonly string BasicPath;
            public readonly string CharacterPath;

            public RomPathEventArgs(string monitorPath, string basicPath, string characterPath)
            {
                MonitorPath = monitorPath;
                BasicPath = basicPath;
                CharacterPath = characterPath;
            }
        }

        private void OnRomPathsSaved(object sender, RomPathEventArgs e)
        {
            monitorRomPath = e.MonitorPath;
            basicRomPath = e.BasicPath;
            characterRomPath = e.CharacterPath;
        }

        private void OnTextComposition(object sender, TextCompositionEventArgs e)
        {
            Interop.putKeyInBuffer((byte)e.Text.ToUpper().ToCharArray()[0]);
        }

        private void AfterProcessorStepCompleted(M6502 sender, EventArgs e)
        {
            UpdateDisassemblySelection(sender.ProgramCounter);
        }

        private void btnSingleStep_Click(object sender, RoutedEventArgs e)
        {
            vm.Processor.DoProcessorStep(null, null);
        }

        //
        private void onExecutionStopped(object sender, EventArgs e)
        {
            Dispatcher.Invoke(new Action(() => { vm.DisassembledOpcodes.Clear(); }));

            ushort length = 0xFFFE;
            IntPtr memoryValuesPtr = Interop.getMemoryRange(0x0000, length);
            byte[] result = new byte[length + 1];
            Marshal.Copy(memoryValuesPtr, result, 0, length);

            Disassembly disassembly = new Disassembly(result);
            disassembly.Begin((ushort)(vm.Processor.ProgramCounter - 0x100));

            while (disassembly.NextInstructionAddress < (ushort)vm.Processor.ProgramCounter + 0x100)
            {
                Dispatcher.Invoke(new Action(() =>
                {
                    vm.DisassembledOpcodes.Add(disassembly.ToDisassembledOpcode());
                }));
                disassembly.Next();
            }

            Dispatcher.Invoke(new Action(() =>
            {
                UpdateDisassemblySelection(vm.Processor.ProgramCounter);
            }));

            lbDisassembly.Dispatcher.Invoke(new Action(() => { enableReadoutControls(); }));
            lbDisassembly.Dispatcher.Invoke(new Action(() => { tbDebugConsole.Text += "Breakpoint hit.\r\n"; }));
        }
    }
}
