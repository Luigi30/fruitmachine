#include "stdafx.h"
#include "state.h"

#include "CPU.h"
#include "memory.h"
#include "mc6821.h"
#include "s2513.h"

void EmulatorState::init() {
	config = APPLE_I;
	MemoryMap::Instance()->initialize();

	/*
	MemoryMap::Instance()->readModifiers.push_back(MemoryModifier(WideAddress{ 0xD0, 0x10 }, WideAddress{ 0xD0, 0x11 }, -0x80));
	MemoryMap::Instance()->writeModifiers.push_back(MemoryModifier(WideAddress{ 0xD0, 0x10 }, WideAddress{ 0xD0, 0x11 }, 0x80));
	MemoryMap::Instance()->writeModifiers.push_back(MemoryModifier(WideAddress{ 0xD0, 0x12 }, WideAddress{ 0xD0, 0x12 }, 0x80));
	*/
}

EmulatorState* EmulatorState::m_pInstance = NULL;

EmulatorState* EmulatorState::Instance() {
	if (!m_pInstance)
		m_pInstance = new EmulatorState();

	return m_pInstance;
}

EmulatorState::COMPUTER_CONFIG EmulatorState::getConfigurationType() {
	return config;
}