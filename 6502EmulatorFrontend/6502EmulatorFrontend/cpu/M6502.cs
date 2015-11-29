using _6502EmulatorFrontend.cpu;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Threading;

namespace _6502EmulatorFrontend
{
    public class CharacterInDisplayBufferEventArgs : EventArgs
    {
        private readonly byte _character;

        public CharacterInDisplayBufferEventArgs(byte character)
        {
            _character = character;
        }

        public byte Character { get { return _character; } }
    }

    public partial class M6502 : INotifyPropertyChanged
    {
        #region events
        public event ProcessorStepCompletedEventHandler ProcessorStepCompleted;
        public delegate void ProcessorStepCompletedEventHandler(M6502 sender, EventArgs e);

        public event ToggleCursorEventHandler ToggleCursor;
        public delegate void ToggleCursorEventHandler(M6502 sender, EventArgs e);

        public event ProcessorClockTickEventHandler ProcessorClockTick;
        public delegate void ProcessorClockTickEventHandler(object sender, EventArgs e);

        public event UpdateDisplayEventHandler UpdateDisplay;
        public delegate void UpdateDisplayEventHandler(object sender, EventArgs e);

        public event ExecutionStoppedEventHandler ExecutionStopped;
        public delegate void ExecutionStoppedEventHandler(object sender, EventArgs e);

        // Invoke the Changed event; called whenever list changes:
        protected virtual void OnProcessorStepCompleted(EventArgs e)
        {
            if (ProcessorStepCompleted != null)
                ProcessorStepCompleted(this, e);
        }

        protected virtual void OnProcessorClockTick(EventArgs e)
        {
            if (ProcessorClockTick != null)
                ProcessorClockTick(this, e);
        }
        protected virtual void OnToggleCursor(EventArgs e)
        {
            if (ToggleCursor != null)
                ToggleCursor(this, e);
        }
        protected virtual void OnExecutionStopped(EventArgs e)
        {
            if (ExecutionStopped != null)
                ExecutionStopped(this, e);
        }
        #endregion

        M6502Model model = new M6502Model();
        public List<ushort> breakpointAddresses = new List<ushort>();
        bool shouldRun = true;

        public void UpdateProperties(Interop.InteropProcessorStatus newStatus)
        {
            Accumulator = newStatus.accumulator;
            IndexX = newStatus.index_x;
            IndexY = newStatus.index_y;
            StackPointer = newStatus.stack_pointer;
            ProgramCounter = newStatus.program_counter;
            Cycles = newStatus.cycles;

            model.FLAG_BREAKPOINT = newStatus.FLAG_BREAKPOINT == 0x01 ? true : false;
            model.FLAG_CARRY = newStatus.FLAG_CARRY == 0x01 ? true : false;
            model.FLAG_DECIMAL = newStatus.FLAG_DECIMAL == 0x01 ? true : false;
            model.FLAG_INTERRUPT = newStatus.FLAG_INTERRUPT == 0x01 ? true : false;
            model.FLAG_OVERFLOW = newStatus.FLAG_OVERFLOW == 0x01 ? true : false;
            model.FLAG_SIGN = newStatus.FLAG_SIGN == 0x01 ? true : false;
            model.FLAG_ZERO = newStatus.FLAG_ZERO == 0x01 ? true : false;
            OnPropertyChanged("Flags");
        }

        public void UpdateModelDirectly(Interop.InteropProcessorStatus newStatus)
        {
            model.accumulator = newStatus.accumulator;
            model.indexX = newStatus.index_x;
            model.indexY = newStatus.index_y;
            model.stackPointer = newStatus.stack_pointer;
            model.programCounter = newStatus.program_counter;
            model.lastOpcode = newStatus.lastOpcodeAsString;
            model.cycles = newStatus.cycles;

            model.FLAG_BREAKPOINT = newStatus.FLAG_BREAKPOINT == 0x01 ? true : false;
            model.FLAG_CARRY = newStatus.FLAG_CARRY == 0x01 ? true : false;
            model.FLAG_DECIMAL = newStatus.FLAG_DECIMAL == 0x01 ? true : false;
            model.FLAG_INTERRUPT = newStatus.FLAG_INTERRUPT == 0x01 ? true : false;
            model.FLAG_OVERFLOW = newStatus.FLAG_OVERFLOW == 0x01 ? true : false;
            model.FLAG_SIGN = newStatus.FLAG_SIGN == 0x01 ? true : false;
            model.FLAG_ZERO = newStatus.FLAG_ZERO == 0x01 ? true : false;
        }

        public void DoProcessorStep(object sender, EventArgs e)
        {
            Interop.doSingleStep();
            Interop.MC6821Status status = Interop.getMC6821Status();
            UpdateProperties(Interop.getProcessorStatus());
            ProcessorStepCompleted(this, null);
            UpdateDisplay(this, null);
        }

        public void DoProcessorStepNoFancyGraphics(object sender, EventArgs e)
        {
            if (breakpointAddresses.Contains(model.programCounter))
            {
                shouldRun = false;
            }
            Interop.doSingleStep();
            Interop.MC6821Status status = Interop.getMC6821Status();
            UpdateModelDirectly(Interop.getProcessorStatus());
            UpdateDisplay(this, null);
        }

        async public void DoProcessorStepInBackground()
        {
            await Task.Run(() =>
            {
                Interop.doSingleStep();
                Interop.MC6821Status status = Interop.getMC6821Status();
                UpdateProperties(Interop.getProcessorStatus());
                UpdateDisplay(this, null);
            });
        }

        public void Run()
        {
            int frameCounter = 0;

            while (shouldRun)
            {
                if(frameCounter % 30 == 0 && frameCounter != 0) 
                {
                    ToggleCursor(this, null);
                    frameCounter = 0;
                }
                ExecuteFrame();
                frameCounter++;
            }

            //Suspend execution.
            OnPropertyChanged("Flags");
            ExecutionStopped(this, null);
            Thread.Sleep(Timeout.Infinite);
        }

        public void ExecuteFrame()
        {
            var stopwatch = new Stopwatch();
            stopwatch.Start();

            while (model.cycles < 17050) //17050 cycles per frame, 1MHz @ 60fps
            {
                if (!shouldRun)
                {
                    return;
                }
                else
                {
                    DoProcessorStepNoFancyGraphics(null, null);
                }
            }
 
            while (stopwatch.Elapsed < TimeSpan.FromMilliseconds(16))
            {

            };
            Interop.resetCycleCounter();
            model.cycles = 0;
        }

        #region INotifyPropertyChanged Implementation
        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string name)
        {
            var handler = System.Threading.Interlocked.CompareExchange(ref PropertyChanged, null, null);
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }

        public Boolean LoadSuccess
        {
            get { return model.loadSuccess; }
            set
            {
                if (value != model.loadSuccess)
                {
                    model.loadSuccess = value;
                    OnPropertyChanged("LoadSuccess");
                }
            }
        }

        public ushort ProgramCounter
        {
            get { return model.programCounter; }
            set
            {
                if (value != model.programCounter)
                {
                    model.programCounter = value;
                    OnPropertyChanged("ProgramCounter");
                }
            }
        }

        public byte Accumulator
        {
            get { return model.accumulator; }
            set
            {
                if (value != model.accumulator)
                {
                    model.accumulator = value;
                    OnPropertyChanged("Accumulator");
                }
            }
        }

        public byte IndexX
        {
            get { return model.indexX; }
            set
            {
                if (value != model.indexX)
                {
                    model.indexX = value;
                    OnPropertyChanged("IndexX");
                }
            }
        }

        public byte IndexY
        {
            get { return model.indexY; }
            set
            {
                if (value != model.indexY)
                {
                    model.indexY = value;
                    OnPropertyChanged("IndexY");
                }
            }
        }

        public byte StackPointer
        {
            get { return model.stackPointer; }
            set
            {
                if (value != model.stackPointer)
                {
                    model.stackPointer = value;
                    OnPropertyChanged("StackPointer");
                }
            }
        }

        public String Flags
        {
            get
            {
                var sb = new StringBuilder();
                if (model.FLAG_SIGN)
                {
                    sb.Append("N");
                } else
                {
                    sb.Append("-");
                }
                if (model.FLAG_OVERFLOW)
                {
                    sb.Append("V");
                }
                sb.Append("-"); //0x20 is not connected
                if (model.FLAG_BREAKPOINT)
                {
                    sb.Append("B");
                }
                else
                {
                    sb.Append("-");
                }
                if (model.FLAG_DECIMAL)
                {
                    sb.Append("D");
                }
                else
                {
                    sb.Append("-");
                }
                if (model.FLAG_INTERRUPT)
                {
                    sb.Append("I");
                }
                else
                {
                    sb.Append("-");
                }
                if (model.FLAG_ZERO)
                {
                    sb.Append("Z");
                }
                else
                {
                    sb.Append("-");
                }
                if (model.FLAG_CARRY)
                {
                    sb.Append("C");
                }
                else
                {
                    sb.Append("-");
                }
                return sb.ToString();
            }
            set { OnPropertyChanged("Flags"); }
        }


        public ushort Cycles
        {
            get { return model.cycles; }
            set
            {
                if (value != model.cycles)
                {
                    model.cycles = value;
                    OnPropertyChanged("Cycles");
                }
            }
        }
        #endregion
    }
}
