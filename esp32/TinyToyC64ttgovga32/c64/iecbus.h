#ifndef __CBMBUS_H__
#define __CBMBUS_H__

#include <stdint.h>
#include <map>

class IECDevice;

class IECBus {
public:
	IECBus();
	void OnChangeDataPortA(uint8_t value);
	uint8_t OnReadDataPortA();

	void RegisterDevice(int id, IECDevice* pDevice);
	void Reset();
	void Update();

protected:
	//! \brief Data port A settings
	static const uint8_t s_PortA_DataIn = (1 << 7);
	static const uint8_t s_PortA_ClockIn = (1 << 6);
	static const uint8_t s_PortA_DataOut = (1 << 5);
	static const uint8_t s_PortA_ClockOut = (1 << 4);
	static const uint8_t s_PortA_ATNOut = (1 << 3);

	//! \brief Bus state
	static const uint8_t s_Bus_Data = (1 << 0);
	static const uint8_t s_Bus_Clock = (1 << 1);
	static const uint8_t s_Bus_ATN = (1 << 2);

	//! \brief ATN bytes
	static const uint8_t s_Command_Listen = 0x20;
	static const uint8_t s_Command_Unlisten = 0x3f;
	static const uint8_t s_Command_Talk = 0x40;
	static const uint8_t s_Command_Untalk = 0x5f;
	static const uint8_t s_Command_OpenChannel = 0x60;
	static const uint8_t s_Command_Close = 0xe0;
	static const uint8_t s_Command_Open = 0xf0;

	//! \brief Find a device by ID
	IECDevice* FindDevice(uint8_t id);

private:
	void ProcessCommandByte(uint8_t byte);
	void ProcessDataByte(uint8_t byte);
	void Dump();

	enum State {
		ST_IDLE,
		ST_DATA_HI,
		ST_WAIT_EOI,
		ST_ACK_EOI,
		ST_TRANSFER,
		ST_TRANSFER_WAIT,
		ST_TRANSFER_DATA,
		ST_ACK,
		ST_ATN,
		ST_TURNAROUND
	};

	enum DeviceState {
		DS_LISTEN,
		DS_TALK,
	};

	//! \brief Bus state as set by the C64
	uint8_t m_bus_c64;

	//! \brief Bus state as set by the devices
	uint8_t m_bus_device;

	//! \brief Updates communication in listener mode
	void UpdateListener();

	//! \brief Updates communication in talker mode
	void UpdateTalker();

	State m_state;
	DeviceState m_devstate;
	uint8_t m_bus_cur_byte;
	int m_bus_cur_bit;

	int m_bus_timeout;
	bool m_eoi;
	bool m_talk_pending;

	char m_bus_dataout[64];

	//! \brief Map of all ID -> device pairs attached to the bus
	typedef std::map<int, IECDevice*> TIDDeviceMap;
	TIDDeviceMap m_device;

	//! \brief Current active device, if any
	IECDevice* m_curdevice;
};

#endif /* __CBMBUS_H__ */
