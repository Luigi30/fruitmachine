using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace _6502EmulatorFrontend
{
    /// <summary>
    /// Interaction logic for SettingsWindow.xaml
    /// </summary>
    public partial class SettingsWindow : Window
    {
        public event RomPathsSavedEventHandler RomPathsSaved;
        public delegate void RomPathsSavedEventHandler(object sender, MainWindow.RomPathEventArgs e);

        public SettingsWindowViewModel swvm;

        public SettingsWindow()
        {
            swvm = new SettingsWindowViewModel();
            InitializeComponent();
            DataContext = swvm;
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            RomPathsSaved(this, new MainWindow.RomPathEventArgs(swvm.MonitorRomPath, swvm.BasicRomPath, swvm.CharacterRomPath));
            this.Close();
        }

        private void btnMonitorRom_Click(object sender, RoutedEventArgs e)
        {
            var ofd = new OpenFileDialog();
            ofd.Filter = "apple1.rom|apple1.rom|All Files (*.*)|*.*";
            ofd.FilterIndex = 1;

            bool? userClickedOK = ofd.ShowDialog();

            if (userClickedOK == true)
            {
                swvm.MonitorRomPath = ofd.FileName;
                tbMonitorRomPath.Text = ofd.FileName;
            }
        }

        private void btnBasicRom_Click(object sender, RoutedEventArgs e)
        {
            var ofd = new OpenFileDialog();
            ofd.Filter = "apple1basic.bin|apple1basic.bin|All Files (*.*)|*.*";
            ofd.FilterIndex = 1;

            bool? userClickedOK = ofd.ShowDialog();

            if (userClickedOK == true)
            {
                swvm.BasicRomPath = ofd.FileName;
                tbBasicRomPath.Text = ofd.FileName;
            }
        }

        private void btnCharacterRom_Click(object sender, RoutedEventArgs e)
        {
            var ofd = new OpenFileDialog();
            ofd.Filter = "apple1.vid|apple1.vid|All Files (*.*)|*.*";
            ofd.FilterIndex = 1;

            bool? userClickedOK = ofd.ShowDialog();

            if (userClickedOK == true)
            {
                swvm.CharacterRomPath = ofd.FileName;
                tbCharacterRomPath.Text = ofd.FileName;
            }
        }
    }
}
