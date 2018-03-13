#pragma once

#include <unordered_map>
#include <string>

namespace PPC {

	static const std::unordered_map<int, std::string> primary_codes;
	static const std::unordered_map<int, std::string> secondary_codes_sb;
	static const std::unordered_map<int, std::string> secondary_codes_math;
	static const std::unordered_map<int, std::string> secondary_codes_float;
	static const std::unordered_map<int, std::string> secondary_codes_double;

	static const std::unordered_map<int, std::string> primary_patterns;
	static const std::unordered_map<int, std::string> secondary_patterns_sb;
	static const std::unordered_map<int, std::string> secondary_patterns_math;
	static const std::unordered_map<int, std::string> secondary_patterns_float;
	static const std::unordered_map<int, std::string> secondary_patterns_double;

}