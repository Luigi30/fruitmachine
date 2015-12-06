using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace _6502EmulatorFrontend
{
    public class SettingsWindowViewModel
    {
        private string _monitorRomPath;
        private string _basicRomPath;
        private string _characterRomPath;

        public string MonitorRomPath
        {
            get { return _monitorRomPath; }
            set
            {
                if (value != _monitorRomPath)
                {
                    _monitorRomPath = value;
                    OnPropertyChanged("MonitorRomPath");
                }
            }
        }

        public string BasicRomPath
        {
            get { return _basicRomPath; }
            set
            {
                if (value != _basicRomPath)
                {
                    _basicRomPath = value;
                    OnPropertyChanged("BasicRomPath");
                }
            }
        }

        public string CharacterRomPath
        {
            get { return _characterRomPath; }
            set
            {
                if (value != _characterRomPath)
                {
                    _characterRomPath = value;
                    OnPropertyChanged("CharacterRomPath");
                }
            }
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
        #endregion
    }
}