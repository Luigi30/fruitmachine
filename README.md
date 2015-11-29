# fruitmachine
This is the start of an Apple I emulator. Nothing really works yet beyond the CPU.

Things that work:
* MOS 6502 core
* Dumb terminal-style display output

Things that don't work:
* The debugger
* Cassette interface
* Most of the UI

Known issues:
* The blinking cursor will hang around if you hit carriage return while it's being displayed
* Clicking Break and then Run will crash the emulator because I don't know how C# threading works
* The display will sometimes lose input focus, click away from the window and back in to fix that
* The cycle counts aren't completely accurate (a cycle isn't added for zero-page operations that wrap)
* Many more

The ROM paths are hardcoded for the moment so the emulator expects these files to exist:

C:\apple\apple1.rom - Monitor ROM, 256B
C:\apple\apple1.vid - Character ROM, 512B
C:\apple\apple1basic.bin - Integer BASIC, available from http://www.pagetable.com/?p=32

To load Integer BASIC from the monitor, type E000R <enter>. The prompt will switch from \ to >, indicating that you are now in BASIC mode.

Most Apple I software on the internet is available in audiocassette format and I haven't written a cassette reader yet. If you have a debug copy of MESS, you can load a file into MESS' Apple I emulator, dump the program memory, and add it to the btnLoadBinary click event with Interop.loadBinary(path, address).

--

M6502EmulatorDLL compiles to a DLL. Add a reference to this DLL in 6502EmulatorFrontend to compile.