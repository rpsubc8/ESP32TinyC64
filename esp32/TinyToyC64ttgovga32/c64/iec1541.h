#ifndef __IEC1541_H__
#define __IEC1541_H__

#include "iecdevice.h"
#include "disk.h"

//! \brief Implements a 1541 disk drive
class IEC1541 : public IECDevice {
public:
	IEC1541();
	~IEC1541();

	bool LoadDisk(const char* fname);

	virtual void OnOpen(uint8_t channel);
	virtual void OnClose(uint8_t channel);
	virtual void OnOpenChannel(uint8_t channel, bool write);
	virtual void OnDataByte(uint8_t byte);
	virtual bool IsReadyToReceive();
	virtual void OnDataEnd();
	virtual bool IsDataAvailable();
	virtual uint8_t GetNextByte();

protected:
	//! \brief Disk in use
	Disk m_disk;

	Disk::Handle m_handle;

	//! \brief Current filename buffer
	char m_filename[32 /* XXX */];
};

#endif /*  __IEC1541_H__ */
