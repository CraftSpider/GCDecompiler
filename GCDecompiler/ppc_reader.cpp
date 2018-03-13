
#include <limits.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <set>
#include <unordered_map>
#include "types.h"
#include "rel.h"
#include "utils.h"
#include "ppc_codes.h"

using std::string;
using std::ios;
using std::endl;

namespace PPC {

	class Instruction {

	protected:

		string name, pattern;
		char *instruction;

	public:

		Instruction() {
			this->name = "UNKNOWN INSTRUCTION";
		}

		Instruction(int type, char *instruction) {
			try {
				this->name = primary_codes.at(type);
			} catch (const std::exception e) {
				this->name = "UNKNOWN INSTRUCTION";
			}
			try {
				this->pattern = primary_patterns.at(type);
			} catch (const std::exception e) {
				this->pattern = "FIX ME";
			}

			if (type == 0 && get_range(instruction, 6, 31) == 0) {
				this->name = "PADDING";
				this->pattern = "{}";
			}

			this->instruction = instruction;
		}

		virtual string code_name() {
			return name;
		}

		virtual string get_variables() {
			if (this->pattern == "") {
				return "FIX ME";
			}
			return char_format(this->instruction, this->pattern);
		}

	};

	class ConditionInstruction : public Instruction {

	public:

		ConditionInstruction(int type, char *instruction) : Instruction(type, instruction) {
			if (get_bit(instruction, 31)) {
				this->name += ".";
			}
		}

	};

	class Ori : public Instruction {

	public:

		Ori(int type, char *instruction) : Instruction(type, instruction) {
			if (get_range(instruction, 6, 31) == 0) {
				this->name = "nop";
				this->pattern = "{}";
			}
		}

	};

	class AddFamily : public Instruction {

	public:

		AddFamily(int type, char *instruction) : Instruction(type, instruction) {
			this->pattern = "r{6,10}, r{11,15}, {sX|16,31}";
			if (get_signed_range(instruction, 16, 31) < 0) {
				this->name = "sub" + this->name.substr(3, this->name.length());
				this->pattern = "r{6,10}, r{11,15}, {aX|16,31}";
			}
		}

	};

	class CmpFamily : public Instruction {

	public:

		CmpFamily(int type, char *instruction) : Instruction(type, instruction) {
			if (!get_bit(instruction, 10)) {
				this->name = this->name.substr(0, this->name.length() - 1) + "wi";
				this->pattern = "crf{6,8}, r{11,15}, {X|16,31}";
			} else {
				this->pattern = "crf{6,8}, {10,10}, r{11,15}, {X|16,31}";
			}
		}

	};

	class BFamily : public Instruction {

	public:

		BFamily(int type, char *instruction) : Instruction(type, instruction) {
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

	};

	class SpecBranchFamily : public Instruction {

	public:

		SpecBranchFamily(int type, char *instruction) : Instruction(type, instruction) {
			int stype = get_range(instruction, 21, 30);
			try {
				this->name = secondary_codes_sb.at(stype);
			} catch (const std::exception e) {}
			try {
				this->pattern = secondary_patterns_sb.at(stype);
			} catch (const std::exception e) {}
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

	};

	class MathFamily : public ConditionInstruction {

	public:

		MathFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
			int stype = get_range(instruction, 21, 30);
			try {
				this->name = secondary_codes_math.at(stype);
				if (this->name.substr(this->name.length() - 1, this->name.length()) == ".") {
					this->name += ".";
				}
			} catch (const std::exception e) {}
			try {
				this->pattern = secondary_patterns_math.at(stype);
			} catch (const std::exception e) {}
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

	};

	class FloatSingleFamily : public ConditionInstruction {

	public:

		FloatSingleFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
			int stype = get_range(instruction, 26, 30);
			try {
				this->name = secondary_codes_float.at(stype);
				if (this->name.substr(this->name.length() - 1, this->name.length()) == ".") {
					this->name += ".";
				}
			} catch (const std::exception e) {}
			try {
				this->pattern = secondary_patterns_float.at(stype);
			} catch (const std::exception e) {}
			// Set up commands with special conditions
		}

	};

	class FloatDoubleFamily : public ConditionInstruction {

	public:

		FloatDoubleFamily(int type, char *instruction) : ConditionInstruction(type, instruction) {
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
			} catch (const std::exception e) {}
			try {
				this->pattern = secondary_patterns_double.at(stype);
			} catch (const std::exception e) {}
			// Set up commands with special conditions
		}

	};

	static Instruction* create_instruction(int type, char *instruction) {
		if (type == 10 || type == 11) return new CmpFamily(type, instruction);
		else if (type >= 12 && type <= 15) return new AddFamily(type, instruction);
		else if (type == 16 || type == 18) return new BFamily(type, instruction);
		else if (type == 19) return new SpecBranchFamily(type, instruction);
		else if (type == 23) return new ConditionInstruction(type, instruction);
		else if (type == 31) return new MathFamily(type, instruction);
		else if (type == 59) return new FloatSingleFamily(type, instruction);
		else if (type == 63) return new FloatDoubleFamily(type, instruction);
		else return new Instruction(type, instruction);
	}

	void disassemble(string file_in, string file_out, int start, int end) {
		std::cout << "Disassembling PPC" << endl;
		std::fstream input(file_in, ios::in | ios::binary);
		std::fstream output(file_out, ios::out);

		static uint position;
		static bool func_end = false, q1 = false, q2 = false, q3 = false;

		if (end == -1) {
			input.seekg(0, ios::end);
			end = (int)input.tellg();
		}
		int size = end - start;
		int hex_length = (int)std::floor((std::log(size) / std::log(16)) + 1) + 2;
		static char instruction[4];

		input.seekg(start, ios::beg);
		while (input.tellg() < end) {
			position = (int)input.tellg() - start;
			
			if ((float)position / size > .25 && !q1) {
				std::cout << "  25% Complete" << endl;
				q1 = true;
			} else if ((float)position / size > .5 && !q2) {
				std::cout << "  50% Complete" << endl;
				q2 = true;
			} else if ((float)position / size > .75 && !q3) {
				std::cout << "  75% Complete" << endl;
				q3 = true;
			}

			input.read(instruction, 4);

			int opcode = get_range(instruction, 0, 5);
			Instruction *instruct = create_instruction(opcode, instruction);
			if (instruct->code_name() == "blr") {
				func_end = true;
			}

			if (func_end && instruct->code_name() != "blr") {
				output << "; function: f_" << std::hex << position << std::dec << " at " << itoh(position) << endl;
				func_end = false;
			}

			string hex = itoh(position);
			string padding(hex_length - hex.length(), ' ');
			output << padding << hex << "    " << ctoh(instruction[0]) << " " << ctoh(instruction[1]) << " " <<
				ctoh(instruction[2]) << " " << ctoh(instruction[3]) << "    ";

			output << instruct->code_name();
			output << " " << instruct->get_variables();
			output << endl;
			delete instruct;
		}
		std::cout << "PPC disassembly finished" << endl;
	}

	void read_data(string file_in, string file_out, int start, int end) {
		std::cout << "Reading data section" << endl;
		std::fstream input(file_in, ios::in | ios::binary);
		std::fstream output(file_out, ios::out);

		//TODO: Data guessing (tm)
	}

	void read_data(REL *to_read, Section *section, std::vector<REL*> knowns, string file_out) {
		// open the file, look through every import in every REL file to check if they refer to a
		// location in the right area in this one. If they do, add it to the list. Once done, go through
		// the list and generate output values. Anything leftover goes in a separate section clearly marked.
		std::cout << "Reading REL data section" << endl;
		std::fstream output(file_out, ios::out);
		std::set<int> offsets;
		for (std::vector<REL*>::iterator rel_r = knowns.begin(); rel_r != knowns.end(); rel_r++) {
			REL *rel = (*rel_r);
			for (std::vector<Import>::iterator imp = rel->imports.begin(); imp != rel->imports.end(); imp++) {
				if (imp->module != to_read->id && rel->id != to_read->id) {
					continue;
				}
				for (std::vector<Relocation>::iterator reloc = imp->instructions.begin(); reloc != imp->instructions.end(); reloc++) {
					if (reloc->get_src_section().id == section->id && imp->module == rel->id) {
						offsets.insert(reloc->get_src_offset());
					}
					if (rel->id == to_read->id && reloc->get_dest_section().id == section->id) {
						offsets.insert(reloc->get_dest_offset());
					}
				}
			}
		}
		for (int offset : offsets) {
			output << itoh(offset) << ": ";
			// Need to add non ASCII stuff later
			int lookahead = offset;
			char dat = section->data[lookahead++ - section->offset];
			string out = "";
			while (dat >= 32 && dat <= 126 && offsets.find(lookahead) == offsets.end()) {
				out.append({ dat });
				dat = section->data[lookahead++ - section->offset];
			}
			if (dat == 0 && out != "") {
				output << out << endl;
			} else {
				lookahead = offset;
				out = ctoh(section->data[lookahead++ - section->offset]);
				while (offsets.find(lookahead) == offsets.end() && lookahead - offset < 5) {
					out += ctoh(section->data[lookahead++ - section->offset]);
				}
				if (out != "") {
					output << out << endl;
				}
			}
		}
		std::cout << "REL data section read complete" << endl;
	}

}