/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42): Rink Springer <rink@rink.nu> wrote
 * this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return Rink Springer
 * ----------------------------------------------------------------------------
 */
#include "info6502.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "cpu6502.h"
#include "memory.h"

Info6502::Info6502(CPU6502& oCPU):m_cpu(oCPU)
{
	m_opcode.push_back(/* 00 */ OpcodeInfo("brk", 7, OA_NONE));
	m_opcode.push_back(/* 01 */ OpcodeInfo("ora", 6, OA_IND_X));
	m_opcode.push_back(/* 02 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 03 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 04 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 05 */ OpcodeInfo("ora", 3, OA_ZP));
	m_opcode.push_back(/* 06 */ OpcodeInfo("asl", 5, OA_ZP));
	m_opcode.push_back(/* 07 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 08 */ OpcodeInfo("php", 3, OA_NONE));
	m_opcode.push_back(/* 09 */ OpcodeInfo("ora", 2, OA_IMM));
	m_opcode.push_back(/* 0a */ OpcodeInfo("asl", 2, OA_ACC));
	m_opcode.push_back(/* 0b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 0c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 0d */ OpcodeInfo("ora", 4, OA_ABS));
	m_opcode.push_back(/* 0e */ OpcodeInfo("asl", 6, OA_ABS));
	m_opcode.push_back(/* 0f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 10 */ OpcodeInfo("bpl", 2, OA_REL));
	m_opcode.push_back(/* 11 */ OpcodeInfo("ora", 5, OA_IND_Y));
	m_opcode.push_back(/* 12 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 13 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 14 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 15 */ OpcodeInfo("ora", 4, OA_ZP_X));
	m_opcode.push_back(/* 16 */ OpcodeInfo("asl", 6, OA_ZP_X));
	m_opcode.push_back(/* 17 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 18 */ OpcodeInfo("clc", 2, OA_NONE));
	m_opcode.push_back(/* 19 */ OpcodeInfo("ora", 4, OA_ABS_Y));
	m_opcode.push_back(/* 1a */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 1b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 1c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 1d */ OpcodeInfo("ora", 4, OA_ABS_X));
	m_opcode.push_back(/* 1e */ OpcodeInfo("asl", 7, OA_ABS_X));
	m_opcode.push_back(/* 1f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 20 */ OpcodeInfo("jsr", 6, OA_ABS));
	m_opcode.push_back(/* 21 */ OpcodeInfo("and", 6, OA_IND_X));
	m_opcode.push_back(/* 22 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 23 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 24 */ OpcodeInfo("bit", 3, OA_ZP));
	m_opcode.push_back(/* 25 */ OpcodeInfo("and", 3, OA_ZP));
	m_opcode.push_back(/* 26 */ OpcodeInfo("rol", 5, OA_ZP));
	m_opcode.push_back(/* 27 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 28 */ OpcodeInfo("plp", 4, OA_NONE));
	m_opcode.push_back(/* 29 */ OpcodeInfo("and", 2, OA_IMM));
	m_opcode.push_back(/* 2a */ OpcodeInfo("rol", 2, OA_ACC));
	m_opcode.push_back(/* 2b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 2c */ OpcodeInfo("bit", 4, OA_ABS));
	m_opcode.push_back(/* 2d */ OpcodeInfo("and", 4, OA_ABS));
	m_opcode.push_back(/* 2e */ OpcodeInfo("rol", 6, OA_ABS));
	m_opcode.push_back(/* 2f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 30 */ OpcodeInfo("bmi", 2, OA_REL));
	m_opcode.push_back(/* 31 */ OpcodeInfo("and", 5, OA_IND_Y));
	m_opcode.push_back(/* 32 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 33 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 34 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 35 */ OpcodeInfo("and", 4, OA_ZP_X));
	m_opcode.push_back(/* 36 */ OpcodeInfo("rol", 6, OA_ZP_X));
	m_opcode.push_back(/* 37 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 38 */ OpcodeInfo("sec", 2, OA_NONE));
	m_opcode.push_back(/* 39 */ OpcodeInfo("and", 3, OA_ABS_Y));
	m_opcode.push_back(/* 3a */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 3b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 3c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 3d */ OpcodeInfo("and", 4, OA_ABS_X));
	m_opcode.push_back(/* 3e */ OpcodeInfo("rol", 7, OA_ABS_X));
	m_opcode.push_back(/* 3f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 40 */ OpcodeInfo("rti", 6, OA_NONE));
	m_opcode.push_back(/* 41 */ OpcodeInfo("eor", 6, OA_IND_X));
	m_opcode.push_back(/* 42 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 43 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 44 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 45 */ OpcodeInfo("eor", 3, OA_ZP));
	m_opcode.push_back(/* 46 */ OpcodeInfo("lsr", 5, OA_ZP));
	m_opcode.push_back(/* 47 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 48 */ OpcodeInfo("pha", 3, OA_NONE));
	m_opcode.push_back(/* 49 */ OpcodeInfo("eor", 2, OA_IMM));
	m_opcode.push_back(/* 4a */ OpcodeInfo("lsr", 2, OA_ACC));
	m_opcode.push_back(/* 4b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 4c */ OpcodeInfo("jmp", 3, OA_ABS));
	m_opcode.push_back(/* 4d */ OpcodeInfo("eor", 4, OA_ABS));
	m_opcode.push_back(/* 4e */ OpcodeInfo("lsr", 6, OA_ABS));
	m_opcode.push_back(/* 4f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 50 */ OpcodeInfo("bvc", 2, OA_REL));
	m_opcode.push_back(/* 51 */ OpcodeInfo("eor", 5, OA_IND_Y));
	m_opcode.push_back(/* 52 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 53 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 54 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 55 */ OpcodeInfo("eor", 4, OA_ZP_X));
	m_opcode.push_back(/* 56 */ OpcodeInfo("lsr", 6, OA_ZP_X));
	m_opcode.push_back(/* 57 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 58 */ OpcodeInfo("cli", 2, OA_NONE));
	m_opcode.push_back(/* 59 */ OpcodeInfo("eor", 4, OA_ABS_Y));
	m_opcode.push_back(/* 5a */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 5b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 5c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 5d */ OpcodeInfo("eor", 4, OA_ABS_X));
	m_opcode.push_back(/* 5e */ OpcodeInfo("lsr", 7, OA_ABS_X));
	m_opcode.push_back(/* 5f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 60 */ OpcodeInfo("rts", 6, OA_NONE));
	m_opcode.push_back(/* 61 */ OpcodeInfo("adc", 6, OA_IND_X));
	m_opcode.push_back(/* 62 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 63 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 64 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 65 */ OpcodeInfo("adc", 3, OA_ZP));
	m_opcode.push_back(/* 66 */ OpcodeInfo("ror", 5, OA_ZP));
	m_opcode.push_back(/* 67 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 68 */ OpcodeInfo("pla", 4, OA_NONE));
	m_opcode.push_back(/* 69 */ OpcodeInfo("adc", 2, OA_IMM));
	m_opcode.push_back(/* 6a */ OpcodeInfo("ror", 2, OA_ACC));
	m_opcode.push_back(/* 6b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 6c */ OpcodeInfo("jmp", 5, OA_IND));
	m_opcode.push_back(/* 6d */ OpcodeInfo("adc", 4, OA_ABS));
	m_opcode.push_back(/* 6e */ OpcodeInfo("ror", 6, OA_ABS));
	m_opcode.push_back(/* 6f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 70 */ OpcodeInfo("bvs", 2, OA_REL));
	m_opcode.push_back(/* 71 */ OpcodeInfo("adc", 5, OA_IND_Y));
	m_opcode.push_back(/* 72 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 73 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 74 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 75 */ OpcodeInfo("adc", 4, OA_ZP_X));
	m_opcode.push_back(/* 76 */ OpcodeInfo("ror", 6, OA_ZP_X));
	m_opcode.push_back(/* 77 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 78 */ OpcodeInfo("sei", 2, OA_NONE));
	m_opcode.push_back(/* 79 */ OpcodeInfo("adc", 4, OA_ABS_Y));
	m_opcode.push_back(/* 7a */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 7b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 7c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 7d */ OpcodeInfo("adc", 4, OA_ABS_X));
	m_opcode.push_back(/* 7e */ OpcodeInfo("ror", 7, OA_ABS_X));
	m_opcode.push_back(/* 7f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 80 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 81 */ OpcodeInfo("sta", 6, OA_IND_X));
	m_opcode.push_back(/* 82 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 83 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 84 */ OpcodeInfo("sty", 3, OA_ZP));
	m_opcode.push_back(/* 85 */ OpcodeInfo("sta", 3, OA_ZP));
	m_opcode.push_back(/* 86 */ OpcodeInfo("stx", 3, OA_ZP));
	m_opcode.push_back(/* 87 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 88 */ OpcodeInfo("dey", 2, OA_NONE));
	m_opcode.push_back(/* 89 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 8a */ OpcodeInfo("txa", 2, OA_NONE));
	m_opcode.push_back(/* 8b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 8c */ OpcodeInfo("sty", 4, OA_ABS));
	m_opcode.push_back(/* 8d */ OpcodeInfo("sta", 4, OA_ABS));
	m_opcode.push_back(/* 8e */ OpcodeInfo("stx", 4, OA_ABS));
	m_opcode.push_back(/* 8f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 90 */ OpcodeInfo("bcc", 2, OA_REL));
	m_opcode.push_back(/* 91 */ OpcodeInfo("sta", 6, OA_IND_Y));
	m_opcode.push_back(/* 92 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 93 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 94 */ OpcodeInfo("sty", 4, OA_ZP_X));
	m_opcode.push_back(/* 95 */ OpcodeInfo("sta", 4, OA_ZP_X));
	m_opcode.push_back(/* 96 */ OpcodeInfo("stx", 4, OA_ZP_Y));
	m_opcode.push_back(/* 97 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 98 */ OpcodeInfo("tya", 2, OA_NONE));
	m_opcode.push_back(/* 99 */ OpcodeInfo("sta", 5, OA_ABS_Y));
	m_opcode.push_back(/* 9a */ OpcodeInfo("txs", 2, OA_NONE));
	m_opcode.push_back(/* 9b */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 9c */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 9d */ OpcodeInfo("sta", 5, OA_ABS_X));
	m_opcode.push_back(/* 9e */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* 9f */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* a0 */ OpcodeInfo("ldy", 2, OA_IMM));
	m_opcode.push_back(/* a1 */ OpcodeInfo("lda", 6, OA_IND_X));
	m_opcode.push_back(/* a2 */ OpcodeInfo("ldx", 2, OA_IMM));
	m_opcode.push_back(/* a3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* a4 */ OpcodeInfo("ldy", 3, OA_ZP));
	m_opcode.push_back(/* a5 */ OpcodeInfo("lda", 3, OA_ZP));
	m_opcode.push_back(/* a6 */ OpcodeInfo("ldx", 3, OA_ZP));
	m_opcode.push_back(/* a7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* a8 */ OpcodeInfo("tay", 2, OA_NONE));
	m_opcode.push_back(/* a9 */ OpcodeInfo("lda", 2, OA_IMM));
	m_opcode.push_back(/* aa */ OpcodeInfo("tax", 2, OA_NONE));
	m_opcode.push_back(/* ab */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* ac */ OpcodeInfo("ldy", 4, OA_ABS));
	m_opcode.push_back(/* ad */ OpcodeInfo("lda", 4, OA_ABS));
	m_opcode.push_back(/* ae */ OpcodeInfo("ldx", 4, OA_ABS));
	m_opcode.push_back(/* af */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* b0 */ OpcodeInfo("bcs", 2, OA_REL));
	m_opcode.push_back(/* b1 */ OpcodeInfo("lda", 5, OA_IND_Y));
	m_opcode.push_back(/* b2 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* b3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* b4 */ OpcodeInfo("ldy", 4, OA_ZP_X));
	m_opcode.push_back(/* b5 */ OpcodeInfo("lda", 4, OA_ZP_X));
	m_opcode.push_back(/* b6 */ OpcodeInfo("ldx", 4, OA_ZP_Y));
	m_opcode.push_back(/* b7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* b8 */ OpcodeInfo("clv", 2, OA_NONE));
	m_opcode.push_back(/* b9 */ OpcodeInfo("lda", 4, OA_ABS_Y));
	m_opcode.push_back(/* ba */ OpcodeInfo("tsx", 2, OA_NONE));
	m_opcode.push_back(/* bb */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* bc */ OpcodeInfo("ldy", 4, OA_ABS_X));
	m_opcode.push_back(/* bd */ OpcodeInfo("lda", 4, OA_ABS_X));
	m_opcode.push_back(/* be */ OpcodeInfo("ldx", 4, OA_ABS_Y));
	m_opcode.push_back(/* bf */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* c0 */ OpcodeInfo("cpy", 2, OA_IMM));
	m_opcode.push_back(/* c1 */ OpcodeInfo("cmp", 6, OA_IND_X));
	m_opcode.push_back(/* c2 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* c3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* c4 */ OpcodeInfo("cpy", 3, OA_ZP));
	m_opcode.push_back(/* c5 */ OpcodeInfo("cmp", 3, OA_ZP));
	m_opcode.push_back(/* c6 */ OpcodeInfo("dec", 5, OA_ZP));
	m_opcode.push_back(/* c7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* c8 */ OpcodeInfo("iny", 2, OA_NONE));
	m_opcode.push_back(/* c9 */ OpcodeInfo("cmp", 2, OA_IMM));
	m_opcode.push_back(/* ca */ OpcodeInfo("dex", 2, OA_NONE));
	m_opcode.push_back(/* cb */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* cc */ OpcodeInfo("cpy", 4, OA_ABS));
	m_opcode.push_back(/* cd */ OpcodeInfo("cmp", 4, OA_ABS));
	m_opcode.push_back(/* ce */ OpcodeInfo("dec", 6, OA_ABS));
	m_opcode.push_back(/* cf */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* d0 */ OpcodeInfo("bne", 2, OA_REL));
	m_opcode.push_back(/* d1 */ OpcodeInfo("cmp", 5, OA_IND_Y));
	m_opcode.push_back(/* d2 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* d3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* d4 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* d5 */ OpcodeInfo("cmp", 4, OA_ZP_X));
	m_opcode.push_back(/* d6 */ OpcodeInfo("dec", 6, OA_ZP_X));
	m_opcode.push_back(/* d7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* d8 */ OpcodeInfo("cld", 2, OA_NONE));
	m_opcode.push_back(/* d9 */ OpcodeInfo("cmp", 4, OA_ABS_Y));
	m_opcode.push_back(/* da */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* db */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* dc */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* dd */ OpcodeInfo("cmp", 4, OA_ABS_X));
	m_opcode.push_back(/* de */ OpcodeInfo("dec", 7, OA_ABS_X));
	m_opcode.push_back(/* df */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* e0 */ OpcodeInfo("cpx", 2, OA_IMM));
	m_opcode.push_back(/* e1 */ OpcodeInfo("sbc", 6, OA_IND_X));
	m_opcode.push_back(/* e2 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* e3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* e4 */ OpcodeInfo("cpx", 3, OA_ZP));
	m_opcode.push_back(/* e5 */ OpcodeInfo("sbc", 3, OA_ZP));
	m_opcode.push_back(/* e6 */ OpcodeInfo("inc", 5, OA_ZP));
	m_opcode.push_back(/* e7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* e8 */ OpcodeInfo("inx", 2, OA_NONE));
	m_opcode.push_back(/* e9 */ OpcodeInfo("sbc", 2, OA_IMM));
	m_opcode.push_back(/* ea */ OpcodeInfo("nop", 2, OA_NONE));
	m_opcode.push_back(/* eb */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* ec */ OpcodeInfo("cpx", 4, OA_ABS));
	m_opcode.push_back(/* ed */ OpcodeInfo("sbc", 4, OA_ABS));
	m_opcode.push_back(/* ee */ OpcodeInfo("inc", 6, OA_ABS));
	m_opcode.push_back(/* ef */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* f0 */ OpcodeInfo("beq", 2, OA_REL));
	m_opcode.push_back(/* f1 */ OpcodeInfo("sbc", 5, OA_IND_Y));
	m_opcode.push_back(/* f2 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* f3 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* f4 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* f5 */ OpcodeInfo("sbc", 4, OA_ZP_X));
	m_opcode.push_back(/* f6 */ OpcodeInfo("inc", 6, OA_ZP_X));
	m_opcode.push_back(/* f7 */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* f8 */ OpcodeInfo("sed", 2, OA_NONE));
	m_opcode.push_back(/* f9 */ OpcodeInfo("sbc", 4, OA_ABS_Y));
	m_opcode.push_back(/* fa */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* fb */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* fc */ OpcodeInfo("???", 0, OA_NONE));
	m_opcode.push_back(/* fd */ OpcodeInfo("sbc", 4, OA_ABS_X));
	m_opcode.push_back(/* fe */ OpcodeInfo("inc", 7, OA_ABS_X));
	m_opcode.push_back(/* ff */ OpcodeInfo("???", 0, OA_NONE));
	assert(m_opcode.size() == 256);
}

int Info6502::GetCyclesForOpcode(uint8_t opcode) const
{
	return m_opcode[opcode].GetCycles();
}

void Info6502::Disassemble(char* output) const
{
	Disassemble(output, m_cpu.m_pc);
}

int Info6502::Disassemble(char* output, uint16_t addr) const
{
	uint16_t old_addr = addr;
	char arg[64];

	// Look up the opcode
	uint8_t opcode = m_cpu.m_memory.ReadByte(addr);
	const OpcodeInfo& oInfo = m_opcode[opcode];
	addr++;

#define GET_BYTE \
	val = m_cpu.m_memory.ReadByte(addr); \
	addr++

#define GET_WORD \
	val = m_cpu.m_memory.ReadWord(addr); \
	addr += 2

	// Resolve arguments
	*arg = '\0';
	uint16_t val;
	switch(oInfo.GetArg()) {
		case OA_NONE:
			break;
		case OA_ACC:
			strcpy(arg, "a");
			break;
		case OA_IMM:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "#$%x", val);
			break;
		case OA_ZP:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "$%x", val);
			break;
		case OA_ZP_X:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "$%x,x", val);
			break;
		case OA_ZP_Y:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "$%x,y", val);
			break;
		case OA_ABS:
			GET_WORD;
			snprintf(arg, sizeof(arg), "$%x", val);
			break;
		case OA_ABS_X:
			GET_WORD;
			snprintf(arg, sizeof(arg), "$%x,x", val);
			break;
		case OA_ABS_Y:
			GET_WORD;
			snprintf(arg, sizeof(arg), "$%x,y", val);
			break;
		case OA_IND:
			GET_WORD;
			snprintf(arg, sizeof(arg), "($%x)", val);
			break;
		case OA_IND_X:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "($%x,x)", val);
			break;
		case OA_IND_Y:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "($%x),y", val);
			break;
		case OA_REL:
			GET_BYTE;
			snprintf(arg, sizeof(arg), "$%x", addr + val);
			break;
		default:
			assert(0);
	}

	// First of all, place the address in the output
	sprintf(output, "%04x  ", old_addr);

	// Prepend the hex bytes
	int n = 0;
	char tmp[16];
	while (n < addr - old_addr) {
		snprintf(tmp, sizeof(tmp), "%02x ", m_cpu.m_memory.ReadByte(old_addr + n));
		strcat(output, tmp);
		n++;
	}

	// Align to 4
	while (n < 4) {
		strcat(output, "   ");
		n++;
	}

	// Append the instruction
	strcat(output, oInfo.GetName());

	// And the argument, if any
	if (*arg != '\0') {
		strcat(output, " ");
		strcat(output, arg);
	}
	return addr - old_addr;
}

/* vim:set ts=2 sw=2: */
