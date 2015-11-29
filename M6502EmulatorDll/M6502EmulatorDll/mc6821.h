#pragma once

#include "stdafx.h"
#include <stdint.h>
#include <iostream>
#include "memory.h"

//Simulates a MC6821 PIA for use with the Apple I.

struct MC6821Status {
	uint8_t KBD;
	uint8_t KBDCR;
	uint8_t DSP;
	uint8_t DSPCR;
};

class MC6821 {
private:
	uint8_t kbd = 0x80;	//$D010
	uint8_t kbdcr = 0;	//$D011
	uint8_t dsp = 0;	//$D012
	uint8_t dspcr = 0;	//$D013

	uint8_t outputBuffer;
	uint8_t inputBuffer;

public:
	void executionLoop();
	MC6821Status getStatus();
	uint8_t getOutputBuffer();
};