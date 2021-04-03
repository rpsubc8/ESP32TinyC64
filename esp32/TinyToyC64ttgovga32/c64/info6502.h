#ifndef __INFO6502_H__
#define __INFO6502_H__

#include <stdint.h>
#include <vector>

class CPU6502;

class Info6502
{
public:
	Info6502(CPU6502& oCPU);

	/*! \brief Retrieves the number of cycles needed for a given opcode
	 *  \param opcode Opcode to retrieve for
	 *  \returns Number of cycles
	 *
	 *  Note that this function cannot take page boundaries, branch
	 *  conditions etc into account; these must be added manually.
	 */
	int GetCyclesForOpcode(uint8_t opcode) const;

	/*! \brief Dissembles a given address
	 *  \param output Output buffer	
	 *  \param addr Address to disassemble
	 *  \returns Number of bytes processed
	 */
	int Disassemble(char* output, uint16_t addr) const;

	/*! \brief Dissembles the current instruction
	 *  \param output Output buffer	
	 */
	void Disassemble(char* output) const;

protected:
	enum OpcodeArg {
		OA_NONE,
		OA_ACC, 	// accumulator 
		OA_IMM, 	// immediate
		OA_ZP,		// zeropage
		OA_ZP_X,	// zeropage,x
		OA_ZP_Y,	// zeropage,y
		OA_ABS,		// absolute
		OA_ABS_X,	// absolute,x
		OA_ABS_Y,	// absolute,y
		OA_IND,		// (indirect)
		OA_IND_X,	// (indirect,x)
		OA_IND_Y,	// (indirect),y
		OA_REL		// relative
	};

	class OpcodeInfo {
	public:
		OpcodeInfo(const char* name, int cycles, OpcodeArg arg)
		 : m_name(name), m_cycles(cycles), m_arg(arg)
		{
		}

		const char* GetName() const { return m_name; }
		int GetCycles() const { return m_cycles; }
		OpcodeArg GetArg() const { return m_arg; }	
	private:
		const char* m_name;
		int m_cycles;
		OpcodeArg m_arg;
	};

	typedef std::vector<OpcodeInfo> TOpcodeInfoVector;
	TOpcodeInfoVector m_opcode;

	CPU6502& m_cpu;
};

#endif /* __INFO6502_H__ */
