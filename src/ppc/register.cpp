
#include <string>
#include <sstream>

#include "types.h"
#include "ppc/register.h"

namespace PPC {

Register::Register() {
    this->number = 0;
    this->type = BAD;
}

Register::Register(const std::string& reg) {
    std::stringstream num, type;
    for (auto ch : reg) {
        if (std::isdigit(ch)) {
            num << ch;
        } else {
            type << ch;
        }
    }
    
    this->number = std::stoi(num.str());
    this->type = get_type(type.str());
}

Register::Register(const uchar& number, const RType& type) {
    this->number = number;
    this->type = type;
}

Register::Register(const uchar& number, const std::string& type) {
    this->number = number;
    this->type = get_type(type);
}

bool Register::operator==(const Register& reg) const {
    return this->number == reg.number && this->type == reg.type;
}

bool Register::operator!=(const Register& reg) const {
    return this->number != reg.number || this->type != reg.type;
}

bool Register::operator<(const Register& reg) const {
    if (this->type == reg.type) {
        return this->number < reg.number;
    }
    return this->type < reg.type;
}

bool Register::operator>(const Register& reg) const {
    if (this->type == reg.type) {
        return this->number > reg.number;
    }
    return this->type > reg.type;
}

Register::RType get_type(const std::string& type) {
    if (type == "r") {
        return Register::REGULAR;
    } else if (type == "fr") {
        return Register::FLOAT;
    } else if (type == "crf") {
        return Register::CONDITION;
    } else {
        return Register::BAD;
    }
}

}