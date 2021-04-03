/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "iec1541.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "disk.h"

#if 1
#define TRACE(x...)
#else
#define TRACE(x...) printf(x)
#endif

IEC1541::IEC1541()
{
}

IEC1541::~IEC1541()
{
}

void IEC1541::OnOpen(uint8_t channel)
{
	//JJ TRACE("IEC1541::OnOpen(): channel=%u\n", channel);
	strcpy(m_filename, "");
}

void IEC1541::OnClose(uint8_t channel)
{
	//JJ TRACE("IEC1541::OnClose(): channel=%u\n", channel);
}

void IEC1541::OnOpenChannel(uint8_t channel, bool write)
{
	//JJ TRACE("IEC1541::OnOpenChannel(): channel=%u, write=%u\n", channel, !!write);
	//JJ TRACE("IEC1541::OnOpenChannel(): filename '%s'\n", m_filename);
	m_disk.OpenFile(m_filename, m_handle);
}

void IEC1541::OnDataByte(uint8_t byte)
{
	//JJ TRACE("IEC1541::OnDataByte(): byte=%u\n", byte);

	char s[2] = { (char)byte, '\0' };
	strcat(m_filename, s); /* XXX */
}

bool IEC1541::IsReadyToReceive()
{
	//JJ TRACE("IEC1541::IsReadyToReceive()\n");
	return true;
}

void IEC1541::OnDataEnd()
{
	//JJ TRACE("IEC1541::OnDataEnd()\n");
}

bool IEC1541::IsDataAvailable()
{
	//JJ TRACE("IEC1541::IsDataAvailable()\n");
	return !m_handle.AtEOF();
}

uint8_t IEC1541::GetNextByte()
{
	//JJ TRACE("IEC1541::GetNextByte()\n");
	return m_handle.GetNextByte();
}

bool IEC1541::LoadDisk(const char* fname)
{
	return m_disk.Load(fname);
}
	
/* vim:set ts=2 sw=2: */
