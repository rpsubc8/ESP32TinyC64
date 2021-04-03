#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include <stdint.h>

class CPU6502;
class Memory;

//! \brief Interface class for C64 peripherals
class XPeripheral
{
public:
	XPeripheral(CPU6502& oCPU, Memory& oMemory) : m_cpu(oCPU), m_memory(oMemory) { }
	virtual ~XPeripheral() { }

	//! \brief Resets the peripheral
	virtual void Reset() = 0;

	/*! \brief Called when something gets written to the peripheral
	 *  \param addr Address to write
	 *  \param val Value to write
	 */
	virtual void Poke(uint16_t addr, uint8_t val) = 0;

	/*! \brief Called when something gets read from the peripheral
	 *  \param addr Address to read
	 *  \returns Value read
	 */
	virtual uint8_t Peek(uint16_t addr) = 0;

protected:
	//! \brief Directly access memory for use by the peripheral
	uint8_t& ByteAt(uint16_t addr);

	//! \brief Signals an IRQ to be triggered at the CPU
	void SignalIRQ();

	//! \brief CPU object we belong to
	CPU6502& m_cpu;

	//! \brief Memory object we belong to
	Memory& m_memory;
};

#endif /* __PERIPHERAL_H__ */
