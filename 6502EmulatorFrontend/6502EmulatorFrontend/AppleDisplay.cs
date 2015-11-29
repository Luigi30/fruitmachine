using _6502EmulatorFrontend.video;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace _6502EmulatorFrontend
{
    public class AppleDisplay : INotifyPropertyChanged, IDisplay
    {
        private CharacterSet _characters;
        private readonly SynchronizationContext _syncContext;

        public List<DisplayRow> DisplayRows;
        public WriteableBitmap _displayCanvas;
        public WriteableBitmap DisplayCanvas {
            get { return _displayCanvas; }
            set
            {
                if (value != _displayCanvas)
                {
                    _displayCanvas = value;
                    OnPropertyChanged("DisplayCanvas");
                }
            }
        }
        int cursorPositionX;
        int cursorPositionY;
        bool cursorEnabled;

        public AppleDisplay()
        {
            Characters = new CharacterSet();
            DisplayCanvas = new WriteableBitmap(320, 192, 96, 96, PixelFormats.Bgr32, null);
            DisplayRows = new List<DisplayRow>();
            _syncContext = SynchronizationContext.Current;
            cursorPositionX = 0;
            cursorPositionY = 0;
            cursorEnabled = false;
            for(int i = 0; i < 24; i++)
            {
                DisplayRows.Insert(i, new DisplayRow());
            }
        }

        public void PutCharacter(CharacterBitmap character)
        {
            SetTile(cursorPositionY, cursorPositionX, character); //(row,column)
            if(cursorPositionX == 39)
            {
                CarriageReturn();
            } else
            {
                cursorPositionX++;
            }
        }

        public void CarriageReturn()
        {
            _syncContext.Post(o => SetTile(cursorPositionY, cursorPositionX, Characters.CharacterList[0x20]), null);

            if (cursorEnabled)
            {

                cursorPositionX = 0;
                cursorPositionY++;
                if (cursorPositionY == 24)
                {
                    cursorPositionY = 23;
                    DisplayRows.RemoveAt(0);
                    DisplayRows.Insert(23, new DisplayRow());
                }
            }
            else
            {
                cursorPositionX = 0;
                cursorPositionY++;
                if (cursorPositionY == 24)
                {
                    cursorPositionY = 23;
                    DisplayRows.RemoveAt(0);
                    DisplayRows.Insert(23, new DisplayRow());
                }
            }
        }

        public void onToggleCursor(M6502 sender, EventArgs e)
        {
            if (!cursorEnabled)
            {
                _syncContext.Post(o => SetTile(cursorPositionY, cursorPositionX, Characters.CharacterList[0x00]), null);
                cursorEnabled = true;
            }
            else
            {
                _syncContext.Post(o => SetTile(cursorPositionY, cursorPositionX, Characters.CharacterList[0x20]), null);
                cursorEnabled = false;
            }

        }

        public void SetTile(int row, int column, CharacterBitmap character)
        {
            DisplayRows[row].Characters[column] = character;
            DrawScreen();
        }

        private void DrawScreen()
        {
            for(int row = 0; row < 24; row++)
            {
                for(int column = 0; column < 40; column++)
                {
                    var tile = DisplayRows[row].Characters[column].CharacterImage;

                    int offsetX = (column * 8);
                    int offsetY = (row * 8);
                    int stride = tile.PixelWidth * (tile.Format.BitsPerPixel / 8);
                    byte[] data = new byte[stride * tile.PixelHeight];
                    tile.CopyPixels(data, stride, 0);

                    DisplayCanvas.WritePixels(new Int32Rect(0, 0, 8, 8), data, stride, offsetX, offsetY);
                }
            }

        }

        public void onCharacterInDisplayBuffer(AppleDisplay sender, CharacterInDisplayBufferEventArgs e)
        {
            byte key = (byte)e.Character;

            if (e.Character == 0x0d)
            {
                _syncContext.Post(o => CarriageReturn(), null);
                return;
            }

            if ((key & 0x60) == 0x60)
            {
                key = (byte)(key & ~0x60);
            }
            if((key & 0x40) == 0x40)
            {
                key = (byte)(key & ~0x40);
            }

            _syncContext.Post(o => PutCharacter(Characters.CharacterList[key]), null);
        }

        public void onUpdateDisplay(object sender, EventArgs e)
        {
            byte outputBuffer = Interop.getOutputBuffer();
            if (outputBuffer != 0x00)
            {
                onCharacterInDisplayBuffer(this, new CharacterInDisplayBufferEventArgs(outputBuffer));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string name)
        {
            var handler = System.Threading.Interlocked.CompareExchange(ref PropertyChanged, null, null);
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }

        public CharacterSet Characters
        {
            get { return _characters; }
            set
            {
                if (value != _characters)
                {
                    _characters = value;
                    OnPropertyChanged("Characters");
                }
            }
        }
    }

    public class DisplayRow
    {
        public List<CharacterBitmap> Characters = new List<CharacterBitmap>();

        public DisplayRow()
        {
            for (int j = 0; j < 40; j++)
            {
                Characters.Insert(j, CharacterBitmap.BlankBitmap);
            }
        }
    }
}
