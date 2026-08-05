/***
   mockcpp is a C/C++ mock framework.
   Copyright [2008] [Darwin Yuan <darwin.yuan@gmail.com>]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
***/
#ifndef __MOCKCPP_JMP_CODE_ARM_H__
#define __MOCKCPP_JMP_CODE_ARM_H__

// ARM/A32: ldr pc, [pc, #-4]; .word target
const unsigned char armJmpCodeTemplate[] =
   { 0x04, 0xF0, 0x1F, 0xE5, 0x00, 0x00, 0x00, 0x00 };

// Thumb-2, word-aligned entry: ldr.w pc, [pc]; .word target
const unsigned char thumbAlignedJmpCodeTemplate[] =
   { 0xDF, 0xF8, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00 };

// Thumb-2, halfword-aligned entry: ldr.w pc, [pc, #4]; nop; .word target
const unsigned char thumbHalfwordJmpCodeTemplate[] =
   { 0xDF, 0xF8, 0x04, 0xF0, 0x00, 0xBF,
     0x00, 0x00, 0x00, 0x00 };

const size_t MAX_JMP_CODE_SIZE = sizeof(thumbHalfwordJmpCodeTemplate);

inline size_t buildArmJmpCode(unsigned char* code, const void* from, const void* to)
{
   const uintptr_t fromAddress = (uintptr_t)from;
   const uintptr_t targetAddress = (uintptr_t)to;

   if ((fromAddress & 1U) == 0)
   {
      ::memcpy(code, armJmpCodeTemplate, sizeof(armJmpCodeTemplate));
      const uint32_t target = (uint32_t)targetAddress;
      ::memcpy(code + 4, &target, sizeof(target));
      return sizeof(armJmpCodeTemplate);
   }

   const uintptr_t patchAddress = fromAddress & ~(uintptr_t)1U;
   if ((patchAddress & 3U) == 0)
   {
      ::memcpy(code, thumbAlignedJmpCodeTemplate,
               sizeof(thumbAlignedJmpCodeTemplate));
      const uint32_t target = (uint32_t)targetAddress;
      ::memcpy(code + 4, &target, sizeof(target));
      return sizeof(thumbAlignedJmpCodeTemplate);
   }

   ::memcpy(code, thumbHalfwordJmpCodeTemplate,
            sizeof(thumbHalfwordJmpCodeTemplate));
   const uint32_t target = (uint32_t)targetAddress;
   ::memcpy(code + 6, &target, sizeof(target));
   return sizeof(thumbHalfwordJmpCodeTemplate);
}

#define GET_JMP_CODE_PATCH_ADDRESS(from) \
   ((void*)((uintptr_t)(from) & ~(uintptr_t)1U))

#endif
