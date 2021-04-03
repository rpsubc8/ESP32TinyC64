#ifndef __CIA1_H__
#define __CIA1_H__

#include "memory.h"
#include "cia.h"

class HostIO;

class CIA1 : public CIA {
public:
	CIA1(CPU6502& oCPU, Memory& oMemory, HostIO& oHostIO) : CIA(oCPU, oMemory), m_hostio(oHostIO) { }

	virtual void Reset();
	virtual void Poke(Memory::addr_t addr, uint8_t val);
	virtual uint8_t Peek(Memory::addr_t addr);

	void Update();
	void TestTeclado(unsigned char aKey,unsigned char aPress);

protected:
	//! \brief Host I/O provider
	HostIO& m_hostio;

	//! \brief Currently selected keyboard columns
	uint8_t m_kbd_cur_columns;

	//! \brief Keyboard matrix
	uint8_t m_kbd_matrix[8];

	//! \brief Callback function for the host I/O
	static void KeyCallback(void* ptr, int key, bool pressed);
};

#endif /* __CIA1_H__ */
