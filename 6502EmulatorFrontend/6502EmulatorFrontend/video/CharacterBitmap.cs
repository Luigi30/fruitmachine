using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace _6502EmulatorFrontend.video
{
    public class CharacterBitmap
    {
        public byte[] Character = new byte[8];
        public WriteableBitmap CharacterImage;

        public CharacterBitmap(byte[] data)
        {
            //Load the data into the byte array
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    Character[j] = data[i];
                    i++;
                }
            }

            //Now generate a bitmap somehow
            CharacterImage = new WriteableBitmap(8, 8, 96, 96, PixelFormats.Bgr32, null);
            CharacterImage.Lock();
            for(int i = 0; i < 8; i++)
            {
                if((Character[i] & 0x10) == 0x10)
                {
                    CharacterImage.SetPixel(3, i, Colors.White);
                } else
                {
                    CharacterImage.SetPixel(3, i, Colors.Black);
                }
                if ((Character[i] & 0x08) == 0x08)
                {
                    CharacterImage.SetPixel(4, i, Colors.White);
                }
                else
                {
                    CharacterImage.SetPixel(4, i, Colors.Black);
                }
                if ((Character[i] & 0x04) == 0x04)
                {
                    CharacterImage.SetPixel(5, i, Colors.White);
                }
                else
                {
                    CharacterImage.SetPixel(5, i, Colors.Black);
                }
                if ((Character[i] & 0x02) == 0x02)
                {
                    CharacterImage.SetPixel(6, i, Colors.White);
                }
                else
                {
                    CharacterImage.SetPixel(6, i, Colors.Black);
                }
                if ((Character[i] & 0x01) == 0x01)
                {
                    CharacterImage.SetPixel(7, i, Colors.White);
                }
                else
                {
                    CharacterImage.SetPixel(7, i, Colors.Black);
                }
            }
            CharacterImage.Unlock();
        }

        public static CharacterBitmap BlankBitmap { get
            {

                byte[] blankCharacter = new byte[64];
                for(int i = 0; i < blankCharacter.Length; i++)
                {
                    blankCharacter[i] = 0x00;
                }
                CharacterBitmap blank = new CharacterBitmap(blankCharacter);
                return blank;
            }
        }

        public void drawToDebugConsole()
        {
            for (int i = 0; i < 8; i++)
            {
                StringBuilder graphic = new StringBuilder();
                graphic.Append("   ");
                if ((Character[i] & 0x10) == 0x10)
                {
                    graphic.Append("X");
                }
                else {
                    graphic.Append(" ");
                }
                if ((Character[i] & 0x08) == 0x08)
                {
                    graphic.Append("X");
                }
                else {
                    graphic.Append(" ");
                }
                if ((Character[i] & 0x04) == 0x04)
                {
                    graphic.Append("X");
                }
                else {
                    graphic.Append(" ");
                }
                if ((Character[i] & 0x02) == 0x02)
                {
                    graphic.Append("X");
                }
                else {
                    graphic.Append(" ");
                }
                if ((Character[i] & 0x01) == 0x01)
                {
                    graphic.Append("X");
                }
                else {
                    graphic.Append(" ");
                }
                Console.WriteLine(graphic);
            }
        }
    }
}
