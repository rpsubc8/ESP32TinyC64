/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "cia1.h"
#include "hostio.h"
#include "vic.h"
#include <stdio.h>
#include <string.h>
#include "gbConfig.h"

void CIA1::Reset()
{
	m_kbd_cur_columns = 0;
	memset(m_kbd_matrix, 0xff, sizeof(m_kbd_matrix));

	ResetCIA();
	//JJ m_hostio.SetKeyCallback(&CIA1::KeyCallback, this);
}

void CIA1::Poke(Memory::addr_t addr, uint8_t val)
{
	ByteAt(addr) = val;

    //if ((addr >= 0xD800) && (addr <= 0xDBE7))
    //{
    // printf("CIA1::Poke addr:0x%x val:%x\n", addr, val);
    //}

	// Try base CIA first
	if (PokeCIA(addr, val))
		return;

	switch(addr & 0xff) {
		case 0x0: // write: keyboard column values for keyboard scan
			m_kbd_cur_columns = val;
			break;
		default:
		    #ifdef use_lib_log_serial
			 printf("CIA1::Poke(): unhandled addr=0x%x val=%x\n", addr, val);
			#endif 
			break;
	}
}

void CIA1::KeyCallback(void* ptr, int key, bool pressed)
{
 CIA1* pCIA1 = (CIA1*)ptr;

 //JJ if (pressed)
 //JJ  pCIA1->m_kbd_matrix[key / 8] &= ~(1 << (key & 7));
 //JJ else
 //JJ  pCIA1->m_kbd_matrix[key / 8] |=  (1 << (key & 7));

 if (pressed)
  pCIA1->m_kbd_matrix[(key>>3)] &= ~(1 << (key & 7));
 else
  pCIA1->m_kbd_matrix[(key>>3)] |=  (1 << (key & 7));
}

//*******************************
void CIA1::TestTeclado(unsigned char aKey,unsigned char aPress)
{
 if (aPress == 0)
  m_kbd_matrix[(aKey>>3)] &= ~(1 << (aKey & 7));
 else
  m_kbd_matrix[(aKey>>3)] |= (1 << (aKey & 7));
}

uint8_t CIA1::Peek(Memory::addr_t addr)
{
	uint8_t val;
	if (PeekCIA(addr, val))
		return val;

	switch(addr & 0xff) {
		case 0x1: // read: keyboard row values for keyboard scan
			val = 0xff;
			for (int i = 0; i < 8; i++)
				if ((m_kbd_cur_columns & (1 << i)) == 0)
					val &= m_kbd_matrix[i];
			return val;
		default:
		    #ifdef use_lib_log_serial
			 printf("CIA1::Peek(): unhandled addr=0x%x\n", addr);
			#endif
			break;
	}
	return ByteAt(addr);
}

void CIA1::Update()
{
	UpdateCIA();
}

/* vim:set ts=2 sw=2: */
