/** @file cs2010.c */

#include <stdlib.h>
#include <string.h>

#include "cs2010.h"
#include "cs_instructions.h"

bool cs_init(cs2010 *cs) {
    if (!cs) {
        return false;
    }

    cs->mem.rom = malloc(sizeof * cs->mem.rom * CS_ROM_SIZE);
    if (!cs->mem.rom) {
        return false;
    }

    cs->mem.ram = malloc(sizeof * cs->mem.ram * CS_RAM_SIZE);
    if (!cs->mem.ram) {
        free(cs->mem.rom);
        return false;
    }

    cs->reg.regfile[0] = &cs->reg.r0;
    cs->reg.regfile[1] = &cs->reg.r1;
    cs->reg.regfile[2] = &cs->reg.r2;
    cs->reg.regfile[3] = &cs->reg.r3;
    cs->reg.regfile[4] = &cs->reg.r4;
    cs->reg.regfile[5] = &cs->reg.r5;
    cs->reg.regfile[6] = &cs->reg.r6;
    cs->reg.regfile[7] = &cs->reg.r7;

    cs_hard_reset(cs);
    return true;
}

void cs_hard_reset(cs2010 *cs) {
    cs_clear_memory(cs, CS_CLEAR_RAM | CS_CLEAR_ROM);
    cs_reset_registers(cs);
    cs->stopped = false;
}

void cs_soft_reset(cs2010 *cs) {
    cs->reg.pc = 0;
    cs->reg.sp = 0xFF;
    cs->stopped = false;
}

void cs_clear_memory(cs2010 *cs, unsigned char flags) {
    if (flags & CS_CLEAR_ROM) {
        memset(cs->mem.rom, 0, CS_ROM_SIZE * sizeof *cs->mem.rom);
    }
    if (flags & CS_CLEAR_RAM) {
        memset(cs->mem.ram, 0, CS_RAM_SIZE * sizeof *cs->mem.ram);
    }
}

void cs_reset_registers(cs2010 *cs) {
    cs->reg.ac = 0;
    cs->reg.ir = 0;
    cs->reg.mar = 0;
    cs->reg.mdr = 0;
    cs->reg.pc = 0;
    cs->reg.r0 = 0;
    cs->reg.r1 = 0;
    cs->reg.r2 = 0;
    cs->reg.r3 = 0;
    cs->reg.r4 = 0;
    cs->reg.r5 = 0;
    cs->reg.r6 = 0;
    cs->reg.r7 = 0;
    cs->reg.sr = 0;
    cs->reg.sp = 0xFF;
    cs->reg.signals = CS_SIGNALS_NONE;
 }

int cs_load_and_check(cs2010 *cs, unsigned short *sentences, size_t sentences_length) {
    size_t i = 0;
    
    if (!sentences) {
        return CS_FAILED;
    }
    
    if (sentences_length > CS_ROM_SIZE) {
        return CS_NOT_ENOUGH_ROM;
    }

    for (; i < sentences_length; i++) {
        if (!cs_ins_list[CS_GET_OPCODE(sentences[i])].exec) {
            return CS_INVALID_INSTRUCTIONS;
        }
        cs->mem.rom[i] = sentences[i];
    }

    return CS_SUCCESS;
}

void cs_microfetch(cs2010 *cs) {
    cs->microop++;
    cs->reg.signals = cs_ins_list[CS_GET_OPCODE(cs->reg.ir)].signals[cs->microop];
}

void cs_fetch(cs2010 *cs) {
    cs->microop = 0;
    cs->reg.ir = cs->mem.rom[cs->reg.pc];
    cs->reg.signals = cs_ins_list[CS_GET_OPCODE(cs->reg.ir)].signals[cs->microop];
    cs->reg.pc++;
}

void cs_microstep(cs2010 *cs) {
    cs_ins_list[CS_GET_OPCODE(cs->reg.ir)].microstepper(cs);
}

void cs_step(cs2010 *cs) {
    cs_ins_list[CS_GET_OPCODE(cs->reg.ir)].stepper(cs);
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

void cs_blockstep(cs2010 *cs) {
    unsigned char opcode;

    cs_fullstep(cs);
    opcode = CS_GET_OPCODE(cs->reg.ir);
    while (opcode != CS_INS_I_JMP &&
        opcode != CS_INS_I_BRXX &&
        opcode != CS_INS_I_CALL &&
        !cs->stopped) {
        cs_step(cs);
        opcode = CS_GET_OPCODE(cs->reg.ir);
    }
}

void cs_free(cs2010 *cs) {
    if (!cs) {
        return;
    }

    if (cs->mem.rom) {
        free(cs->mem.rom);
    }

    if (cs->mem.ram) {
        free(cs->mem.ram);
    }
}
