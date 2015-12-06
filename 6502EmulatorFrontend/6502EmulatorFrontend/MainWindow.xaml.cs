using _6502EmulatorFrontend.cpu;
using _6502EmulatorFrontend.video;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace _6502EmulatorFrontend
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Thread M6502WorkerThread;
        MainWindowViewModel vm = new MainWindowViewModel();
        string monitorRomPath;
        string basicRomPath;
        string characterRomPath;
        SettingsWindow settingsWindow;
        byte[] videoRom;

        public MainWindow()
        {
            //Set up background thread
            M6502WorkerThread = new Thread(() =>
            {
                Thread.CurrentThread.IsBackground = true;
                vm.Processor.Run();
            });

            #region events

            #endregion

            //Set up viewmodel
            vm.Processor = new M6502();
            vm.DisplayGrid = new AppleDisplay();
            vm.DisassembledOpcodes = new ObservableCollection<Disassembly.DisassembledOpcode>();

            //Set up events
            vm.Processor.ProcessorStepCompleted += new M6502.ProcessorStepCompletedEventHandler(AfterProcessorStepCompleted);
            vm.Processor.ToggleCursor += new M6502.ToggleCursorEventHandler(vm.DisplayGrid.onToggleCursor);
            vm.Processor.UpdateDisplay += new M6502.UpdateDisplayEventHandler(vm.DisplayGrid.onUpdateDisplay);
            vm.Processor.ExecutionStopped += new M6502.ExecutionStoppedEventHandler(onExecutionStopped);
            TextCompositionManager.AddTextInputHandler(this, new TextCompositionEventHandler(OnTextComposition));

            basicRomPath = @"C:\apple\apple1basic.bin";
            monitorRomPath = @"C:\apple\apple1.rom";
            characterRomPath = @"C:\apple\apple1.vid";

            //Set up window
            InitializeComponent();
            binaryLoadedStatus.SetBinding(ContentProperty, new Binding("LoadSuccess"));
            DataContext = vm;
        }

        private void btnLoadBinary_Click(object sender, RoutedEventArgs e)
        {
            Interop.loadBinary(monitorRomPath, 0xFF00);
            Interop.loadBinary(basicRomPath, 0xE000);
            videoRom = File.ReadAllBytes(characterRomPath);
            decodeGraphics();

            Interop.resetProcessor();
            vm.Processor.UpdateProperties(Interop.getProcessorStatus());

            ushort length = 0xFFFE;
            IntPtr memoryValuesPtr = Interop.getMemoryRange(0x0000, length);
            byte[] result = new byte[length+1];
            Marshal.Copy(memoryValuesPtr, result, 0, length);

            Disassembly disassembly = new Disassembly(result);
            disassembly.Begin(0xFF00);

            while (disassembly.NextInstructionAddress < 0xFFFE)
            {
                vm.DisassembledOpcodes.Add(disassembly.ToDisassembledOpcode());
                disassembly.Next();
            }

            btnRun.IsEnabled = true;
            btnSingleStep.IsEnabled = true;

        }

        private void decodeGraphics()
        {
            for(int i=0; i < (videoRom.Length / 8); i++)
            {
                vm.DisplayGrid.Characters.CharacterList.Add(new CharacterBitmap(videoRom.Skip(i * 8).Take(8).ToArray()));
            }
        }

        private void disableReadoutControls()
        {
            BindingOperations.ClearBinding(txtAccumulator, TextBox.TextProperty);
            BindingOperations.ClearBinding(txtIndexX, TextBox.TextProperty);
            BindingOperations.ClearBinding(txtIndexY, TextBox.TextProperty);
            BindingOperations.ClearBinding(txtFlags, TextBox.TextProperty);
            BindingOperations.ClearBinding(txtProgramCounter, TextBox.TextProperty);
            BindingOperations.ClearBinding(txtStackPointer, TextBox.TextProperty);
            BindingOperations.ClearBinding(lbDisassembly, ListBox.ItemsSourceProperty);
        }

        private void enableReadoutControls()
        {
            BindingOperations.SetBinding(txtAccumulator, TextBox.TextProperty, new Binding("Processor.Accumulator") { StringFormat = "{0:X2}" });
            BindingOperations.SetBinding(txtIndexX, TextBox.TextProperty, new Binding("Processor.IndexX") { StringFormat = "{0:X2}" });
            BindingOperations.SetBinding(txtIndexY, TextBox.TextProperty, new Binding("Processor.IndexY") { StringFormat = "{0:X2}" });
            BindingOperations.SetBinding(txtFlags, TextBox.TextProperty, new Binding("Processor.Flags"));
            BindingOperations.SetBinding(txtProgramCounter, TextBox.TextProperty, new Binding("Processor.ProgramCounter") { StringFormat = "{0:X4}" });
            BindingOperations.SetBinding(txtStackPointer, TextBox.TextProperty, new Binding("Processor.StackPointer") { StringFormat = "{0:X2}" });
            BindingOperations.SetBinding(lbDisassembly, ListBox.ItemsSourceProperty, new Binding("DisassembledOpcodes"));
        }

        

        private void UpdateDisassemblySelection(ushort address)
        {
            //update disassembly
            for (int i = 0; i < lbDisassembly.Items.Count; i++)
            {
                var item = (Disassembly.DisassembledOpcode)lbDisassembly.Items[i];
                if (address == item.Address)
                {
                    lbDisassembly.Dispatcher.Invoke(new Action(() => { lbDisassembly.SelectedIndex = i; }));
                }
            }
        }

        private void btnRun_Click(object sender, RoutedEventArgs e)
        {
            btnLoadBinary.IsEnabled = false;
            btnRun.IsEnabled = false;
            btnSingleStep.IsEnabled = false;
            tbDebugConsole.IsEnabled = false;
            disableReadoutControls();
            M6502WorkerThread.Start();
        }

        private void btnBreak_Click(object sender, RoutedEventArgs e)
        {
            enableReadoutControls();
            btnRun.IsEnabled = true;
            btnSingleStep.IsEnabled = true;
        }

        private void tbDebugEntry_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                tbDebugConsole.Text += ">" + tbDebugEntry.Text + "\r\n";
                Debug.WriteLine(tbDebugEntry.Text);
                var split = tbDebugEntry.Text.Split();
                if (split[0].Equals("bpset"))
                {
                    var address = ushort.Parse(split[1], System.Globalization.NumberStyles.HexNumber);
                    vm.Processor.breakpointAddresses.Add(address);
                    tbDebugConsole.Text += String.Format("Breakpoint added: ${0}\r\n", address.ToString("X4"));
                    
                }
                else if (split[0].Equals("bpunset"))
                {
                    var address = ushort.Parse(split[1], System.Globalization.NumberStyles.HexNumber);
                    if (vm.Processor.breakpointAddresses.Contains(address))
                    {
                        vm.Processor.breakpointAddresses.Remove(address);
                        tbDebugConsole.Text += String.Format("Breakpoint removed: ${0}\r\n", address.ToString("X4"));
                    } else
                    {
                        tbDebugConsole.Text += String.Format("Breakpoint ${0} not found\r\n", address.ToString("X4"));
                    }
                }
                //TODO: bpdisable, bpenable
                tbDebugEntry.Clear();
            }
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            settingsWindow = new SettingsWindow();
            settingsWindow.RomPathsSaved += new SettingsWindow.RomPathsSavedEventHandler(OnRomPathsSaved);
            settingsWindow.swvm.BasicRomPath = basicRomPath;
            settingsWindow.swvm.MonitorRomPath = monitorRomPath;
            settingsWindow.swvm.CharacterRomPath = characterRomPath;
            settingsWindow.Show();
        }
    }
}