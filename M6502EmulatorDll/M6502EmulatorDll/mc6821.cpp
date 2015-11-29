#include "stdafx.h"
#include "mc6821.h"
#include "state.h"

void MC6821::executionLoop() {
	//update the registers
	kbd = MemoryMap::Instance()->read_byte(WideAddress{ 0xD0, 0x10 }, false);
	kbdcr = MemoryMap::Instance()->read_byte(WideAddress{ 0xD0, 0x11 }, false);
	dsp = MemoryMap::Instance()->read_byte(WideAddress{ 0xD0, 0x12 }, false);
	dspcr = MemoryMap::Instance()->read_byte(WideAddress{ 0xD0, 0x13 }, false);

	//Character is waiting for the display
	if ((dsp & 0x80) == 0x80) {
		dsp = dsp - 0x80;
		if (dsp <= 0x7f) {
			if (dsp == 0x0d) {
					WCHAR output[2];
					wsprintfW(output, L"%c", dsp);
					outputBuffer = dsp;
					OutputDebugString(output);
					OutputDebugStringW(L"\r\n");
			}
			else if (dsp != 0x7f) {
				WCHAR output[2];
				wsprintfW(output, L"%c", dsp);
				outputBuffer = dsp;
				OutputDebugString(output);
			}
		}
		MemoryMap::Instance()->write_byte(WideAddress(0xD0, 0x12), dsp, false);
	}

}

uint8_t MC6821::getOutputBuffer() {
	uint8_t character = outputBuffer;
	outputBuffer = 0x00;
	return character;
}

MC6821Status MC6821::getStatus() {
	MC6821Status status;

	status.DSP = dsp;
	status.DSPCR = dspcr;
	status.KBD = kbd;
	status.KBDCR = kbdcr;

	return status;
}