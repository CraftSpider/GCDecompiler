#pragma once

#include <string>
#include <set>

#include "ppc/register.h"

namespace PPC {

class Instruction {

protected:

    std::string name, pattern;
    std::set<Register> *used = nullptr, *sources = nullptr;
    Register *destination = nullptr;
    uchar *instruction = nullptr, type;

public:

    Instruction();
    Instruction(const Instruction &inst);
    Instruction(Instruction &&inst) noexcept;
    Instruction(const uchar& type, const uchar *instruction);
    virtual ~Instruction();

    void set_instruction(const uchar *instruction);
    virtual std::string code_name();
    virtual std::string get_variables();

    virtual std::set<Register> used_registers();
    virtual Register destination_register();
    virtual std::set<Register> source_registers();

};

class ConditionInstruction : public Instruction {

public:

    ConditionInstruction(const uchar& type, const uchar *instruction);

};

class Ori : public Instruction {

public:

    Ori(const uchar& type, const uchar *instruction);

};

class PairedSingleFamily : public Instruction {

public:

    PairedSingleFamily(const uchar& type, const uchar *instruction);

};

class AddFamily : public Instruction {

public:

    AddFamily(const uchar& type, const uchar *instruction);

};

class CmpFamily : public Instruction {

public:

    CmpFamily(const uchar& type, const uchar *instruction);

};

class BFamily : public Instruction {

public:

    BFamily(const uchar& type, const uchar *instruction);

};

class SpecBranchFamily : public Instruction {

public:

    SpecBranchFamily(const uchar& type, const uchar *instruction);

};

class MathFamily : public ConditionInstruction {

public:

    MathFamily(const uchar& type, const uchar *instruction);
    
    virtual Register destination_register() override;
    
};

class FloatSingleFamily : public ConditionInstruction {

public:

    FloatSingleFamily(const uchar& type, const uchar *instruction);

};

class FloatDoubleFamily : public ConditionInstruction {

public:

    FloatDoubleFamily(const uchar& type, const uchar *instruction);
    
    virtual Register destination_register() override;

};

Instruction* create_instruction(const uchar *instruction);

}

