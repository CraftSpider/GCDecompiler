#pragma once

#include "types.h"

uint btoi(char *bytes, uint len);
const char* itob(uint num, uint length = 4);
std::string itoh(uint num);
std::string itoh(int num);
std::string ctoh(char num);
uint next_int(std::fstream *file, uint length = 4);
void write_int(std::fstream *file, uint num, uint length = 4);

void create_directory(std::string name);
