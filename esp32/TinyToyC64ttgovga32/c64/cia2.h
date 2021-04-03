#ifndef __CIA2_H__
#define __CIA2_H__

#include "memory.h"
#include "cia.h"

class VIC;
class IECBus;

class CIA2 : public CIA {
public:
	CIA2(CPU6502& oCPU, Memory& oMemory, VIC& oVIC, IECBus& oIECBus)
	 : CIA(oCPU, oMemory), m_VIC(oVIC), m_IECBus(oIECBus) { }

	virtual void Reset();
	virtual void Poke(Memory::addr_t addr, uint8_t val);
	virtual uint8_t Peek(Memory::addr_t addr);
	virtual void Update();

protected:
	VIC& m_VIC;
	IECBus& m_IECBus;
};

#endif /* __CIA2_H__ */
