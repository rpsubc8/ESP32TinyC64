#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <list>
#include <stdint.h>
#include "peripheral.h"

class Memory {
	friend class XPeripheral;
public:
	typedef uint16_t addr_t;

	Memory();
	~Memory();

	//! \brief Resets the memory state
	void Reset();

	/*! \brief Reads a single byte from memory
	 *  \param addr Address to read from
	 *  \returns Value read
	 */
	uint8_t ReadByte(addr_t addr);

	/*! \brief Reads a word from memory
	 *  \param addr Address to read from
	 *  \returns Value read
	 */
	uint16_t ReadWord(addr_t addr);

	/*! \brief Writes a single byte to memory
	 *  \param addr Address to write to
	 *  \param val Value to write
	 */
	void WriteByte(addr_t addr, uint8_t val);

	/*! \brief Writes a word to memory
	 *  \param addr Address to write to
	 *  \param val Value to write
	 */
	void WriteWord(addr_t addr, uint16_t val);

	/*! \brief Reads memory contents from a file
	 *  \param filename File to read
	 *  \param base Base address to store the contents
	 *  \param length Number of bytes to read
	 *  \returns true on success
	 */
	//JJ bool ReadFromFile(const char* filename, addr_t base, uint16_t length);
    void ReadFromFlash(const unsigned char* auxPtr, addr_t base, uint16_t length);

	/*! \brief Registers a connected peripheral
	 *  \param peripheral Peripheral to register
	 *  \param base Base address
	 *  \param length Number of bytes used by the peripheral
	 */
	void RegisterPeripheral(XPeripheral* peripheral, addr_t base, uint16_t length);

	//! \brief Stack high byte
	static const addr_t s_stack_base = 0x100;

	//! \brief VIC base address and length
	static const addr_t s_vic_base = 0xd000;
	static const uint16_t s_vic_length = 1024;

	//! \brief BASIC ROM address and length
	static const addr_t s_basic_base = 0xa000;
	static const addr_t s_basic_length = 8192;

	//! \brief Kernal ROM address and length
	static const addr_t s_kernal_base = 0xe000;
	static const addr_t s_kernal_length = 8192;

	//! \brief Charachter ROM address and length
	static const addr_t s_charrom_base = 0xd000;
	static const addr_t s_charrom_length = 4096;

	//! \brief SID base address and length
	static const addr_t s_sid_base = 0xd400;
	static const uint16_t s_sid_length = 1024;

	//! \brief Color RAM
	static const addr_t s_colorram_base = 0xd800;
	static const uint16_t s_colorram_length = 512;

	//! \brief CIA1
	static const addr_t s_cia1_base = 0xdc00;
	static const uint16_t s_cia1_length = 256;

	//! \brief CIA2
	static const addr_t s_cia2_base = 0xdd00;
	static const uint16_t s_cia2_length = 256;

	//! \brief NMI vector
	static const addr_t s_vector_nmi = 0xfffa;

	//! \brief Reset vector
	static const addr_t s_vector_reset = 0xfffc;

	//! \brief IRQ/BRK vector
	static const addr_t s_vector_irq = 0xfffe;
	
	void AssignPtrMemory(uint8_t *auxPtr);

protected:
	//! \brief Describes a peripheral connected to the memory bus
	class ConnectedPeripheral {
	public:
		/*! \brief Constructs a peripheral -> memory bus mapping
		 *  \param peripheral Peripheral to use
		 *  \param base Base address
		 *  \param length Number of bytes used by the peripheral
		 */
		ConnectedPeripheral(XPeripheral* peripheral, addr_t base, uint16_t length);

		//! \brief Resets the peripheral
		void Reset();

		/*! \brief Is a given address claimed by the peripheral?
		 *  \param addr Address to check
		 *  \returns true if the address is claimed
		 */
		bool IsAddressClaimed(addr_t addr) const;

		/*! \brief Writes a byte to the peripheral
		 *  \param addr Address to use
		 *  \param val Value to write
		 */
		void Poke(addr_t addr, uint8_t val);

		/*! \brief Reads a byte from the peripheral
		 *  \param addr Address to use
		 *  \returns Value read
		 */
		uint8_t Peek(addr_t addr);

	private:
		//! \brief Peripheral base address
		addr_t m_base;

		//! \brief Peripheral address length
		uint16_t m_length;

		//! \brief Peripheral itself
		XPeripheral* m_peripheral;
	};
	typedef std::list<ConnectedPeripheral> TConnectedPeripheralList;
	TConnectedPeripheralList m_peripheral;

	/*! \brief Is the given address readonly?
	 *  \param addr Address to check
	 *  \returns true if the memory is readonly
	 */
	bool IsReadOnly(addr_t addr) const;

	//! \brief Operator used by friends to directly access memory without tying it to peripherals
	uint8_t& operator[](addr_t addr);

private:
	//! \brief Memory size
	static const unsigned int m_MemorySize = 65536;

	//! \brief Memory contents
	uint8_t* m_Memory;
};

#endif /*__MEMORY_H__ */
