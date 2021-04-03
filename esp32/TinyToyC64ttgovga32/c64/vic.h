#ifndef __VIC_H__
#define __VIC_H__

#include "memory.h"
#include "peripheral.h"

void jj_video_Update(void);

class HostIO;

class VIC : public XPeripheral {
public:
	VIC(CPU6502& oCPU, Memory& oMemory, HostIO& oHostIO);
	~VIC();

	virtual void Reset();
	virtual void Poke(Memory::addr_t addr, uint8_t val);
	virtual uint8_t Peek(Memory::addr_t addr);

	void SetBaseAddress(Memory::addr_t addr) {
		m_addr_base = addr;
	}

	static const int s_video_width = 320;
	static const int s_video_height = 200;

	void Update();
	void AssignPtrRegs(uint8_t *auxPtr);

protected:
	//! \brief Screen memory base
	Memory::addr_t m_screen_base;

	//! \brief Character memory base
	Memory::addr_t m_char_base;

	//! \brief Base address
	Memory::addr_t m_addr_base;

	//! \brief Host I/O provider
	HostIO& m_hostio;

	//! \brief VIC registers
	uint8_t* m_vic_regs;
};

void PreparaColorVGA();

#endif /* __VIC_H__ */
