#ifndef __CIA_H__
#define __CIA_H__

#include "memory.h"
#include "peripheral.h"

class HostIO;

class CIA : public XPeripheral {
public:
	CIA(CPU6502& oCPU, Memory& oMemory) : XPeripheral(oCPU, oMemory) { }

	virtual void ResetCIA();
	virtual bool PokeCIA(Memory::addr_t addr, uint8_t val);
	virtual bool PeekCIA(Memory::addr_t addr, uint8_t& val);

	void UpdateCIA();

protected:
	//! \brief Timer A value
	uint16_t m_timer_a;

	//! \brief Timer B value
	uint16_t m_timer_b;

	//! \brief Timer A current value
	uint16_t m_timerval_a;

	//! \brief Timer B current value
	uint16_t m_timerval_b;

	//! \brief Interrupt control register
	uint8_t m_icr;

	//! \brief Control register A
	uint8_t m_cra;

	//! \brief Control register B
	uint8_t m_crb;

	//! \brief IRQ's that have occured
	uint8_t m_read_irqs;

	static const uint8_t m_irq_flag = (1 << 7);
	static const uint8_t m_irq_flag1 = (1 << 4);
	static const uint8_t m_irq_serial = (1 << 3);
	static const uint8_t m_irq_tod = (1 << 2);
	static const uint8_t m_irq_timer_b = (1 << 1);
	static const uint8_t m_irq_timer_a = (1 << 0);
	
};

#endif /* __CIA_H__ */
