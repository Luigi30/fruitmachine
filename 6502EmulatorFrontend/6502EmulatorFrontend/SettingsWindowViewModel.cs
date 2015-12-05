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