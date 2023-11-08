/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "peripheral.h"
#include "cpu6502.h"
#include "memory.h"

uint8_t& XPeripheral::ByteAt(Memory::addr_t addr)
{
 //if ((addr >= 0xD800) && (addr <= 0xDBE7))
 //{
 // printf("ByteAt addr:0x%x\n", addr);
 //}         
	return m_memory[addr];
}

void XPeripheral::SignalIRQ()
{
	m_cpu.SignalIRQ();
}

/* vim:set ts=2 sw=2: */
