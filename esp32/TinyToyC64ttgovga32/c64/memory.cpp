/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gb_globals.h"
#include "gbConfig.h"

Memory::ConnectedPeripheral::ConnectedPeripheral(XPeripheral* peripheral, addr_t base, uint16_t length)
	: m_base(base), m_length(length), m_peripheral(peripheral)
{
}

inline bool Memory::ConnectedPeripheral::IsAddressClaimed(addr_t addr) const
{
	return addr >= m_base && addr < m_base + m_length;
}

inline void Memory::ConnectedPeripheral::Poke(addr_t addr, uint8_t val)
{
 //if ((addr >= 0xD800) && (addr <= 0xDBE7))
 //{
 // printf("Pheripheral Poke addr:0x%x val:%x\n", addr, val);
 //}     
 m_peripheral->Poke(addr, val);
}

inline uint8_t Memory::ConnectedPeripheral::Peek(addr_t addr)
{
	return m_peripheral->Peek(addr);
}

inline void Memory::ConnectedPeripheral::Reset()
{
	m_peripheral->Reset();
}

Memory::Memory()
{    
	//JJ m_Memory = new uint8_t[m_MemorySize];
	//m_Memory = (uint8_t *) malloc(65536);
	//memset(m_Memory, 0, m_MemorySize); /* XXX Should be random! */	
}

Memory::~Memory()
{
	delete[] m_Memory;
}

//********************************************
void Memory::AssignPtrMemory(uint8_t *auxPtr)
{
 m_Memory = auxPtr;
 memset(m_Memory, 0, m_MemorySize);
}

void Memory::RegisterPeripheral(XPeripheral* peripheral, addr_t base, uint16_t length)
{
	m_peripheral.push_back(ConnectedPeripheral(peripheral, base, length));
}

inline uint8_t Memory::ReadByte(addr_t addr)
{
 #ifdef usb_lib_fast_MemoryReadByte
  if (addr >= Memory::s_vic_base && addr < (Memory::s_vic_base + Memory::s_vic_length))
    return gb_ptr_oVIC->Peek(addr);
  if (addr >= Memory::s_cia1_base && addr < (Memory::s_cia1_base + Memory::s_cia1_length))
    return gb_ptr_oCIA1->Peek(addr);
  if (addr >= Memory::s_cia2_base && addr < (Memory::s_cia2_base + Memory::s_cia2_length))
    return gp_ptr_oCIA2->Peek(addr);        
  return m_Memory[addr];
 #else
  for (TConnectedPeripheralList::iterator it = m_peripheral.begin(); it != m_peripheral.end(); it++) {
   if (it->IsAddressClaimed(addr))
	return it->Peek(addr);
  }
  //No peripherals here, must be plain memory
  return m_Memory[addr];	
 #endif
}

void Memory::WriteByte(addr_t addr, uint8_t val)
{
 if ((addr >= 0xD800) && (addr <= 0xDBE7))
 {
  //printf("WriteByte addr:0x%x val:%x\n", addr, val);
  gb_memory[addr]= (val & 0x0F); //Video color text mode standar
 }
 else
 {
  switch (addr)
  {
   case 0xD011: gb_memory[addr]= val; break; //ECM BMM
   case 0xD016: gb_memory[addr]= val; break; //MCM
   case 0xD020: gb_memory[addr]= (val & 0x0F); break; //color borde
   case 0xD021: gb_memory[addr]= (val & 0x0F); break; //color background
   //printf("WriteByte Backcolor addr:0x%x val:%x\n", addr, val);
   //gb_memory[addr]= (val & 0x0F); //backcolor y borde
  }
 }

 #ifdef usb_lib_fast_MemoryWriteByte
  if (addr >= Memory::s_vic_base && addr < (Memory::s_vic_base + Memory::s_vic_length))
  {
   gb_ptr_oVIC->Poke(addr,val);
   return;
  }
  if (addr >= Memory::s_cia1_base && addr < (Memory::s_cia1_base + Memory::s_cia1_length))
  {
   gb_ptr_oCIA1->Poke(addr,val);
   return;
  }
  if (addr >= Memory::s_cia2_base && addr < (Memory::s_cia2_base + Memory::s_cia2_length))
  {
   gp_ptr_oCIA2->Poke(addr,val);  
   return;
  }
 #else
  for (TConnectedPeripheralList::iterator it = m_peripheral.begin(); it != m_peripheral.end(); it++)
  {
   if (!it->IsAddressClaimed(addr))
    continue;
   it->Poke(addr, val);
    return;
  }
 #endif 

 //No peripherals here, must be plain memory
 if (!IsReadOnly(addr)) {
  m_Memory[addr] = val;
 }
}

uint16_t Memory::ReadWord(addr_t addr)
{
	return ReadByte(addr) | (uint16_t)ReadByte(addr + 1) << 8;
}

inline void Memory::WriteWord(addr_t addr, uint16_t val)
{
	WriteByte(addr, val & 0xff);
	WriteByte(addr + 1, val >> 8);
}

//JJ bool Memory::ReadFromFile(const char* filename, addr_t base, uint16_t length)
//JJ {
//JJ 	FILE* f = fopen(filename, "rb");
//JJ 	if (f == NULL)
//JJ 		return false;
//JJ 
//JJ 	bool ok = fread((void*)&m_Memory[base], length, 1, f) > 0;
//JJ 
//JJ 	fclose(f);
//JJ 	return ok;
//JJ }

//*****************************************************************************
void Memory::ReadFromFlash(const unsigned char* auxPtr, addr_t base, uint16_t length)
{
 memcpy(&m_Memory[base],auxPtr,length);
}

uint8_t& Memory::operator[](addr_t addr)
{
	return *(uint8_t*)&m_Memory[addr];
}

void Memory::Reset()
{
	for (TConnectedPeripheralList::iterator it = m_peripheral.begin(); it != m_peripheral.end(); it++) {
		it->Reset();
	}
}

bool Memory::IsReadOnly(addr_t addr) const
{
	/* XXX We must check whether these are available! */
	if (addr >= s_basic_base && addr < s_basic_base + s_basic_length)
		return true;
	if (addr >= s_kernal_base && addr < s_kernal_base + s_basic_length)
		return true;
	if (addr >= s_charrom_base && addr < s_charrom_base + s_basic_length)
		return true;
	return false;
}

/* vim:set ts=2 sw=2: */
