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
            
        }
    }
}
