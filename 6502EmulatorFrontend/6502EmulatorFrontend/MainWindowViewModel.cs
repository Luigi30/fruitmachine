using _6502EmulatorFrontend.video;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace _6502EmulatorFrontend
{
    public class MainWindowViewModel : INotifyPropertyChanged
    {
        private M6502 _processor;
        private AppleDisplay _displayGrid;
        private ObservableCollection<cpu.Disassembly.DisassembledOpcode> _disassembledOpcodes;

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

        public AppleDisplay DisplayGrid
        {
            get { return _displayGrid; }
            set
            {
                if (value != _displayGrid)
                {
                    _displayGrid = value;
                    OnPropertyChanged("DisplayGrid");
                }
            }
        }

        public ObservableCollection<cpu.Disassembly.DisassembledOpcode> DisassembledOpcodes
        {
            get { return _disassembledOpcodes; }
            set
            {
                if(value != _disassembledOpcodes)
                {
                    _disassembledOpcodes = value;
                    OnPropertyChanged("DisassembledOpcodes");
                }
            }
        }

        public M6502 Processor
        {
            get { return _processor; }
            set
            {
                if (value != _processor)
                {
                    _processor = value;
                    OnPropertyChanged("Processor");
                }
            }
        }
        #endregion
    }
}
