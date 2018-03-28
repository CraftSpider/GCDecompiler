
#include <sstream>
#include <iostream>
#include "../utils.h"
#include "codes.h"
#include "instruction.h"
#include "register.h"

using std::endl;

namespace PPC {

    Instruction::Instruction() {
        this->name = "UNKNOWN INSTRUCTION";
    }

    Instruction::Instruction(const Instruction &inst) {
        this->name = inst.name;
        this->pattern = inst.pattern;
        this->used = nullptr;
        this->sources = nullptr;
        this->destination = nullptr;
        this->instruction = new char[4]();
        for (int i = 0; i < 4; ++i) {
            this->instruction[i] = inst.instruction[i];
        }
        this->type = inst.type;
    }

    Instruction::Instruction(int type, char *instruction) {
        this->type = (char)type;
        if (primary_codes.count(type) > 0) {
            this->name = primary_codes.at(type);
        } else {
            this->name = "UNKNOWN INSTRUCTION";
        }
        if (primary_patterns.count(type) > 0) {
            this->pattern = primary_patterns.at(type);
        } else {
            this->pattern = "FIX ME";
        }

        if (type == 0 && get_range(instruction, 6, 31) == 0) {
            this->name = "PADDING";
            this->pattern = "{}";
        }

        this->set_instruction(instruction);
    }

    Instruction::~Instruction() {
        delete this->used;
        delete this->sources;
        delete this->destination;
        delete[] this->instruction;
    }

    void Instruction::set_instruction(char *instruction) {
        delete[] this->instruction;
        this->instruction = new char[4]();
        for (int i = 0; i < 4; ++i) {
            this->instruction[i] = instruction[i];
        }
    }

    string Instruction::code_name() {
        return name;
    }

    string Instruction::get_variables() {
        if (this->pattern == "") {
            return "FIX ME";
        }
        return char_format(this->instruction, this->pattern);
    }

    std::set<Register> Instruction::used_registers() {
        // Examine own variable output, look for [rtype][num]
        if (this->used != nullptr) {
            return *this->used;
        }

        std::set<Register> *out = new std::set<Register>();
        string args = this->get_variables();
        string type = "";
        bool in_num = false;
        bool reg_var = false;
        bool pass = false;
        std::stringstream temp;
        for (int i = 0; i < args.length(); i++) {
            if (pass) {
                if (!is_num(args[i]) && !is_letter(args[i])) {
                    pass = false;
                    temp.clear();
                    temp.str("");
                }
            } else if (!in_num) {
                if (is_letter(args[i])) {
                    reg_var = true;
                    type += args[i];
                } else if (is_num(args[i]) && reg_var) {
                    in_num = true;
                    temp << args[i];
                } else if (is_num(args[i])) {
                    pass = true;
                } else if (reg_var) {
                    reg_var = false;
                    type = "";
                    temp.clear();
                    temp.str("");
                }
            } else {
                if (is_num(args[i])) {
                    temp << args[i];
                } else if (!is_num(args[i]) && !is_letter(args[i])) {
                    in_num = false;
                    reg_var = false;
                    int num;
                    temp >> num;
                    temp.clear();
                    temp.str("");
                    out->insert(Register(num, type));
                    type = "";
                } else {
                    pass = true;
                    in_num = false;
                    reg_var = false;
                    temp.clear();
                    temp.str("");
                }
            }
        }
        if (reg_var && in_num && !pass) {
            int reg;
            temp >> reg;
            out->insert(Register(reg, type));
        }
        this->used = out;
        return *this->used;
    }

    Register Instruction::destination_register() {
        // Get 1st register of variable output, or -1 if none
        // Which variable is the destination? It's the first one, unless we don't have a destination.
        if (this->destination != nullptr) {
            return *this->destination;
        }

        if (primary_missing_dest.find(this->type) != primary_missing_dest.end()) {
            this->destination = new Register();
            return *this->destination;
        }

        string args = this->get_variables();
        bool in_num = false;
        std::stringstream nums;
        int num = 0;
        string type = "";
        for (int i = 0; i < args.length() && args[i] != ','; i++) {
            if (!in_num) {
                if (is_letter(args[i])) {
                    type += args[i];
                } else if (is_num(args[i])) {
                    in_num = true;
                    nums << args[i];
                }
            } else {
                if (is_num(args[i])) {
                    nums << args[i];
                } else {
                    nums >> num;
                }
            }
        }

        this->destination = new Register(num, type);
        return *this->destination;
    }

    std::set<Register> Instruction::source_registers() {
        // Get whatever registers aren't used by destination
        if (this->sources != nullptr) {
            return *this->sources;
        }

        std::set<Register> *out;
        out = new std::set<Register>(this->used_registers());
        if (primary_missing_dest.find(this->type) == primary_missing_dest.end() && out->size() > 0) {
            out->erase(this->destination_register());
        }

        this->sources = out;
        return *this->sources;
    }

    ConditionInstruction::ConditionInstruction(int type, char *instruction) : Instruction(type, instruction) {
        if (get_bit(instruction, 31)) {
            this->name += ".";
        }
    }

    Ori::Ori(int type, char *instruction) : Instruction(type, instruction) {
        if (get_range(instruction, 6, 31) == 0) {
            this->name = "nop";
            this->pattern = "{}";
        }
    }

    AddFamily::AddFamily(int type, char *instruction) : Instruction(type, instruction) {
        this->pattern = "r{6,10}, r{11,15}, {sX|16,31}";
        if (get_signed_range(instruction, 16, 31) < 0) {
            this->name = "sub" + this->name.substr(3, this->name.length());
            this->pattern = "r{6,10}, r{11,15}, {aX|16,31}";
        }
    }

    CmpFamily::CmpFamily(int type, char *instruction) : Instruction(type, instruction) {
        if (!get_bit(instruction, 10)) {
            this->name = this->name.substr(0, this->name.length() - 1) + "wi";
            this->pattern = "crf{6,8}, r{11,15}, {X|16,31}";
        } else {
            this->pattern = "crf{6,8}, {10,10}, r{11,15}, {X|16,31}";
        }
    }

    BFamily::BFamily(int type, char *instruction) : Instruction(type, instruction) {
        if (get_bit(instruction, 31)) {
            this->name += "l";
        }
        if (get_bit(instruction, 30)) {
            this->name += "a";
        }

        if (type == 16) {
            int BO = get_range(instruction, 6, 10);
            int BI = get_range(instruction, 11, 15);
            if (BO == 12 && BI == 0) {
                this->name = "blt";
                this->pattern = "{X|16,29}";
            } else if (BO == 4 && BI == 10) {
                this->name = "bne";
                this->pattern = "{X|16,29}";
            }
        }
    }

    SpecBranchFamily::SpecBranchFamily(int type, char *instruction) : Instruction(type, instruction) {
        int stype = get_range(instruction, 21, 30);
        if (secondary_codes_sb.count(stype) > 0) {
            this->name = secondary_codes_sb.at(stype);
        }
        if (secondary_patterns_sb.count(stype) > 0) {
            this->pattern = secondary_patterns_sb.at(stype);
        }
        // Set up commands with special conditions
        if (stype == 16 || stype == 528) { // If it's a branch command, set up that
            int BO = get_range(instruction, 6, 10);
            int BI = get_range(instruction, 11, 15);
            this->pattern = "{6,10}, {11,15}";
            if (get_bit(instruction, 31)) {
                this->name += "l";
            }
            if (BO == 20) {
                this->name = "b" + this->name.substr(2, this->name.length());
                this->pattern = "{}";
            }
        }
    }

    MathFamily::MathFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
        int stype = get_range(instruction, 21, 30);
        try {
            this->name = secondary_codes_math.at(stype);
            if (this->name.substr(this->name.length() - 1, this->name.length()) == ".") {
                this->name += ".";
            }
        } catch (const std::exception e) {
            std::cout << "Condition Catch" << endl;
        }
        if (secondary_patterns_math.count(stype) > 0) {
            this->pattern = secondary_patterns_math.at(stype);
        }
        // Set up commands with special conditions.
        if (stype == 0 || stype == 32) {
            if (!get_bit(instruction, 10)) {
                this->name += "w";
                this->pattern = "crf{6,8}, r{11,15}, r{16,20}";
            } else {
                this->pattern = "crf{6,8}, {10,10}, r{11,15}, r{16,20}";
            }
        } else if (stype == 124) {
            if (get_range(instruction, 6, 10) == get_range(instruction, 16, 20)) {
                this->name = "not";
                this->pattern = "r{11,15}, r{6,10}";
            }
        } else if (stype == 144) {
            if (get_range(instruction, 12, 19) == 0xFF) {
                this->name == "mtcr";
                this->pattern = "r{6,10}";
            } else {
                this->pattern = "{X|12,19}, r{6,10}";
            }
        } else if (stype == 339 || stype == 467) {
            int reg = get_range(instruction, 11, 15) + (get_range(instruction, 16, 20) << 5);
            if (reg == 0b1) {
                this->name = this->name.substr(0, 2) + "xer";
                this->pattern = "r{6,10}";
            } else if (reg == 0b01000) {
                this->name = this->name.substr(0, 2) + "lr";
                this->pattern = "r{6,10}";
            } else if (reg == 0b01001) {
                this->name = this->name.substr(0, 2) + "ctr";
                this->pattern = "r{6,10}";
            } else {
                std::stringstream out;
                if (stype == 339) {
                    out << reg << ", r{6,10}";
                    this->pattern = out.str();
                } else {
                    out << "r{6,10}, " << reg;
                    this->pattern = out.str();
                }
            }
        } else if (stype == 371) {
            int reg = get_range(instruction, 11, 15) + (get_range(instruction, 16, 20) << 5);
            if (reg == 0b0100001100) {
                this->name = "mftb";
                this->pattern = "r{6,10}";
            } else if (reg == 0b0100001101) {
                this->name = "mftbu";
                this->pattern = "r{6,10}";
            } else {
                std::stringstream out;
                out << "r{6,10}, " << reg;
                this->pattern = out.str();
            }
        } else if (stype == 444) {
            if (get_range(instruction, 6, 10) == get_range(instruction, 16, 20)) {
                this->name = "mr";
                this->pattern = "r{11,15}, r{6,10}";
            }
        }
    }

    //TODO: Add special consideration for stswi and stswx source registers for math

    //TODO: Add special consideration for lswi and lswx destination register for math

    FloatSingleFamily::FloatSingleFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
        int stype = get_range(instruction, 26, 30);
        try {
            this->name = secondary_codes_float.at(stype);
            if (this->name.substr(this->name.length() - 1, this->name.length()) == ".") {
                this->name += ".";
            }
        } catch (const std::exception e) {
            std::cout << "FloatSingle Catch" << endl;
        }
        if (secondary_patterns_float.count(stype) > 0) {
            this->pattern = secondary_patterns_float.at(stype);
        }
        // Set up commands with special conditions
    }

    FloatDoubleFamily::FloatDoubleFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
        int stype;
        if (get_bit(instruction, 26)) {
            stype = get_range(instruction, 26, 30);
        } else {
            stype = get_range(instruction, 21, 30);
        }
        try {
            this->name = secondary_codes_double.at(stype);
            if (this->name.substr(this->name.length() - 1, this->name.length()) == ".") {
                this->name += ".";
            }
        } catch (const std::exception e) {
            std::cout << "FloatDouble Catch" << endl;
        }
        if (secondary_patterns_double.count(stype) > 0) {
            this->pattern = secondary_patterns_double.at(stype);
        }
        // Set up commands with special conditions
    }

}