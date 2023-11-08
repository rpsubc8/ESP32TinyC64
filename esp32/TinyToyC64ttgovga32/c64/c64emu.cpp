/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
/*
#include "cia1.h"
#include "cia2.h"
#include "cpu6502.h"
#include "iecbus.h"
#include "iec1541.h"
#include "memory.h"
#include "vic.h"
#include "hostio.h"
//JJ #include <err.h>

Memory oMemory;
CPU6502 oCPU(oMemory);

int main(int argc, char* argv[])
{
	HostIO oHostIO;

	IECBus oIECBus;
	VIC oVIC(oCPU, oMemory, oHostIO);
	CIA1 oCIA1(oCPU, oMemory, oHostIO);
	CIA2 oCIA2(oCPU, oMemory, oVIC, oIECBus);
	IEC1541 oFDD;

	if (!oMemory.ReadFromFile("../rom/basic.901226-01.bin", Memory::s_basic_base, Memory::s_basic_length))
	{
		//JJ errx(1, "can't read basic rom");
    }
	if (!oMemory.ReadFromFile("../rom/characters.901225-01.bin", Memory::s_charrom_base, Memory::s_charrom_length))
	{
		//JJ errx(1, "can't read character rom");
    }
	if (!oMemory.ReadFromFile("../rom/kernal.901227-03.bin", Memory::s_kernal_base, Memory::s_kernal_length))
	{
		//JJ errx(1, "can't read kernal rom");
    }
	if (!oFDD.LoadDisk("../disk/c64diagnostic.d64"))
	{
		//JJ errx(1, "can't read demo disk");
    }

#if 0
	// Cartridge support
	if (!oMemory.ReadFromFile("../rom/kbd-test.bin", 0x8000, 8192))
	{
	//JJ 	errx(1, "can't read cartridge rom");
    }
#endif

	if (!oHostIO.Initialize())
	{
	//JJ 	errx(1, "can't initialize host I/O");
    }

	oMemory.RegisterPeripheral(&oVIC, Memory::s_vic_base, Memory::s_vic_length);
	oMemory.RegisterPeripheral(&oCIA1, Memory::s_cia1_base, Memory::s_cia1_length);
	oMemory.RegisterPeripheral(&oCIA2, Memory::s_cia2_base, Memory::s_cia2_length);
	oCPU.Reset();

	oIECBus.RegisterDevice(8, &oFDD);
	
	int frame = 0;
	oCPU.Dump();
	while (!oHostIO.Quitting()) {
		oCIA1.Update();
		oIECBus.Update();

		if (frame == 100000) {
			oVIC.Update();
			oHostIO.Yield();
			frame = 0;
		}
		frame++;
		oCPU.RunOpcode();
//		oCPU.Dump();
	}
	oHostIO.Cleanup();	
}

// vim:set ts=2 sw=2:
*/
