#include "Z80Instruction.h"
#ifndef __Z80_RRA_H__
#define __Z80_RRA_H__
namespace Z80 {

class RRA :public Z80Instruction
{
public:
	virtual RRA* Clone() {return new RRA();}
	virtual RRA* ClonePlacement(void* buffer) {return new(buffer) RRA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00011111", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"RRA", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);
		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = (op1 >> 1);
		result.SetBit(result.GetBitCount() - 1, cpu->GetFlagC());
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.GetBit(0));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
