/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "iecbus.h"
#include <assert.h>
#include <stdio.h>
#include <string.h> // XXX
#include "iecdevice.h"

#if 1
#define TRACE(x...)
#else
#define TRACE(x...) printf(x)
#endif

#if 1
#define LOG(x...)
#else
#define LOG(x...) fprintf(stderr, x)
#endif

/*
 * On the C64-side, each pin has an inverter between the CIA#2 output and the
 * actual output; however, this is not the case for pin -> CIA#2 input.
 *
 * The result is that we'll need to inverted logic when setting pins, similar to
 * what the C64 does: setting something ON means making it low because the inverter
 * will do so as well - input is unchanged.
 */
#define SET_LO(x) m_bus_device |= (x)
#define SET_HI(x) m_bus_device &= ~(x)

#define SET_DATA_ON SET_HI(s_Bus_Data)
#define SET_DATA_OFF SET_LO(s_Bus_Data)
#define SET_CLK_ON SET_HI(s_Bus_Clock)
#define SET_CLK_OFF SET_LO(s_Bus_Clock)

#define IS_LO(x) ((m_bus_c64 & (x)) == 0)
#define IS_HI(x) (!IS_LO(x))

#define IS_DATA_LO IS_LO(s_Bus_Data)
#define IS_DATA_HI IS_HI(s_Bus_Data)
#define IS_CLOCK_LO IS_LO(s_Bus_Clock)
#define IS_CLOCK_HI IS_HI(s_Bus_Clock)
#define IS_ATN_LO IS_LO(s_Bus_ATN)
#define IS_ATN_HI IS_HI(s_Bus_ATN)
			
IECBus::IECBus()
{
	Reset();
}

void IECBus::OnChangeDataPortA(uint8_t value)
{
	//JJ TRACE("IECBus::HandleDataPortA(): state=%u, val=0x%x => %c%c%c\n",
	//JJ  m_state, value,
	//JJ  (value & s_PortA_DataOut) ? 'D' : '.',
	//JJ  (value & s_PortA_ClockOut) ? 'C' : '.',
	//JJ  (value & s_PortA_ATNOut) ? 'A' : '.');

	// Store the bus output pins; we'll construct the input ones as they are needed
	uint8_t prev_bus_c64 = m_bus_c64;
	m_bus_c64 &= ~(s_Bus_Data | s_Bus_Clock | s_Bus_ATN);
	if (value & s_PortA_DataOut)
		m_bus_c64 |= s_Bus_Data;
	if (value & s_PortA_ClockOut)
		m_bus_c64 |= s_Bus_Clock;
	if (value & s_PortA_ATNOut)
		m_bus_c64 |= s_Bus_ATN;

	// On /ATN, change state immediately; we'll have to comply soon
	if ((prev_bus_c64 & s_Bus_ATN) == 0 && (m_bus_c64 & s_Bus_ATN) != 0)
		m_state = ST_ATN;

	// On ATN -> /ATN, if the device was pending talker, make it the talker
	if ((prev_bus_c64 & s_Bus_ATN) != 0 && (m_bus_c64 & s_Bus_ATN) == 0) {
		if (m_talk_pending) {
			m_devstate = DS_TALK;
			m_talk_pending = false;
			m_state = ST_TURNAROUND;
			//JJ TRACE("IECBus::OnChangeDataPortA(): device became TALKER -> ST_TURNAROUND\n");
		}
	}
	
	if (m_devstate == DS_LISTEN && m_state == ST_TRANSFER) {
		/*
		 * We are a listener and bytes are being transferred to us; this can only
		 * happen when the C64 is sending us data, so we handle it whenever the
		 * data port value changes.
		 *
		 * Note that, initially, CLK will be high (either because EOI wasn't
		 * signalled or because the talker acknowledged our 'we noticed EOI'
		 * response); the DATA line is only valid on /CLK so we have to wait
		 * until CLK drops before we can store the bit.
		 *
		 * Finally, there is no acknowledge that we got the bit; only at the
		 * end of the transfer we can raise DATA to indicate we received the
		 * byte.
		 */
		if (m_devstate != DS_TALK && IS_CLOCK_LO) {
			// On /CLK, a new bit is standing by which we need to addd
			m_bus_cur_byte  = (m_bus_cur_byte >> 1) | ((m_bus_c64 & s_Bus_Data) ? 0 : 0x80);
			m_bus_cur_bit++;
			if (m_bus_cur_bit == 8) {
				// All bits are in; process what we have
				if (IS_ATN_HI)
					ProcessCommandByte(m_bus_cur_byte);
				else
					ProcessDataByte(m_bus_cur_byte);

				// If the EOI flag was set, signal data end to the device
				if (m_eoi && m_curdevice != NULL)
					m_curdevice->OnDataEnd();

				// Acknowledge successful transfer by raising DATA
				SET_DATA_ON;

				// All done; wait for the next state
				m_state = ST_ACK;
				m_bus_cur_bit = 0;
				m_bus_timeout = 100;
			}
		}
	}
}

uint8_t IECBus::OnReadDataPortA()
{
	uint8_t result = 0;

	// Fill out the bus state as resulting from the C64 and device
	int bus = m_bus_c64 | m_bus_device;
	if (bus & s_Bus_Data)
		result |= s_PortA_DataIn;
	if (bus & s_Bus_Clock)
		result |= s_PortA_ClockIn;

	// Add the bits set by the C64
	if (m_bus_c64 & s_Bus_Data)
		result |= s_PortA_DataOut;
	if (m_bus_c64 & s_Bus_Clock)
		result |= s_PortA_ClockOut;
	if (m_bus_c64 & s_Bus_ATN)
		result |= s_PortA_ATNOut;

	//JJ TRACE("IECBus::OnReadDataPortA(): state=%u, c64bus=%c%c%c, devbus=%c%c, val=0x%x\n",
	//JJ  m_state,
	//JJ  (m_bus_c64 & s_Bus_Data) ? 'D' : '.',
	//JJ  (m_bus_c64 & s_Bus_Clock) ? 'C' : '.',
	//JJ  (m_bus_c64 & s_Bus_ATN) ? 'A' : '.',
	//JJ  (m_bus_device & s_Bus_Data) == 0 ? 'D' : '.',
	//JJ  (m_bus_device & s_Bus_Clock) == 0 ? 'C' : '.',
	//JJ  result);
	return result;
}

void IECBus::Reset()
{
	//JJ TRACE("IECBus::Reset()\n");
	m_state = ST_IDLE;
	m_bus_cur_byte = 0;
	m_bus_cur_bit = 0;
	m_bus_timeout = 10;
	m_talk_pending = false;

	// Initially, nothing is yet asserted on the bus
	SET_DATA_OFF; SET_CLK_OFF;

	// And we are LISTENING to the ATN bytes
	m_devstate = DS_LISTEN;
}

void IECBus::Dump()
{
	static int iTimestamp = 0;
	static int old_data = -1;
	int new_data = (m_bus_c64 << 8) | m_bus_device;
	if (new_data == old_data)
		return;
	if (!iTimestamp)
		LOG("#ts\tc64atn\tc64clk\tc64dat\tdevclk\tdevdat\n");
	old_data = new_data;
	LOG("%u\t%u\t%u\t%u\t%u\t%u\n",
	 iTimestamp++,
	(m_bus_c64 & s_Bus_ATN) ? 1 : 0,
	(m_bus_c64 & s_Bus_Clock) ? 1 : 0,
	(m_bus_c64 & s_Bus_Data) ? 1 : 0,
	(m_bus_device & s_Bus_Clock) ? 1 : 0,
	(m_bus_device & s_Bus_Data) ? 1 : 0);
}

void IECBus::Update()
{
	/*
	 * ATN can be signalled at any time; the C64 will assume talker role once it
	 * raises CLK - we have to stop whatever we are doing and become a listener.
	 */
	if (m_state == ST_ATN && IS_CLOCK_HI) {
		//JJ TRACE("IECBus::UpdateListener: ATN signalled, CLK signalled -> resetting\n");
		Reset();
		return;
	}

	switch(m_devstate) {
		case DS_LISTEN:
			UpdateListener();
			break;
		case DS_TALK:
			UpdateTalker();
			break;
		default:
			assert(0);
	}
	//JJ Dump();
}

void IECBus::UpdateListener()
{
	switch(m_state) {
		case ST_IDLE:
			// We have to wait a bit and raise DATA to signal we are ready;
			// signalling it too quickly causes problems
			if (m_bus_timeout > 0) {
				// Need to timeout
				if (--m_bus_timeout == 0) {
					//JJ TRACE("IECBus::UpdateListener: ST_IDLE expired -> ST_DATA_HI\n");
					m_state = ST_DATA_HI;
					SET_DATA_ON;
				}
			}
			break;
		case ST_DATA_HI:
			// We've raised DATA, wait for /CLK
			if (IS_CLOCK_LO) {
				// CLK was dropped; submit request to send by dropping DATA
				SET_DATA_OFF;
				//JJ TRACE("IECBus::UpdateListener: acknowledged /CLK; ready for CLK -> WAIT_EOI\n");
				m_state = ST_WAIT_EOI;
				m_bus_timeout = 100;
			}
			break;
		case ST_WAIT_EOI:
			/*
			 * We are checking whether the talker is signalling EOI; it will either
			 *
			 * (1) Signal CLK, which means EOI isn't signalled
			 * (2) Do nothing (CLK remains hi), which means it wants to signal EOI
			 *     We need to raise DATA for at least 60ms to signal we noticed the
			 *     EOI.
			 *
			 * (1) has to happen within 200ms, otherwise (2) is assumed.
			 */
			if (IS_CLOCK_HI) {
				// Case (1): EOI is not active so we can transfer right away
				//JJ TRACE("IECBus::UpdateListener: CLK while waiting for EOI, no eoi -> ST_TRANSFER\n");
				m_state = ST_TRANSFER;
				m_eoi = false;
			} else if (IS_CLOCK_LO && --m_bus_timeout == 0) {
				// Case (2): Timeout happened and /CLK is still the case, EOI is active
				SET_DATA_ON; // Acknowledge EOI
				//JJ TRACE("IECBus::UpdateListener: no CLK, timeout expired: eoi signalled -> ST_ACK_EOI\n");
				m_state = ST_ACK_EOI;
				m_eoi = true;
				m_bus_timeout = 10;
			}
			break;
		case ST_ACK_EOI:
			/*
			 * We noticed the EOI condition and signalled our approval by raising
			 * DATA; we now have to wait until CLK goes hi - once this is the case,
			 * we drop DATA and handle the bits as they pour in.
			 */
			if (IS_CLOCK_HI) {
				SET_DATA_OFF; // ACK clock
				//JJ TRACE("IECBus::UpdateListener: got CLK -> ST_TRANSFER\n");
				m_state = ST_TRANSFER;
			}
			if (--m_bus_timeout == 0) {
				SET_DATA_OFF;
				//JJ TRACE("IECBus::UpdateListener: EOI timeout expired -> ST_TRANSFER\n");
				m_state = ST_TRANSFER;
			}
			break;
		case ST_ACK:
			if (--m_bus_timeout == 0) {
				// We are done ACK'ing; if EOI was signalled, we just go back to idle;
				// otherwise, we must wait for the next byte
				if (m_eoi) {
					//JJ TRACE("IECBus::UpdateListener: done ACK, eoi active -> ST_IDLE\n");
					SET_DATA_OFF;
					m_state = ST_IDLE;
				} else {
					//JJ TRACE("IECBus::UpdateListener: done ACK, no eoi -> ST_DATA_HI\n");
					m_state = ST_DATA_HI;
				}
			}
			break;
		case ST_ATN:
			/*
			 * ATN can be signalled at any time; once CLK is raised, the C64 has assumed
			 * talker role and we have to become a listener.
			 */
			if (IS_CLOCK_HI) {
				//JJ TRACE("IECBus::UpdateListener: ATN signalled, CLK signalled -> resetting\n");
				Reset();
			}
			break;
		default:
			break;
	}
}

void IECBus::ProcessCommandByte(uint8_t byte)
{
	uint8_t channel = byte & 0xf;
	switch((byte & 0xf0)) {
		case s_Command_Open:
			//JJ TRACE("IECBus::ProcessCommandByte(): got OPEN (%x)\n", byte);
			if (m_curdevice != NULL)
				m_curdevice->OnOpen(channel);
			return;
		case s_Command_Close:
			//JJ TRACE("IECBus::ProcessCommandByte(): got CLOSE (%x)\n", byte);
			if (m_curdevice != NULL)
				m_curdevice->OnClose(channel);
			return;
		case s_Command_Listen:
			//JJ TRACE("IECBus::ProcessCommandByte(): got LISTEN (%x), channel %x\n", byte, byte & 0xf);
			m_devstate = DS_LISTEN;
			m_curdevice = FindDevice(byte & 0xf);
			return;
		case s_Command_Talk:
			//JJ TRACE("IECBus::ProcessCommandByte(): got TALK (%x), channel %x\n", byte, byte & 0xf);
			m_talk_pending = true;
			m_curdevice = FindDevice(byte & 0xf);
			return;
		case s_Command_OpenChannel:
			//JJ TRACE("IECBus::ProcessCommandByte(): got OPEN CHANNEL (%x), channel %x\n", byte, byte & 0xf);
			if (m_curdevice != NULL)
				m_curdevice->OnOpenChannel(byte & 0xf, !m_talk_pending);
			return;
	}

	switch(byte) {
		case s_Command_Unlisten:
			//JJ TRACE("IECBus::ProcessCommandByte(): got UNLISTEN (%x)\n", byte);
			m_curdevice = NULL;
			return;
		case s_Command_Untalk:
			//JJ TRACE("IECBus::ProcessCommandByte(): got UNTALK (%x)\n", byte);
			m_curdevice = NULL;
			return;
		default:
			//JJ TRACE("IECBus::ProcessCommandByte(): unrecognized byte 0x%x, ignored\n", byte);
			break;
	}
}

void IECBus::UpdateTalker()
{
	switch(m_state) {
		case ST_IDLE:
			// Wait until DATA is hi; this happens when the listener is ready
			if (IS_DATA_HI) {
				// If we have data to send, signal /CLK after a bit; this means we are
				// ready to send
				if (m_bus_timeout > 0 && --m_bus_timeout == 0) {
					if (!m_eoi) {
						m_state = ST_DATA_HI;
						SET_CLK_OFF;
						//JJ TRACE("IECBus::UpdateTalker(): data hi -> ST_DATA_HI\n");
					} else {
						/*
						 * No data available - this only happens when there is no data
						 * _initially_; dropping the CLK and DATA lines will lead to the
						 * C64 recognizing this as an error condition. The device will
						 * just remain stuck in ST_IDLE, from which it will recover once
						 * /ATN is signalled.
						 */
						//JJ TRACE("IECBus::UpdateTalker(): no data to send -> signalling error\n");
						SET_CLK_OFF;
						SET_DATA_OFF;
					}
				}
			}
			break;
		case ST_DATA_HI:
 			/*
			 * /DATA indicates the listener is ready to receive our data; we'll have
			 * to handle the EOI case here: if we do nothing, the listener will
			 * assume we are signalling EOI; if we raise CLK, it will know more bytes
			 * are to follow and things will progress as usual.
			 */
			if (IS_DATA_LO) {
				m_bus_cur_byte = m_curdevice->GetNextByte();
				m_eoi = !m_curdevice->IsDataAvailable();
				if (!m_eoi) {
					// No EOI; raise CLK and process as usual
					//JJ TRACE("IECBus::UpdateTalker(): data lo, device ready, no eoi -> ST_TRANSFER_WAIT\n");
					SET_CLK_ON; // do not signal EOI
					m_state = ST_TRANSFER_WAIT;
					m_bus_cur_bit = 0;
					m_bus_timeout = 50;

				} else {
					// EOI is to be signalled
					//JJ TRACE("IECBus::UpdateTalker(): data lo, device ready, eoi -> ST_WAIT_EOI\n");
					m_bus_timeout = 100;
					m_state = ST_WAIT_EOI;
				}
			}
			break;
		case ST_WAIT_EOI:
			// We are signalling EOI; we'll just wait until the device signals DATA
			// to acknowledge our EOI condition
			if (IS_DATA_HI) {
				//JJ TRACE("IECBus::UpdateTalker(): got EOI acknowledge -> ST_ACK_EOI\n");
				m_state = ST_ACK_EOI;
			}
			break;
		case ST_ACK_EOI:
			// DATA was signalled; wait for /DATA and acknowledge it by raising CLK;
			// we can then transfer data as usual
			if (IS_DATA_LO) {
				//JJ TRACE("IECBus::UpdateTalker(): data lo -> ST_TRANSFER_WAIT\n");
				SET_CLK_ON;
				m_state = ST_TRANSFER_WAIT;
				m_bus_cur_bit = 0;
				m_bus_timeout = 50;
			}
			break;
		case ST_ACK:
			// The complete byte was sent; wait for the device to acknowledge
			if (IS_DATA_HI) {
				// Once we are acknowledged; raise CLK and repeat the whole ordeal
				//JJ TRACE("IECBus::UpdateTalker(): data hi -> ST_IDLE\n");
				m_state = ST_IDLE;
				m_bus_timeout = 10;
				SET_DATA_OFF; SET_CLK_ON;
			}
			break;
		case ST_TRANSFER_WAIT:
			// This state is used as an intermediate state while CLK is raised
			if (--m_bus_timeout == 0) {
				//JJ TRACE("IECBus::UpdateTalker(): timeout expired -> ST_TRANSFER\n");
				m_state = ST_TRANSFER;
			}
			break;
		case ST_TRANSFER_DATA:
			/*
			 * This state is used to give the listener time to look at the DATA we
			 * placed on the bus; as there is no acknowledge per bit sent, we'll just
			 * wait until it's time to send the next bit.
			 *
			 * Note that we always come here with /CLK; this means the current
			 * bit is represented in the DATA line. After a while, we'll raise CLK
			 * and determine what to do from there.
			 */
			if (--m_bus_timeout == 0) {
				m_bus_timeout = 50;
				SET_CLK_ON; // data is no longer valid
				if (m_bus_cur_bit == 8) {
					//JJ TRACE("IECBus::UpdateTalker(): all bits sent -> ST_ACK\n");
					m_state = ST_ACK;
				} else {
					//JJ TRACE("IECBus::UpdateTalker(): timeout expired -> ST_TRANSFER_WAIT\n");
					m_state = ST_TRANSFER_WAIT;
				}
			}
			break;
		case ST_TRANSFER:
			//JJ TRACE("IECBus::UpdateTalker(): timeout expired, transferring bit %u\n", m_bus_cur_bit);
			// place the current bit on the bus
			if (m_bus_cur_byte & (1 << m_bus_cur_bit))
				SET_DATA_OFF;
			else
				SET_DATA_ON;
			SET_CLK_OFF; // data signal is valid

			m_bus_cur_bit++;
			m_bus_timeout = 50;
			m_state = ST_TRANSFER_DATA;
			break;
		case ST_TURNAROUND:
			/*
			 * Note that this state occurs in _listening_ mode when we are about to
			 * become talker.
			 */
			if (IS_DATA_HI && IS_CLOCK_LO) {
				//JJ TRACE("IECBus::UpdateListener: ATN turnaround, DATA hi, CLK inactive\n");
				SET_DATA_OFF; SET_CLK_ON;
				m_state = ST_IDLE;
				m_bus_timeout = 10;

				// Update initial EOI state; if this is false, ST_IDLE will raise an
				// error
				m_eoi = !m_curdevice->IsDataAvailable();
			}
			break;
		default:
			break;
	}
}

void IECBus::ProcessDataByte(uint8_t byte)
{
	//JJ TRACE("IECBus::ProcessDataByte(): got byte 0x%x (%c)\n", byte, byte);
	if (m_curdevice != NULL)
		m_curdevice->OnDataByte(byte);
}

IECDevice* IECBus::FindDevice(uint8_t id)
{
	TIDDeviceMap::iterator it = m_device.find(id);
	if (it == m_device.end())
		return NULL;
	return it->second;
}

void IECBus::RegisterDevice(int id, IECDevice* pDevice)
{
	std::pair<TIDDeviceMap::iterator, bool> it = m_device.insert(std::pair<int, IECDevice*>(id, pDevice));
	assert(it.second); // id already in use if this fails
}

/* vim:set ts=2 sw=2: */
