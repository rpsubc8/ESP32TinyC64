/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "cpu6502.h"
#include <assert.h>
#include <stdio.h>
#include "memory.h"
#include "gbConfig.h"

#ifdef usb_lib_fast_GetCyclesForOpcode
 static unsigned char jj_fast_GetCyclesForOpcode[256]={
  7,6,0,0,0,3,5,0,3,2,2,0,0,4,6,0,
  2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
  6,6,0,0,3,3,5,0,4,2,2,0,4,4,6,0,
  2,5,0,0,0,4,6,0,2,3,0,0,0,4,7,0,
  6,6,0,0,0,3,5,0,3,2,2,0,3,4,6,0,
  2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
  6,6,0,0,0,3,5,0,4,2,2,0,5,4,6,0, 
  2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
  0,6,0,0,3,3,3,0,2,0,2,0,4,4,4,0,
  2,6,0,0,4,4,4,0,2,5,2,0,0,5,0,0,
  2,6,2,0,3,3,3,0,2,2,2,0,4,4,4,0,
  2,5,0,0,4,4,4,0,2,4,2,0,4,4,4,0,
  2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,
  2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0,
  2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0,
  2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0
 };
#endif 


CPU6502::CPU6502(Memory& oMemory)
	:m_memory(oMemory), m_info(*this)	
{
	//JJ Reset();
}

//*****************************************
void CPU6502::AssignPtrMemory6502(uint8_t * auxPtr)
{
 m_memory.AssignPtrMemory(auxPtr);
 Reset();
}

void CPU6502::Reset()
{
	m_memory.Reset();

	/* 1.2.1.2.11 /RES */
	m_pc = m_memory.ReadWord(Memory::s_vector_reset);
	m_irq_pending = false; m_nmi_pending = false;

	/* XXX */
	m_flags = 0;
}

int CPU6502::RunOpcode()
{
#define DO_ACC(op) \
	m_a = op(m_a)

#define DO_X(op) \
	m_x = op(m_x)

#define DO_Y(op) \
	m_y = op(m_y)

#define GET_ZEROPAGE \
	addr = ReadByteAtPC()

#define GET_ZEROPAGE_X \
	addr = (ReadByteAtPC() + m_x) & 0xff

#define GET_ZEROPAGE_Y \
	addr = (ReadByteAtPC() + m_y) & 0xff

#define GET_ABSOLUTE \
	addr = ReadWordAtPC()

#define GET_ABSOLUTE_X \
	addr = ReadWordAtPC() + m_x

#define GET_ABSOLUTE_Y \
	addr = ReadWordAtPC() + m_y

#define GET_INDIRECT_X \
	addr = m_memory.ReadWord((ReadByteAtPC() + m_x) & 0xff)

#define GET_INDIRECT_Y \
	addr = m_memory.ReadWord(ReadByteAtPC()) + m_y

#define DO_ZEROPAGE(op) \
	GET_ZEROPAGE; \
	m_memory.WriteByte(addr, op(m_memory.ReadByte(addr)));

#define DO_ZEROPAGE_X(op) \
	GET_ZEROPAGE_X; \
	m_memory.WriteByte(addr, op(m_memory.ReadByte(addr)));

#define DO_ABSOLUTE(op) \
	GET_ABSOLUTE; \
	m_memory.WriteByte(addr, op(m_memory.ReadByte(addr)));

#define DO_ABSOLUTE_X(op) \
	GET_ABSOLUTE_X; \
	m_memory.WriteByte(addr, op(m_memory.ReadByte(addr)));
	
	if (m_tracing) {
		Dump();
		char buf[1024];
		m_info.Disassemble(buf);
		printf(">> %s\n", buf);
	}

	uint16_t addr; // for use in the GET_... macros
	uint8_t opcode = ReadByteAtPC();
	#ifdef usb_lib_fast_GetCyclesForOpcode
 	 int iCycles = jj_fast_GetCyclesForOpcode[opcode];
	#else
 	 int iCycles = m_info.GetCyclesForOpcode(opcode);
 	#endif	
	switch(opcode) {
		case 0x00: /* brk */
			assert(0); /* XXX for now; this is almost always a bug */
			break;
		case 0x01: /* ora (in,x) */
			DoORA(GetIndirectX());
			break;
		case 0x05: /* ora - zp */
			DoORA(GetZeroPage());
			break;
		case 0x06: /* asl - zp */
			DO_ZEROPAGE(DoASL);
			break;
		case 0x08: /* php */
			PushByte(m_flags);
			break;
		case 0x09: /* ora - im */
			DoORA(GetImmediate());
			break;
		case 0x0a: /* asl - acc */
			DO_ACC(DoASL);
			break;
		case 0x0d: /* ora - abs */
			DoORA(GetAbsolute());
			break;
		case 0x0e: /* asl - abs */
			DO_ABSOLUTE(DoASL);
			break;
		case 0x10: /* bpl */
			DoBranch((m_flags & Flag_N) == 0, iCycles);
			break;
		case 0x11: /* ora (in,y) */
			DoORA(GetIndirectY(iCycles));
			break;
		case 0x15: /* ora - zp,x */
			DoORA(GetZeroPageX());
			break;
		case 0x16: /* asl - zp,x */
			DO_ZEROPAGE_X(DoASL);
			break;
		case 0x18: /* clc */
			m_flags &= ~Flag_C;
			break;
		case 0x19: /* ora - abs,y */
			DoORA(GetAbsoluteY(iCycles));
			break;
		case 0x1d: /* ora - abs,x */
			DoORA(GetAbsoluteX(iCycles));
			break;
		case 0x1e: /* asl - abs,x */
			DO_ABSOLUTE_X(DoASL);
			break;
		case 0x20: /* jsr */
			GET_ABSOLUTE;
			PushWord(m_pc - 1);
			m_pc = addr;
			break;
		case 0x21: /* and - (in,x) */
			DoAND(GetIndirectX());
			break;
		case 0x24: /* bit - zp */
			DoBIT(GetZeroPage());
			break;
		case 0x25: /* and - zp */
			DoAND(GetZeroPage());
			break;
		case 0x26: /* rol - zp */
			DO_ZEROPAGE(DoROL);
			break;
		case 0x28: /* plp */
			m_flags = PopByte() | Flag_1;
			break;
		case 0x29: /* and - im */
			DoAND(GetImmediate());
			break;
		case 0x2a: /* rol - acc */
			DO_ACC(DoROL);
			break;
		case 0x2c: /* bit - abs  */
			DoBIT(GetAbsolute());
			break;
		case 0x2d: /* and - abs */
			DoAND(GetAbsolute());
			break;
		case 0x2e: /* rol - abs */
			DO_ABSOLUTE(DoROL);
			break;
		case 0x30: /* bmi */
			DoBranch((m_flags & Flag_N) != 0, iCycles);
			break;
		case 0x31: /* and - (ind),y */
			DoAND(GetIndirectY(iCycles));
			break;
		case 0x35: /* and - zp,x */
			DoAND(GetZeroPageX());
			break;
		case 0x36: /* rol - zp,x */
			DO_ZEROPAGE_X(DoROL);
			break;
		case 0x38: /* sec */
			m_flags |= Flag_C;
			break;
		case 0x39: /* and - abs,y */
			DoAND(GetAbsoluteY(iCycles));
			break;
		case 0x3d: /* and - abs,x */
			DoAND(GetAbsoluteX(iCycles));
			break;
		case 0x3e: /* rol - abs,x */
			DO_ABSOLUTE_X(DoROL);
			break;
		case 0x40: /* rti */
			m_flags = PopByte() | Flag_1;
			m_pc = PopWord();
			break;
		case 0x41: /* eor - (ind,x) */
			DoEOR(GetIndirectX());
			break;
		case 0x45: /* eor - zp */
			DoEOR(GetZeroPage());
			break;
		case 0x46: /* lsr - zp */
			DO_ZEROPAGE(DoLSR);
			break;
		case 0x48: /* pha */
			PushByte(m_a);
			break;
		case 0x49: /* eor - imm */
			DoEOR(GetImmediate());
			break;
		case 0x4a: /* lsr - acc */
			DO_ACC(DoLSR);
			break;
		case 0x4c: /* jmp - abs */
			m_pc = ReadWordAtPC();
			break;
		case 0x4d: /* eor - abs */
			DoEOR(GetAbsolute());
			break;
		case 0x4e: /* lsr - abs */
			DO_ABSOLUTE(DoLSR);
			break;
		case 0x50: /* bvc */
			DoBranch((m_flags & Flag_V) == 0, iCycles);
			break;
		case 0x51: /* eor - (ind,y) */
			DoEOR(GetIndirectY(iCycles));
			break;
		case 0x55: /* eor - zp,x */
			DoEOR(GetZeroPageX());
			break;
		case 0x56: /* lsr - zp,x */
			DO_ZEROPAGE_X(DoLSR);
			break;
		case 0x58: /* cli */
			m_flags &= ~Flag_I;
			break;
		case 0x59: /* eor - abs,y */
			DoEOR(GetAbsoluteY(iCycles));
			break;
		case 0x5d: /* eor - abs,x */
			DoEOR(GetAbsoluteX(iCycles));
			break;
		case 0x5e: /* lsr - abs,x */
			DO_ABSOLUTE_X(DoLSR);
			break;
		case 0x60: /* rts */
			m_pc = PopWord() + 1;
			break;
		case 0x61: /* adc - (ind,x) */
			DoADC(GetIndirectX());
			break;
		case 0x65: /* adc - zp */
			DoADC(GetZeroPage());
			break;
		case 0x66: /* ror - zp */
			DO_ZEROPAGE(DoROR);
			break;
		case 0x68: /* pla */
			m_a = PopByte();
			UpdateNZ(m_a);
			break;
		case 0x69: /* adc - imm */
			DoADC(GetImmediate());
			break;
		case 0x6a: /* ror - acc */
			DO_ACC(DoROR);
			break;
		case 0x6c: /* jmp - ind */
			addr = ReadWordAtPC();
			m_pc = m_memory.ReadWord(addr);
			break;
		case 0x6d: /* adc - abs */
			DoADC(GetAbsolute());
			break;
		case 0x6e: /* ror - abs */
			DO_ABSOLUTE(DoROR);
			break;
		case 0x70: /* bvs */
			DoBranch((m_flags & Flag_V) != 0, iCycles);
			break;
		case 0x71: /* adc - (ind),y */
			DoADC(GetIndirectY(iCycles));
			break;
		case 0x75: /* adc - zp,x */
			DoADC(GetZeroPageX());
			break;
		case 0x76: /* ror - zp,x */
			DO_ZEROPAGE_X(DoROR);
			break;
		case 0x78: /* sei */
			m_flags |= Flag_I;
			break;
		case 0x79: /* adc - abs,y */
			DoADC(GetAbsoluteY(iCycles));
			break;
		case 0x7d: /* adc - abs,x */
			DoADC(GetAbsoluteX(iCycles));
			break;
		case 0x7e: /* ror - abs,x */
			DO_ABSOLUTE_X(DoROR);
			break;
		case 0x81: /* sta - (ind,x) */
			GET_INDIRECT_X; 
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x84: /* sty - zp */
			GET_ZEROPAGE;
			m_memory.WriteByte(addr, m_y);
			break;
		case 0x85: /* sta - zp */
			GET_ZEROPAGE;
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x86: /* stx - zp */
			GET_ZEROPAGE;
			m_memory.WriteByte(addr, m_x);
			break;
		case 0x88: /* dey */
			DO_Y(DoDEC);
			break;
		case 0x8a: /* txa */
			m_a = m_x;
			UpdateNZ(m_a);
			break;
		case 0x8c: /* sty - abs */
			GET_ABSOLUTE;
			m_memory.WriteByte(addr, m_y);
			break;
		case 0x8d: /* sta - abs */
			GET_ABSOLUTE; 
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x8e: /* stx - abs */
			GET_ABSOLUTE;
			m_memory.WriteByte(addr, m_x);
			break;
		case 0x90: /* bcc */
			DoBranch((m_flags & Flag_C) == 0, iCycles);
			break;
		case 0x91: /* sta - (ind),y */
			GET_INDIRECT_Y;
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x94: /* sty - zp,x */
			GET_ZEROPAGE_X;
			m_memory.WriteByte(addr, m_y);
			break;
		case 0x95: /* sta - zp,x */
			GET_ZEROPAGE_X;
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x96: /* stx - zp,y */
			GET_ZEROPAGE_Y;
			m_memory.WriteByte(addr, m_x);
			break;
		case 0x98: /* tya */
			m_a = m_y;
			UpdateNZ(m_a);
			break;
		case 0x99: /* sta - abs,y */
			GET_ABSOLUTE_Y;
			m_memory.WriteByte(addr, m_a);
			break;
		case 0x9a: /* txs */
			m_sp = Memory::s_stack_base | m_x;
			break;
		case 0x9d: /* sta - abs,x */
			GET_ABSOLUTE_X;
			m_memory.WriteByte(addr, m_a);
			break;
		case 0xa0: /* ldy - imm */
			m_y = GetImmediate();
			UpdateNZ(m_y);
			break;
		case 0xa1: /* lda - (ind,x) */
			m_a = GetIndirectX();
			UpdateNZ(m_a);
			break;
		case 0xa2: /* ldx - imm */
			m_x = GetImmediate();
			UpdateNZ(m_x);
			break;
		case 0xa4: /* ldy - zp */
			m_y = GetZeroPage();
			UpdateNZ(m_y);
			break;
		case 0xa5: /* lda - zp */
			m_a = GetZeroPage();
			UpdateNZ(m_a);
			break;
		case 0xa6: /* ldx - zp */
			m_x = GetZeroPage();
			UpdateNZ(m_x);
			break;
		case 0xa8: /* tay */
			m_y = m_a;
			UpdateNZ(m_y);
			break;
		case 0xa9: /* lda - imm */
			m_a = GetImmediate();
			UpdateNZ(m_a);
			break;
		case 0xaa: /* tax */
			m_x = m_a;
			UpdateNZ(m_x);
			break;
		case 0xac: /* ldy - abs */
			m_y = GetAbsolute();
			UpdateNZ(m_y);
			break;
		case 0xad: /* lda - abs */
			m_a = GetAbsolute();
			UpdateNZ(m_a);
			break;
		case 0xae: /* ldx - abs */
			m_x = GetAbsolute();
			UpdateNZ(m_x);
			break;
		case 0xb0: /* bcs */
			DoBranch((m_flags & Flag_C) != 0, iCycles);
			break;
		case 0xb1: /* lda - (ind),y */
			m_a = GetIndirectY(iCycles);
			UpdateNZ(m_a);
			break;
		case 0xb4: /* ldy - zp,x */
			m_y = GetZeroPageX();
			UpdateNZ(m_y);
			break;
		case 0xb5: /* lda - zp,x */
			m_a = GetZeroPageX();
			UpdateNZ(m_a);
			break;
		case 0xb6: /* ldx - zp,y */
			m_x = GetZeroPageY();
			UpdateNZ(m_x);
			break;
		case 0xb8: /* clv */
			m_flags &= ~Flag_V;
			break;
		case 0xb9: /* lda - abs,y */
			m_a = GetAbsoluteY(iCycles);
			UpdateNZ(m_a);
			break;
		case 0xba: /* tsx */
			m_x = m_sp & 0xff;
			UpdateNZ(m_x);
			break;
		case 0xbc: /* ldy - abs,x */
			m_y = GetAbsoluteX(iCycles);
			UpdateNZ(m_y);
			break;
		case 0xbd: /* lda - abs,x */
			m_a = GetAbsoluteX(iCycles);
			UpdateNZ(m_a);
			break;
		case 0xbe: /* ldx - abs,y */
			m_x = GetAbsoluteY(iCycles);
			UpdateNZ(m_a);
			break;
		case 0xc0: /* cpy - imm */
			DoCPY(GetImmediate());
			break;
		case 0xc1: /* cmp - (ind,x) */
			DoCMP(GetIndirectX());
			break;
		case 0xc4: /* cpy - zp */
			DoCPY(GetZeroPage());
			break;
		case 0xc5: /* cmp - zp */
			DoCMP(GetZeroPage());
			break;
		case 0xc6: /* dec - zp */
			DO_ZEROPAGE(DoDEC);
			break;
		case 0xc8: /* iny */
			DO_Y(DoINC);
			break;
		case 0xc9: /* cmp - imm */
			DoCMP(GetImmediate());
			break;
		case 0xca: /* dex */
			DO_X(DoDEC);
			break;
		case 0xcc: /* cpy - abs */
			DoCPY(GetAbsolute());
			break;
		case 0xcd: /* cmp - abs */
			DoCMP(GetAbsolute());
			break;
		case 0xce: /* dec - abs */
			DO_ABSOLUTE(DoDEC);
			break;
		case 0xd0: /* bne */
			DoBranch((m_flags & Flag_Z) == 0, iCycles);
			break;
		case 0xd1: /* cmp - (ind),y */
			DoCMP(GetIndirectY(iCycles));
			break;
		case 0xd5: /* cmp - zp,x */
			DoCMP(GetZeroPageX());
			break;
		case 0xd6: /* dec - zp,x */
			DO_ZEROPAGE_X(DoDEC);
			break;
		case 0xd8: /* cld */
			m_flags &= ~Flag_D;
			break;
		case 0xd9: /* cmp - abs,y */
			DoCMP(GetAbsoluteY(iCycles));
			break;
		case 0xdd: /* cmp - abs,x */
			DoCMP(GetAbsoluteX(iCycles));
			break;
		case 0xde: /* dec - abs,x */
			DO_ABSOLUTE_X(DoDEC);
			break;
		case 0xe0: /* cpx - imm */
			DoCPX(GetImmediate());
			break;
		case 0xe1: /* sbc - (ind,x) */
			DoSBC(GetIndirectX());
			break;
		case 0xe4: /* cpx - zp */
			DoCPX(GetZeroPage());
			break;
		case 0xe5: /* sbc - zp */
			DoSBC(GetZeroPage());
			break;
		case 0xe6: /* inc - zp */
			DO_ZEROPAGE(DoINC);
			break;
		case 0xe8: /* inx */
			DO_X(DoINC);
			break;
		case 0xe9: /* sbc - imm */
			DoSBC(GetImmediate());
			break;
		case 0xea: /* nop */
			break;
		case 0xec: /* cpx - abs */
			DoCPX(GetAbsolute());
			break;
		case 0xed: /* sbc - abs */
			DoSBC(GetAbsolute());
			break;
		case 0xee: /* inc - abs */
			DO_ABSOLUTE(DoINC);
			break;
		case 0xf0: /* beq */
			DoBranch((m_flags & Flag_Z) != 0, iCycles);
			break;
		case 0xf1: /* sbc - (ind),y */
			DoSBC(GetIndirectY(iCycles));
			break;
		case 0xf5: /* sbc - zp,x */
			DoSBC(GetZeroPageX());
			break;
		case 0xf6: /* inc - zp,x */
			DO_ZEROPAGE_X(DoINC);
			break;
		case 0xf8: /* sed */
			m_flags |= Flag_D;
			break;
		case 0xf9: /* sbc - abs,y */
			DoSBC(GetAbsoluteY(iCycles));
			break;
		case 0xfd: /* sbc - abs,x */
			DoSBC(GetAbsoluteX(iCycles));
			break;
		case 0xfe: /* inc - abs,x */
			DO_ABSOLUTE_X(DoINC);
			break;
		case 0x02: /* invalid */
		case 0x03: /* invalid */
		case 0x04: /* invalid */
		case 0x07: /* invalid */
		case 0x0b: /* invalid */
		case 0x0c: /* invalid */
		case 0x0f: /* invalid */
		case 0x12: /* invalid */
		case 0x13: /* invalid */
		case 0x14: /* invalid */
		case 0x17: /* invalid */
		case 0x1a: /* invalid */
		case 0x1b: /* invalid */
		case 0x1c: /* invalid */
		case 0x1f: /* invalid */
		case 0x22: /* invalid */
		case 0x23: /* invalid */
		case 0x27: /* invalid */
		case 0x2b: /* invalid */
		case 0x2f: /* invalid */
		case 0x32: /* invalid */
		case 0x33: /* invalid */
		case 0x34: /* invalid */
		case 0x37: /* invalid */
		case 0x3a: /* invalid */
		case 0x3b: /* invalid */
		case 0x3c: /* invalid */
		case 0x3f: /* invalid */
		case 0x42: /* invalid */
		case 0x43: /* invalid */
		case 0x44: /* invalid */
		case 0x47: /* invalid */
		case 0x4b: /* invalid */
		case 0x4f: /* invalid */
		case 0x52: /* invalid */
		case 0x53: /* invalid */
		case 0x54: /* invalid */
		case 0x57: /* invalid */
		case 0x5a: /* invalid */
		case 0x5b: /* invalid */
		case 0x5c: /* invalid */
		case 0x5f: /* invalid */
		case 0x67: /* invalid */
		case 0x6b: /* invalid */
		case 0x6f: /* invalid */
		case 0x72: /* invalid */
		case 0x73: /* invalid */
		case 0x74: /* invalid */
		case 0x77: /* invalid */
		case 0x7a: /* invalid */
		case 0x7b: /* invalid */
		case 0x7c: /* invalid */
		case 0x7f: /* invalid */
		case 0x80: /* invalid */
		case 0x82: /* invalid */
		case 0x83: /* invalid */
		case 0x87: /* invalid */
		case 0x89: /* invalid */
		case 0x8b: /* invalid */
		case 0x8f: /* invalid */
		case 0x92: /* invalid */
		case 0x93: /* invalid */
		case 0x97: /* invalid */
		case 0x9b: /* invalid */
		case 0x9c: /* invalid */
		case 0x9e: /* invalid */
		case 0x9f: /* invalid */
		case 0xa3: /* invalid */
		case 0xa7: /* invalid */
		case 0xab: /* invalid */
		case 0xaf: /* invalid */
		case 0xb2: /* invalid */
		case 0xb3: /* invalid */
		case 0xb7: /* invalid */
		case 0xbb: /* invalid */
		case 0xbf: /* invalid */
		case 0xc2: /* invalid */
		case 0xc3: /* invalid */
		case 0xc7: /* invalid */
		case 0xcb: /* invalid */
		case 0xcf: /* invalid */
		case 0xd2: /* invalid */
		case 0xd3: /* invalid */
		case 0xd4: /* invalid */
		case 0xd7: /* invalid */
		case 0xda: /* invalid */
		case 0xdb: /* invalid */
		case 0xdc: /* invalid */
		case 0xdf: /* invalid */
		case 0xe2: /* invalid */
		case 0xe3: /* invalid */
		case 0xe7: /* invalid */
		case 0xeb: /* invalid */
		case 0xef: /* invalid */
		case 0xf2: /* invalid */
		case 0xf3: /* invalid */
		case 0xf4: /* invalid */
		case 0xf7: /* invalid */
		case 0xfa: /* invalid */
		case 0xfb: /* invalid */
		case 0xfc: /* invalid */
		case 0xff: /* invalid */
			break;
	}

#undef DO_ABSOLUTE_X
#undef DO_ABSOLUTE
#undef DO_ZEROPAGE_X
#undef DO_ZEROPAGE
#undef GET_INDIRECT_Y
#undef GET_INDIRECT_X
#undef GET_ABSOLUTE_Y
#undef GET_ABSOLUTE_X
#undef GET_ABSOLUTE
#undef GET_ZEROPAGE_Y
#undef GET_ZEROPAGE_X
#undef GET_ZEROPAGE
#undef DO_Y
#undef DO_X
#undef DO_ACC

	if (m_nmi_pending) {
		// IRQ is pending; handle it on the next instruction
		Memory::addr_t addr = m_memory.ReadWord(Memory::s_vector_nmi);
		PushWord(m_pc);
		PushByte(m_flags);
		m_pc = addr;
		m_nmi_pending = false;
	} else if (m_irq_pending && (m_flags & Flag_I) == 0) {
		// IRQ is pending; handle it on the next instruction
		Memory::addr_t addr = m_memory.ReadWord(Memory::s_vector_irq);
		PushWord(m_pc);
		PushByte(m_flags);
		m_pc = addr;
		m_irq_pending = false;
	}

	assert((m_flags & Flag_D) == 0);
	return iCycles;
}

inline uint8_t CPU6502::ReadByteAtPC()
{
	uint8_t val = m_memory.ReadByte(m_pc);
	m_pc++;
	return val;
}

inline uint16_t CPU6502::ReadWordAtPC()
{
	uint16_t address = ReadByteAtPC();
	address |= (uint16_t)ReadByteAtPC() << 8;
	return address;
}

inline void CPU6502::PushByte(uint8_t val)
{
	m_memory.WriteByte(m_sp, val);
	m_sp--;
}

inline void CPU6502::PushWord(uint16_t val)
{
	PushByte(((val >> 8)&0xFF));
	PushByte(val & 0xff);
}

inline uint8_t CPU6502::PopByte()
{
	m_sp++;
	return m_memory.ReadByte(m_sp);
}

inline uint16_t CPU6502::PopWord()
{
	uint16_t val = PopByte();
	val |= (uint16_t)PopByte() << 8;
	return val;
}

inline void CPU6502::UpdateNZ(uint8_t val)
{
	m_flags &= ~(Flag_N | Flag_Z);
	if (val & 0x80)
		m_flags |= Flag_N;
	if (!val)
		m_flags |= Flag_Z;
}

inline uint8_t CPU6502::GetImmediate()
{
	return ReadByteAtPC();
}

inline uint8_t CPU6502::GetZeroPage()
{
	return m_memory.ReadByte(ReadByteAtPC());
}

inline uint8_t CPU6502::GetZeroPageX() /* zero page indexed with x: zp,x */
{
	return m_memory.ReadByte((ReadByteAtPC() + m_x) & 0xff);
}

inline uint8_t CPU6502::GetZeroPageY() /* zero page indexed with x: zp,y */
{
	return m_memory.ReadByte((ReadByteAtPC() + m_y) & 0xff);
}

inline uint8_t CPU6502::GetAbsolute() /* absolute: a */
{
	return m_memory.ReadByte(ReadWordAtPC());
}

inline uint8_t CPU6502::GetAbsoluteX(int& iCycles) /* absolute indexed with x: a,x */
{
	(void)iCycles; /* TODO What is a page boundary? */
	uint16_t address = ReadWordAtPC() + m_x;
	return m_memory.ReadByte(address);
}

inline uint8_t CPU6502::GetAbsoluteY(int& iCycles) /* absolute indexed with y: a,y */
{
	(void)iCycles; /* TODO What is a page boundary? */
	uint16_t address = ReadWordAtPC() + m_y;
	return m_memory.ReadByte(address);
}

inline uint8_t CPU6502::GetIndirectX() /* indexed indirect: (zp,x) */
{
	uint16_t address = m_memory.ReadWord((ReadByteAtPC() + m_x) & 0xff);
	return m_memory.ReadByte(address);
}

inline uint8_t CPU6502::GetIndirectY(int& iCycles) /* indirect indexed: (zp),y */
{
	(void)iCycles; /* TODO What is a page boundary? */
	uint16_t address = m_memory.ReadWord(ReadByteAtPC()) + m_y;
	return m_memory.ReadByte(address);
}

inline void CPU6502::SetCarry(bool set)
{
	if (set)
		m_flags |= Flag_C;
	else
		m_flags &= ~Flag_C;
}

inline void CPU6502::DoADC(uint8_t val)
{
	int16_t result = m_a + val + ((m_flags & Flag_C) ? 1 : 0);
	SetCarry(result & 0x100);
	if (result < -128 || result > 128)
		m_flags |= Flag_V;
	else
		m_flags &= ~Flag_V;
	m_a = result & 0xff;
	UpdateNZ(m_a);
}

inline void CPU6502::DoAND(uint8_t val)
{
	m_a &= val;
	UpdateNZ(m_a);
}

inline void CPU6502::DoORA(uint8_t val)
{
	m_a |= val;
	UpdateNZ(m_a);
}

inline uint8_t CPU6502::DoASL(uint8_t val)
{
	uint8_t result = val << 1;
	SetCarry(val & 0x80);
	UpdateNZ(result);
	return result;
}

inline void CPU6502::DoBIT(uint8_t imm)
{
	uint8_t result = m_a & imm;
	m_flags &= ~(Flag_N | Flag_Z | Flag_V);
	if (imm & 0x80)
		m_flags |= Flag_N;
	if (imm & 0x40)
		m_flags |= Flag_V;
	if (result == 0)
		m_flags |= Flag_Z;
}

inline uint8_t CPU6502::DoROL(uint8_t imm)
{
	uint8_t result = (imm << 1) | ((m_flags & Flag_C) ? 1 : 0);
	SetCarry(imm & 0x80);
	UpdateNZ(result);
	return result;
}

inline void CPU6502::DoEOR(uint8_t eor)
{
	m_a ^= eor;
	UpdateNZ(m_a);
}

inline uint8_t CPU6502::DoLSR(uint8_t imm)
{
	uint8_t result = imm >> 1;
	SetCarry(imm & 1);
	UpdateNZ(result); /* N is always zero */
	return result;
}

inline uint8_t CPU6502::DoROR(uint8_t imm)
{
	uint8_t result = ((m_flags & Flag_C) ? 0x80 : 0) | (imm >> 1);
	SetCarry(imm & 1);
	UpdateNZ(result);
	return result;
}

inline uint8_t CPU6502::DoDEC(uint8_t imm)
{
	uint8_t result = imm - 1;
	UpdateNZ(result);
	return result;
}

inline uint8_t CPU6502::DoINC(uint8_t imm)
{
	uint8_t result = imm + 1;
	UpdateNZ(result);
	return result;
}

inline void CPU6502::DoCompare(uint8_t src, uint8_t imm)
{
	uint8_t result = src - imm;
	SetCarry(src >= imm);
	UpdateNZ(result);
}

inline void CPU6502::DoBranch(bool take, int& iCycles)
{
	int8_t val = (int8_t)ReadByteAtPC();

	/* XXX What is a page? */
	iCycles++;
	if (take)
		m_pc += (int)val;
}

inline void CPU6502::DoSBC(uint8_t imm)
{
	int16_t result = m_a - imm - ((m_flags & Flag_C) ? 0 : 1);
	SetCarry(result >= 0);
	if (result < -128 || result > 128)
		m_flags |= Flag_V;
	else
		m_flags &= ~Flag_V;
	m_a = (uint16_t)result & 0xff;
	UpdateNZ(m_a);
}

void CPU6502::Dump()
{
 #ifdef use_lib_log_serial
	printf("a:%02x x:%02x y:%02x pc:%04x sp:%04x flags:", m_a, m_x, m_y, m_pc, m_sp);
	printf("%c%c%c%c%c%c%c%c\n",
		(m_flags & Flag_N) ? 'N' : '.',
		(m_flags & Flag_V) ? 'V' : '.',
		(m_flags & Flag_1) ? '1' : '.',
		(m_flags & Flag_B) ? 'B' : '.',
		(m_flags & Flag_D) ? 'D' : '.',
		(m_flags & Flag_I) ? 'I' : '.',
		(m_flags & Flag_Z) ? 'Z' : '.',
		(m_flags & Flag_C) ? 'C' : '.');
 #endif		
}

/* vim:set ts=2 sw=2: */
