/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "cia.h"
#include <stdio.h>

#if 1
#define TRACE(x...)
#else
#define TRACE(x...) printf(x)
#endif

void CIA::ResetCIA()
{
	m_icr = 0; m_cra = 0; m_crb = 0; m_read_irqs = 0;
}

bool CIA::PokeCIA(Memory::addr_t addr, uint8_t val)
{
	ByteAt(addr) = val;

    //if ((addr >= 0xD800) && (addr <= 0xDBE7))
    //{
    // printf("PokeCIA addr:0x%x val:%x\n", addr, val);
    //}

	switch(addr & 0xff) {
		case 0x4: // timer a lo-byte
			m_timer_a = (m_timer_a & 0xff00) | val;
			break;
		case 0x5: // timer a hi-byte
			m_timer_a = (m_timer_a & 0xff) | (uint16_t)val << 8;
			break;
		case 0x6: // timer b lo-byte
			m_timer_b = (m_timer_b & 0xff00) | val;
			break;
		case 0x7: // timer b hi-byte
			m_timer_b = (m_timer_b & 0xff) | (uint16_t)val << 8;
			break;
		case 0xd: // interrupt control
			if ((val & 0x80) == 0) {
				m_icr &= ~(val & 0x7f);
			} else {
				m_icr |= (val & 0x7f);
			}
			//JJ TRACE("CIA::Poke(): val=%x => icr=%x\n", val, m_icr);
			break;
		case 0xe: // interrupt control register a
			m_cra = val;
			//JJ TRACE("CIA::Poke(): cra=%x\n", m_cra);
			break;
		case 0xf: // interrupt control register b
			m_crb = val;
			//JJ TRACE("CIA::Poke(): crb=%x\n", m_crb);
			break;
		default:
			return false; // what's this?
	}

	return true;
}

bool CIA::PeekCIA(Memory::addr_t addr, uint8_t& val)
{
	switch(addr & 0xff) {
		case 0xd: { // interrupt control register
			val = m_read_irqs;
			m_read_irqs = 0;
			break;
		}
		default:
			return false; // what's this?
	}
	return true;
}

void CIA::UpdateCIA()
{
	// Reload timer A if needed
	if (m_cra & (1 << 4)) {
		m_timerval_a = m_timer_a;
		m_cra &= ~(1 << 4);
	}
	
	// Reload timer B if needed
	if (m_crb & (1 << 4)) {
		m_timerval_b = m_timer_b;
		m_crb &= ~(1 << 4);
	}

	// If timer A is enabled, let it count down
	if (m_cra & (1 << 0)) {
		if (--m_timerval_a == 0) {
			m_read_irqs |= m_irq_timer_a;
			if (m_icr & (1 << 0))
				SignalIRQ();

			// Reload value
			m_timerval_a = m_timer_a;

			// If the timer is in one-shot mode, disable it
			if (m_cra & (1 << 3))
				m_cra &= ~(1 << 0);
		}
	}

	// If timer B is enabled, let it count down
	if (m_crb & (1 << 0)) {
		if (--m_timerval_b == 0) {
			m_read_irqs |= m_irq_timer_b;
			if (m_icr & (1 << 1))
				SignalIRQ();

			// Reload value
			m_timerval_b = m_timer_b;

			// If the timer is in one-shot mode, disable it
			if (m_crb & (1 << 3))
				m_crb &= ~(1 << 0);
		}
	}
}

/* vim:set ts=2 sw=2: */
