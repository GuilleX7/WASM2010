/** @file cs_op.c */

#include "utils.h"

#include "cs_instructions.h"
#include "cs_op.h"
#include <stdint.h>

static unsigned char cs_read_input(cs2010 *cs, size_t offset) {
  size_t value = cs->io_read_fn(offset);
  if (value > UINT8_MAX) {
    value = cs->memory.ram[offset];
  }
  return value;
}

static void cs_write_output(cs2010 *cs, size_t offset, unsigned char content) {
  if (!cs->io_write_fn(offset, content)) {
    cs->memory.ram[offset] = content;
  }
}

void cs_op_st_stepper(cs2010 *cs) {
  cs->registers.mar = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
  cs->registers.ac = *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  cs->registers.mdr = cs->registers.ac;
  cs_write_output(cs, cs->registers.mar, cs->registers.mdr);
  cs_fetch(cs);
}

void cs_op_st_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.ac;
    cs->registers.ac = *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
    cs_microfetch(cs);
    break;
  case 2:
    cs->registers.mdr = cs->registers.ac;
    cs_microfetch(cs);
    break;
  case 3:
  default:
    cs_write_output(cs, cs->registers.mar, cs->registers.mdr);
    cs_fetch(cs);
    break;
  }
}

void cs_op_ld_stepper(cs2010 *cs) {
  cs->registers.ac = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
  cs->registers.mar = cs->registers.ac;
  cs->registers.mdr = cs_read_input(cs, cs->registers.mar);
  *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.mdr;
  cs_fetch(cs);
}

void cs_op_ld_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.ac;
    cs_microfetch(cs);
    break;
  case 2:
    cs->registers.mdr = cs_read_input(cs, cs->registers.mar);
    cs_microfetch(cs);
    break;
  case 3:
  default:
    *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.mdr;
    cs_fetch(cs);
    break;
  }
}

void cs_op_sts_stepper(cs2010 *cs) {
  cs->registers.mar = CS_GET_ARG_B(cs->registers.ir);
  cs->registers.ac = *cs->registers.regfile[CS_GET_ARG_A(cs->registers.ir)];
  cs->registers.mdr = cs->registers.ac;
  cs_write_output(cs, cs->registers.mar, cs->registers.mdr);
  cs_fetch(cs);
}

void cs_op_sts_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.ac;
    cs->registers.ac = *cs->registers.regfile[CS_GET_ARG_A(cs->registers.ir)];
    cs_microfetch(cs);
    break;
  case 2:
    cs->registers.mdr = cs->registers.ac;
    cs_microfetch(cs);
    break;
  case 3:
  default:
    cs_write_output(cs, cs->registers.mar, cs->registers.mdr);
    cs_fetch(cs);
    break;
  }
}

void cs_op_lds_stepper(cs2010 *cs) {
  cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
  cs->registers.mar = cs->registers.ac;
  cs->registers.mdr = cs_read_input(cs, cs->registers.mar);
  *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.mdr;
  cs_fetch(cs);
}

void cs_op_lds_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.ac;
    cs_microfetch(cs);
    break;
  case 2:
    cs->registers.mdr = cs_read_input(cs, cs->registers.mar);
    cs_microfetch(cs);
    break;
  case 3:
  default:
    *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.mdr;
    cs_fetch(cs);
    break;
  }
}

void cs_op_call_stepper(cs2010 *cs) {
  cs->registers.mdr = cs->registers.pc;
  cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
  cs->registers.mar = cs->registers.sp--;
  cs->registers.pc = cs->registers.ac;
  cs->memory.ram[cs->registers.mar] = cs->registers.mdr;
  cs_fetch(cs);
}

void cs_op_call_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.mdr = cs->registers.pc;
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.sp--;
    cs_microfetch(cs);
    break;
  case 2:
  default:
    cs->registers.pc = cs->registers.ac;
    cs->memory.ram[cs->registers.mar] = cs->registers.mdr;
    cs_fetch(cs);
    break;
  }
}

void cs_op_ret_stepper(cs2010 *cs) {
  cs->registers.mar = ++cs->registers.sp;
  cs->registers.mdr = cs->memory.ram[cs->registers.mar];
  cs->registers.pc = cs->registers.mdr;
  cs_fetch(cs);
}

void cs_op_ret_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.sp++;
    cs_microfetch(cs);
    break;
  case 1:
    cs->registers.mar = cs->registers.sp;
    cs_microfetch(cs);
    break;
  case 2:
    cs->registers.mdr = cs->memory.ram[cs->registers.mar];
    cs_microfetch(cs);
    break;
  case 3:
  default:
    cs->registers.pc = cs->registers.mdr;
    cs_fetch(cs);
    break;
  }
}

static bool cs_op_is_jmp_condition_met(cs2010 *cs) {
  bool is_jmp_condition_met = false;
  switch (CS_GET_JMP_CONDITION(cs->registers.ir)) {
  case CS_JMP_COND_EQUAL:
    is_jmp_condition_met = cs->registers.sr & CS_SR_Z;
    break;
  case CS_JMP_COND_LOWER:
    is_jmp_condition_met = cs->registers.sr & CS_SR_C;
    break;
  case CS_JMP_COND_OVERFLOW:
    is_jmp_condition_met = cs->registers.sr & CS_SR_V;
    break;
  case CS_JMP_COND_SLOWER:
    is_jmp_condition_met =
        !!(cs->registers.sr & CS_SR_V) ^ !!(cs->registers.sr & CS_SR_N);
    break;
  default:
    break;
  }
  return is_jmp_condition_met;
}

void cs_op_brxx_stepper(cs2010 *cs) {
  if (cs_op_is_jmp_condition_met(cs)) {
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs->registers.pc = cs->registers.ac;
  }
  cs_fetch(cs);
}

void cs_op_brxx_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    if (cs_op_is_jmp_condition_met(cs)) {
      cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
      cs_microfetch(cs);
    } else {
      cs_fetch(cs);
    }
    break;
  case 1:
  default:
    cs->registers.pc = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

void cs_op_jmp_stepper(cs2010 *cs) {
  cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
  cs->registers.pc = cs->registers.ac;
  cs_fetch(cs);
}

void cs_op_jmp_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs_microfetch(cs);
    break;
  case 1:
  default:
    cs->registers.pc = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

static void cs_perform_arithmetic_op(cs2010 *cs, unsigned char a,
                                     unsigned char b, bool is_substracting) {
  /* bit 76543210
  SR  =  0000VNZC   where
  V: 2's complement overflow
  N: negative sign
  Z: zero
  C: carry (unsigned overflow)

  a: operand 1
  b: operand 2
  r: result */

  if (is_substracting) {
    b = -b;
  }
  cs->registers.ac = a + b;

  unsigned char a_7 = BIT_AT(a, 7);
  unsigned char b_7 = BIT_AT(b, 7);
  unsigned char c_7 = BIT_AT(cs->registers.ac, 7);
  cs->registers.sr =
      (a_7 == b_7 && a_7 != c_7)
          << CS_SR_V_OFFSET | /* sign(a) == sign(b) AND sign(a) != sign(c) */
      (c_7) << CS_SR_N_OFFSET |
      (!cs->registers.ac) << CS_SR_Z_OFFSET |
      (is_substracting ? (cs->registers.ac >= a) : (cs->registers.ac < a))
          << CS_SR_C_OFFSET;
}

static void cs_op_arithmetic_stepper(cs2010 *cs, unsigned char *dst_register,
                                     unsigned char b, bool is_substracting) {
  unsigned char a = *dst_register;
  cs_perform_arithmetic_op(cs, a, b, is_substracting);
  *dst_register = cs->registers.ac;
  cs_fetch(cs);
}

static void cs_op_arithmetic_microstepper(cs2010 *cs,
                                          unsigned char *dst_register,
                                          unsigned char b,
                                          bool is_substracting) {
  unsigned char a;

  switch (cs->microop) {
  case 0:
    a = *dst_register;
    cs_perform_arithmetic_op(cs, a, b, is_substracting);
    cs_microfetch(cs);
    break;
  case 1:
  default:
    *dst_register = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

void cs_op_add_stepper(cs2010 *cs) {
  return cs_op_arithmetic_stepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)], false);
}

void cs_op_add_microstepper(cs2010 *cs) {
  return cs_op_arithmetic_microstepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)], false);
}

void cs_op_sub_stepper(cs2010 *cs) {
  return cs_op_arithmetic_stepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)], true);
}

void cs_op_sub_microstepper(cs2010 *cs) {
  return cs_op_arithmetic_microstepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)], true);
}

void cs_op_cp_stepper(cs2010 *cs) {
  unsigned char a = *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char b = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
  cs_perform_arithmetic_op(cs, a, b, true);
  cs_fetch(cs);
}

void cs_op_mov_stepper(cs2010 *cs) {
  cs->registers.ac = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
  *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.ac;
  cs_fetch(cs);
}

void cs_op_mov_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = *cs->registers.regfile[CS_GET_REG_B(cs->registers.ir)];
    cs_microfetch(cs);
    break;
  case 1:
  default:
    *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

void cs_op_clc_stepper(cs2010 *cs) {
  cs->registers.sr &= ~(CS_SR_C);
  cs_fetch(cs);
}

void cs_op_sec_stepper(cs2010 *cs) {
  cs->registers.sr |= CS_SR_C;
  cs_fetch(cs);
}

static void cs_op_set_ror_flags(cs2010 *cs, unsigned char r, unsigned char a_7,
                                unsigned char a_0, unsigned char c_in) {
  /* bit 76543210
  SR  =  0000VNZC   where
  V: 2's complement overflow
  N: negative sign
  Z: zero
  C: carry (unsigned overflow) */

  cs->registers.sr =
      (a_7 ^ c_in) << CS_SR_V_OFFSET |
      c_in << CS_SR_N_OFFSET | /* same as BIT_AT(r, 7) << CS_SR_N_OFFSET */
      (!r) << CS_SR_Z_OFFSET | a_0 << CS_SR_C_OFFSET;
}

void cs_op_ror_stepper(cs2010 *cs) {
  unsigned char *dst_register =
      cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char a_7 = BIT_AT(*dst_register, 7);
  unsigned char a_0 = BIT_AT(*dst_register, 0);
  unsigned char c_in = BIT_AT(cs->registers.sr, CS_SR_C_OFFSET);
  cs->registers.ac = (*dst_register >> 1) | (c_in << 7);
  cs_op_set_ror_flags(cs, cs->registers.ac, a_7, a_0, c_in);
  *dst_register = cs->registers.ac;
  cs_fetch(cs);
}

void cs_op_ror_microstepper(cs2010 *cs) {
  unsigned char *dst_register =
      cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char a_7;
  unsigned char a_0;
  unsigned char c_in;

  switch (cs->microop) {
  case 0:
    a_7 = BIT_AT(*dst_register, 7);
    a_0 = BIT_AT(*dst_register, 0);
    c_in = BIT_AT(cs->registers.sr, CS_SR_C_OFFSET);
    cs->registers.ac = (*dst_register >> 1) | (c_in << 7);
    cs_op_set_ror_flags(cs, cs->registers.ac, a_7, a_0, c_in);
    cs_microfetch(cs);
    break;
  case 1:
  default:
    *dst_register = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

static void cs_op_set_rol_flags(cs2010 *cs, unsigned char r, unsigned char a_7,
                                unsigned char a_6, unsigned char c_in) {
  /* bit 76543210
  SR  =  0000VNZC   where
  V: 2's complement overflow
  N: negative sign
  Z: zero
  C: carry (unsigned overflow) */

  cs->registers.sr =
      (a_7 ^ a_6) << CS_SR_V_OFFSET |
      a_6 << CS_SR_N_OFFSET | /* same as BIT_AT(r, 7) << CS_SR_N_OFFSET */
      (!r) << CS_SR_Z_OFFSET | a_7 << CS_SR_C_OFFSET;
}

void cs_op_rol_stepper(cs2010 *cs) {
  unsigned char *dst_register =
      cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char a_7 = BIT_AT(*dst_register, 7);
  unsigned char a_6 = BIT_AT(*dst_register, 6);
  unsigned char c_in = BIT_AT(cs->registers.sr, CS_SR_C_OFFSET);
  cs->registers.ac = (*dst_register << 1) | c_in;
  cs_op_set_rol_flags(cs, cs->registers.ac, a_7, a_6, c_in);
  *dst_register = cs->registers.ac;
  cs_fetch(cs);
}

void cs_op_rol_microstepper(cs2010 *cs) {
  unsigned char *dst_register =
      cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char a_7;
  unsigned char a_6;
  unsigned char c_in;

  switch (cs->microop) {
  case 0:
    a_7 = BIT_AT(*dst_register, 7);
    a_6 = BIT_AT(*dst_register, 6);
    c_in = BIT_AT(cs->registers.sr, CS_SR_C_OFFSET);
    cs->registers.ac = (*dst_register << 1) | c_in;
    cs_op_set_rol_flags(cs, cs->registers.ac, a_7, a_6, c_in);
    cs_microfetch(cs);
    break;
  case 1:
  default:
    *dst_register = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}

void cs_op_stop_stepper(cs2010 *cs) { cs->stopped = true; }

void cs_op_addi_stepper(cs2010 *cs) {
  return cs_op_arithmetic_stepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      CS_GET_ARG_B(cs->registers.ir), false);
}

void cs_op_addi_microstepper(cs2010 *cs) {
  return cs_op_arithmetic_microstepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      CS_GET_ARG_B(cs->registers.ir), false);
}

void cs_op_subi_stepper(cs2010 *cs) {
  return cs_op_arithmetic_stepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      CS_GET_ARG_B(cs->registers.ir), true);
}

void cs_op_subi_microstepper(cs2010 *cs) {
  return cs_op_arithmetic_microstepper(
      cs, cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)],
      CS_GET_ARG_B(cs->registers.ir), true);
}

void cs_op_cpi_stepper(cs2010 *cs) {
  unsigned char a = *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)];
  unsigned char b = CS_GET_ARG_B(cs->registers.ir);
  cs_perform_arithmetic_op(cs, a, b, true);
  cs_fetch(cs);
}

void cs_op_ldi_stepper(cs2010 *cs) {
  cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
  *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.ac;
  cs_fetch(cs);
}

void cs_op_ldi_microstepper(cs2010 *cs) {
  switch (cs->microop) {
  case 0:
    cs->registers.ac = CS_GET_ARG_B(cs->registers.ir);
    cs_microfetch(cs);
    break;
  case 1:
  default:
    *cs->registers.regfile[CS_GET_REG_A(cs->registers.ir)] = cs->registers.ac;
    cs_fetch(cs);
    break;
  }
}
