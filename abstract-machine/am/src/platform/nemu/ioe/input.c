#include <am.h>
#include <klib.h>
#include <nemu.h>

#include "../../../riscv/riscv.h"

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t input = inl(KBD_ADDR);
  if (input != AM_KEY_NONE){
    kbd->keydown = KEYDOWN_MASK & input;
    kbd->keycode = input & (~KEYDOWN_MASK);
  }else {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  }
}