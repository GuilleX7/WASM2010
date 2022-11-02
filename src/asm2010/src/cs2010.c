/** @file cs2010.c */

#include <stdlib.h>
#include <string.h>

#include "cs2010.h"
#include "cs_instructions.h"

static size_t cs_io_read_stub(size_t address) {
  return CS_IO_READ_NOT_CONTROLLED;
}

static bool cs_io_write_stub(size_t address, unsigned char content) {
  return CS_IO_WRITE_NOT_CONTROLLED;
}

cs2010 *cs_create() { return malloc(sizeof(cs2010)); }

bool cs_init(cs2010 *cs) {
  if (!cs) {
    return false;
  }

  cs->memory.rom = malloc(sizeof *cs->memory.rom * CS_ROM_SIZE);
  if (!cs->memory.rom) {
    return false;
  }

  cs->memory.ram = malloc(sizeof *cs->memory.ram * CS_RAM_SIZE);
  if (!cs->memory.ram) {
    free(cs->memory.rom);
    return false;
  }

  cs->registers.regfile[0] = &cs->registers.r0;
  cs->registers.regfile[1] = &cs->registers.r1;
  cs->registers.regfile[2] = &cs->registers.r2;
  cs->registers.regfile[3] = &cs->registers.r3;
  cs->registers.regfile[4] = &cs->registers.r4;
  cs->registers.regfile[5] = &cs->registers.r5;
  cs->registers.regfile[6] = &cs->registers.r6;
  cs->registers.regfile[7] = &cs->registers.r7;

  cs->io_read_fn = cs_io_read_stub;
  cs->io_write_fn = cs_io_write_stub;

  return true;
}

void cs_set_io_functions(cs2010 *cs, io_read_fn_t io_read_fn,
                         io_write_fn_t io_write_fn) {
  cs->io_read_fn = io_read_fn;
  cs->io_write_fn = io_write_fn;
}

void cs_hard_reset(cs2010 *cs, bool clear_rom) {
  unsigned char flags = CS_CLEAR_RAM;
  if (clear_rom) {
    flags |= CS_CLEAR_ROM;
  }
  cs_clear_memory(cs, flags);
  cs_reset_registers(cs);
  cs_fetch(cs);
}

void cs_soft_reset(cs2010 *cs) {
  cs->registers.pc = 0;
  cs->registers.sp = 0xFF;
  cs->stopped = false;
  cs_fetch(cs);
}

void cs_clear_memory(cs2010 *cs, unsigned char flags) {
  if (flags & CS_CLEAR_ROM) {
    memset(cs->memory.rom, 0, CS_ROM_SIZE * sizeof *cs->memory.rom);
  }
  if (flags & CS_CLEAR_RAM) {
    memset(cs->memory.ram, 0, CS_RAM_SIZE * sizeof *cs->memory.ram);
  }
}

void cs_reset_registers(cs2010 *cs) {
  cs->registers.signals = CS_SIGNALS_NONE;
  cs->registers.ir = 0;
  cs->registers.r0 = 0;
  cs->registers.r1 = 0;
  cs->registers.r2 = 0;
  cs->registers.r3 = 0;
  cs->registers.r4 = 0;
  cs->registers.r5 = 0;
  cs->registers.r6 = 0;
  cs->registers.r7 = 0;
  cs->registers.sp = 0xFF;
  cs->registers.pc = 0;
  cs->registers.ac = 0;
  cs->registers.sr = 0;
  cs->registers.mdr = 0;
  cs->registers.mar = 0;
  cs->stopped = false;
}

int cs_load_program(cs2010 *cs, unsigned short *sentences,
                    size_t sentences_length) {
  size_t i;

  if (!sentences) {
    return CS_FAILED;
  }

  if (sentences_length > CS_ROM_SIZE) {
    return CS_NOT_ENOUGH_ROM;
  }

  for (i = 0; i < sentences_length; i++) {
    if (!cs_ins_list[CS_GET_OPCODE(sentences[i])].is_executable) {
      return CS_INVALID_INSTRUCTIONS;
    }
  }

  cs_clear_memory(cs, CS_CLEAR_RAM | CS_CLEAR_ROM);
  cs_reset_registers(cs);
  memcpy(cs->memory.rom, sentences, sentences_length);
  cs_fetch(cs);
  return CS_SUCCESS;
}

void cs_microfetch(cs2010 *cs) {
  cs->registers.signals =
      cs_ins_list[CS_GET_OPCODE(cs->registers.ir)].signals[cs->microop++];
}

void cs_fetch(cs2010 *cs) {
  cs->registers.ir = cs->memory.rom[cs->registers.pc++];
  cs->microop = 0;
  cs->registers.signals =
      cs_ins_list[CS_GET_OPCODE(cs->registers.ir)].signals[cs->microop];
}

void cs_microstep(cs2010 *cs) {
  cs_ins_list[CS_GET_OPCODE(cs->registers.ir)].microstepper(cs);
}

void cs_step(cs2010 *cs) {
  cs_ins_list[CS_GET_OPCODE(cs->registers.ir)].stepper(cs);
}

void cs_fullstep(cs2010 *cs) {
  if (!cs->microop) {
    cs_step(cs);
  } else {
    do {
      cs_microstep(cs);
    } while (cs->microop);
  }
}

bool cs_blockstep(cs2010 *cs, size_t max_instructions) {
  unsigned char opcode;

  cs_fullstep(cs);
  max_instructions--;

  opcode = CS_GET_OPCODE(cs->registers.ir);
  while (opcode != CS_INS_I_JMP && opcode != CS_INS_I_BRXX &&
         opcode != CS_INS_I_CALL && !cs->stopped) {
    cs_step(cs);
    opcode = CS_GET_OPCODE(cs->registers.ir);

    max_instructions--;
    if (!max_instructions) {
      return false;
    }
  }

  return true;
}

void cs_free(cs2010 *cs) {
  if (!cs) {
    return;
  }

  if (cs->memory.rom) {
    free(cs->memory.rom);
  }

  if (cs->memory.ram) {
    free(cs->memory.ram);
  }
}
