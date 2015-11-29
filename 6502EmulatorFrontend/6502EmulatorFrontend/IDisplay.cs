using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace _6502EmulatorFrontend
{
    interface IDisplay
    {
        void onUpdateDisplay(object sender, EventArgs e);
        WriteableBitmap DisplayCanvas { get; set; }
    }
}
