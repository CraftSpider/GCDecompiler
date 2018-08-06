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
		Instruction(const int& type, const char *instruction);
		virtual ~Instruction();

		void set_instruction(const char *instruction);
		virtual string code_name();
		virtual string get_variables();

		virtual std::set<Register> used_registers();
		virtual Register destination_register();
		virtual std::set<Register> source_registers();

	};

	class ConditionInstruction : public Instruction {

	public:

		ConditionInstruction(const int& type, const char *instruction);

	};

	class Ori : public Instruction {

	public:

		Ori(const int& type, const char *instruction);

	};

	class PairedSingleFamily : public Instruction {

	public:

		PairedSingleFamily(const int& type, const char *instruction);

	};

	class AddFamily : public Instruction {

	public:

		AddFamily(const int& type, const char *instruction);

	};

	class CmpFamily : public Instruction {

	public:

		CmpFamily(const int& type, const char *instruction);

	};

	class BFamily : public Instruction {

	public:

		BFamily(const int& type, const char *instruction);

	};

	class SpecBranchFamily : public Instruction {

	public:

		SpecBranchFamily(const int& type, const char *instruction);

	};

	class MathFamily : public ConditionInstruction {

	public:

		MathFamily(const int& type, const char *instruction);

	};

	class FloatSingleFamily : public ConditionInstruction {

	public:

		FloatSingleFamily(const int& type, const char *instruction);

	};

	class FloatDoubleFamily : public ConditionInstruction {

	public:

		FloatDoubleFamily(const int& type, const char *instruction);

	};

}

