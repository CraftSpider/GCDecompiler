#pragma once

#include <string>
#include <set>

#include "register.h"

using std::string;

namespace PPC {

	class Instruction {

	protected:

		string name, pattern;
		std::set<Register> *used = nullptr, *sources = nullptr;
		Register *destination = nullptr;
		char *instruction = nullptr, type;

	public:

		Instruction();
		Instruction(const Instruction &inst);
		Instruction(int type, char *instruction);
		~Instruction();

		void set_instruction(char *instruction);
		virtual string code_name();
		virtual string get_variables();

		virtual std::set<Register> used_registers();
		virtual Register destination_register();
		virtual std::set<Register> source_registers();

	};

	class ConditionInstruction : public Instruction {

	public:

		ConditionInstruction(int type, char *instruction);

	};

	class Ori : public Instruction {

	public:

		Ori(int type, char *instruction);

	};

	class PairedSingleFamily : public Instruction {

	public:

		PairedSingleFamily(int type, char *instruction);

	};

	class AddFamily : public Instruction {

	public:

		AddFamily(int type, char *instruction);

	};

	class CmpFamily : public Instruction {

	public:

		CmpFamily(int type, char *instruction);

	};

	class BFamily : public Instruction {

	public:

		BFamily(int type, char *instruction);

	};

	class SpecBranchFamily : public Instruction {

	public:

		SpecBranchFamily(int type, char *instruction);

	};

	class MathFamily : public ConditionInstruction {

	public:

		MathFamily(int type, char *instruction);

	};

	class FloatSingleFamily : public ConditionInstruction {

	public:

		FloatSingleFamily(int type, char *instruction);

	};

	class FloatDoubleFamily : public ConditionInstruction {

	public:

		FloatDoubleFamily(int type, char *instruction);

	};

}

