#pragma once

#include <stdint.h>

#include "slim_vector.hpp"

#define PATCH_COUNT 6
#define EXE_COUNT 2

struct patch {
   uint32_t address = 0;
   uint32_t expected_value = 0;
   uint32_t replacement_value = 0;
   bool value_is_ext_section_relative_address = false;
};

struct code_patch {
   uint32_t address = 0;
   const uint8_t* expected_bytes = nullptr;
   const uint8_t* replacement_bytes = nullptr;
   uint32_t length = 0;
};

struct patch_set {
   const char* name = "";
   slim_vector<patch> patches;
   slim_vector<code_patch> code_patches;
};

struct exe_patch_list {
   const char* name = "";
   uint32_t id_address = 0;
   uint64_t expected_id = 0;

   const patch_set patches[PATCH_COUNT];
};

extern const exe_patch_list patch_lists[EXE_COUNT];
extern const uint32_t ext_section_size;
