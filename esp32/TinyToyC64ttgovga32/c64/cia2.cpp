/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "cia2.h"
#include "iecbus.h"
#include "vic.h"
#include <stdio.h>
#include "gbConfig.h"

void CIA2::Reset()
{
	ResetCIA();
}

void CIA2::Poke(Memory::addr_t addr, uint8_t val)
{
	ByteAt(addr) = val;
	
  //if ((addr >= 0xD800) && (addr <= 0xDBE7))
  //{
  // printf("CIA2:Poke addr:0x%x val:%x\n", addr, val);
  //}	

	// Try base CIA first
	if (PokeCIA(addr, val))
		return;

	switch(addr & 0xff) {
		case 0x00: { // data port a, vic memory
			Memory::addr_t vic_base = (3 - (val & 3)) * 0x4000;
			m_VIC.SetBaseAddress(vic_base);
			m_IECBus.OnChangeDataPortA(val & ~0x3);
			break;
		}
		default:
		    #ifdef use_lib_log_serial
			 printf("CIA2::Poke(): unhandled addr=0x%x val=%x\n", addr, val);
			#endif 
			break;
	}
}

uint8_t CIA2::Peek(Memory::addr_t addr)
{
	uint8_t val;
	if (PeekCIA(addr, val))
		return val;

	switch(addr & 0xff) {
		case 0x00: // data port a, vic memory
			return (ByteAt(addr) & 3) | m_IECBus.OnReadDataPortA();
	}
	return ByteAt(addr);
}

void CIA2::Update()
{	
	UpdateCIA();
}

/* vim:set ts=2 sw=2: */
