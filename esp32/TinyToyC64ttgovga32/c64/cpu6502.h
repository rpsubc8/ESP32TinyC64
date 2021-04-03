#ifndef __CPU6502_H__
#define __CPU6502_H__

#include <stdint.h>

#include "info6502.h"

class Memory;

class CPU6502
{
	friend class Info6502; // so that it can access registers
public:
	CPU6502(Memory& oMemory);

	//! \brief Resets the 6502
	void Reset();

	/*! \brief Executes a single instruction
	 *  \returns Number of clock cycles consumed
	 */
	int RunOpcode();

	//! \brief Display 6502 status
	void Dump();

	//! \brief Signals the normal BRK/IRQ code
	void SignalIRQ() { m_irq_pending = true; }

	//! \brief Signals the NMI code
	void SignalNMI() { m_nmi_pending = true; }

	/*! \brief Enable or disable instruction tracing
	 *  \param bEnable Whether to enable tracing
	 */
	void SetTrace(bool bEnable) { m_tracing = bEnable; }
	void AssignPtrMemory6502(uint8_t * auxPtr);

protected:
	static const int Flag_C = (1 << 0);
	static const int Flag_Z = (1 << 1);
	static const int Flag_I = (1 << 2);
	static const int Flag_D = (1 << 3);
	static const int Flag_B = (1 << 4);
	static const int Flag_1 = (1 << 5); /* Always 1 */
	static const int Flag_V = (1 << 6);
	static const int Flag_N = (1 << 7);

	//! \brief Performs ADC (add with carry)
	void DoADC(uint8_t imm);

	//! \brief Performs AND
	void DoAND(uint8_t imm);

	//! \brief Performs ORA
	void DoORA(uint8_t imm);

	//! \brief Performs ASL and returns the result
	uint8_t DoASL(uint8_t imm);

	//! \brief Performs BIT
	void DoBIT(uint8_t imm);

	//! \brief Performs ROL and returns the result
	uint8_t DoROL(uint8_t imm);

	//! \brief Performs EOR (exclusive OR)
	void DoEOR(uint8_t imm);

	//! \brief Performs LSR and returns the result
	uint8_t DoLSR(uint8_t imm);

	//! \brief Performs ROR and returns the result
	uint8_t DoROR(uint8_t imm);

	//! \brief Performs decrement and returns the result
	uint8_t DoDEC(uint8_t imm);

	//! \brief Performs increment and returns the result
	uint8_t DoINC(uint8_t imm);

	//! \brief Performs compare
	void DoCompare(uint8_t src, uint8_t imm);

	//! \brief Performs CMP
	inline void DoCMP(uint8_t imm) { DoCompare(m_a, imm); }

	//! \brief Performs CPX
	inline void DoCPX(uint8_t imm) { DoCompare(m_x, imm); }

	//! \brief Performs CPY
	inline void DoCPY(uint8_t imm) { DoCompare(m_y, imm); }

	//! \brief Handles a branch instruction
	void DoBranch(bool take, int& iCycles);

	//! \birief Performs SBC (subtract borrow carry)
	void DoSBC(uint8_t imm);

	//! \brief Updates the overflow and zero flags
	void UpdateNZ(uint8_t val);

	//! \brief Sets or clears the carry flag
	void SetCarry(bool set);

private:
	//! \brief Retrieve the immediate operand
	uint8_t GetImmediate();

	//! \brief Retrieve the zero page operand contents
	uint8_t GetZeroPage();

	//! \brief Retrieve the zero page,x operand contents
	uint8_t GetZeroPageX();

	//! \brief Retrieve the zero page,y operand contents
	uint8_t GetZeroPageY();

	//! \brief Retrieve the absolute operand contents
	uint8_t GetAbsolute();

	//! \brief Retrieve the absolute,x operand contents
	uint8_t GetAbsoluteX(int& iCycles);

	//! \brief Retrieve the absolute,y operand contents
	uint8_t GetAbsoluteY(int& iCycles);
	
	//! \brief Retrieve the (indirect,x) operand contents
	uint8_t GetIndirectX();

	//! \brief Retrieve the (indirect),y operand contents
	uint8_t GetIndirectY(int& iCycles);

	//! \brief Reads the next byte at the PC and increments it
	uint8_t ReadByteAtPC();

	//! \brief Reads the next word at the PC and increments it
	uint16_t ReadWordAtPC();

	//! \brief Pushes a byte on the stack
	void PushByte(uint8_t val);

	//! \brief Pushes a word on the stack
	void PushWord(uint16_t val);

	//! \brief Pops a byte from the stack
	uint8_t PopByte();

	//! \brief Pops a word from the stack
	uint16_t PopWord();

	//! \brief Memory used
	Memory& m_memory;

	//! \brief Flags register
	uint8_t m_flags;

	//! \brief Accumulator register
	uint8_t m_a;

	//! \brief X register
	uint8_t m_x;

	//! \brief Y register
	uint8_t m_y;

	//! \brief Stack pointer
	uint16_t m_sp;

	//! \brief Program counter
	uint16_t m_pc;

	//! \brief Is an IRQ pending
	bool m_irq_pending;

	//! \brief Is an NMI pending
	bool m_nmi_pending;

	//! \brief Are we tracing execution?
	bool m_tracing;

	//! \brief Information provider, for clocks and disassembly
	Info6502 m_info;
};

#endif /* __CPU6502_H__ */
