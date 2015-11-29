#pragma once
#include "stdafx.h"
#include "CPU.h"
#include "mc6821.h"
#include "s2513.h"

class EmulatorState {
public:
	enum COMPUTER_CONFIG { APPLE_I };
	static EmulatorState* Instance();
	CPU processor = CPU();
	MC6821 mc6821 = MC6821();
	S2513 s2513 = S2513();
	
	void init();
	COMPUTER_CONFIG EmulatorState::getConfigurationType();

private:
	EmulatorState() {};
	EmulatorState(EmulatorState const&) {};
	EmulatorState& operator=(EmulatorState const&) {};
	static EmulatorState* m_pInstance;
	COMPUTER_CONFIG config = APPLE_I;
};