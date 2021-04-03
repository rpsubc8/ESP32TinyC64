#ifndef __IECDEVICE_H__
#define __IECDEVICE_H__

#include <stdint.h>

/*! \brief IEC-connected device interface class
 *
 *  This class is used to send higher-level commands to a IEC-connected
 *  device, such as a disk drive.
 *
 */
class IECDevice {
public:
	virtual ~IECDevice() { }

	/*! \brief Called when a open request is recieved
 	 *  \param channel Channel to open
	 *
	 *  This corresponds with the 0xF0 OPEN command normally used
	 *  to open a given file for reading or writing.
	 */
	virtual void OnOpen(uint8_t channel) = 0;

	/*! \brief Called when a close request is recieved
 	 *  \param channel Channel to close
	 *
	 *  This corresponds with the 0xE0 CLOSE command normally used
	 *  to close the currently-opened file.
	 */
	virtual void OnClose(uint8_t channel) = 0;

	/*! \brief Called when a open channel request is recieved
	 *  \param channel Channel to use
	 *  \param write If true, channel is opened for writing
	 *
	 *  This corresponds with the 0x60 OPEN CHANNEL command; the write flag
	 *  is based on whether this is a TALK or LISTEN request - the flag is
	 *  based on the C64's point of view, so write=true means the C64 will
	 *  write to the device.
	 *
	 *  If the channel is opened for reading, is IsDataAvailable() and
	 *  GetNextByte() functions are used; if the first IsDataAvailable()
	 *  call returns false, the resource is assumed to be missing and an
	 *  error will be reported.
	 *
	 *  If the channel is opened for writing, OnDataByte() will be called
	 *  at least a single time followed by OnDataEnd().
	 */
	virtual void OnOpenChannel(uint8_t channel, bool write) = 0;

	//! \brief Called when a data byte is received
	virtual void OnDataByte(uint8_t byte) = 0;

	//! \brief Called to determine whether the device is ready for data
	virtual bool IsReadyToReceive() = 0;

	//! \brief Called when all data bytes are in
	virtual void OnDataEnd() = 0;

	//! \brief Is there data available?
	virtual bool IsDataAvailable() = 0;

	//! \brief Retrieve the next data byre
	virtual uint8_t GetNextByte() = 0;

protected:
};


#endif /* __IECDEVICE_H__ */
