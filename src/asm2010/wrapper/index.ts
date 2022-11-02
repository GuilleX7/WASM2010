import {
  Jasm,
  JasmType,
  TJasmObjectConstructor,
  $getAddress,
  $getValue,
  $deref,
  TJasmObjectProxy,
} from '@guillex7/jasm';
import asm2010 from '../build/asm2010.wasm';
import { wasm_custom_io_read, wasm_custom_io_write } from './io';

const imports = {
  env: {
    wasm_custom_io_read,
    wasm_custom_io_write,
  },
  wasi_snapshot_preview1: {
    fd_close: () => {
      /* Stub method */
    },
    fd_seek: () => {
      /* Stub method */
    },
    fd_write: () => {
      /* Stub method */
    },
  },
};

const hash_table = Jasm.struct({
  members: [
    { name: 'size_index', type: JasmType.size_t },
    { name: 'entry_count', type: JasmType.size_t },
    { name: 'entries', type: JasmType.pointer },
  ],
});

const trace_log = Jasm.struct({
  members: [
    {
      name: 'trace',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.char },
    },
    {
      name: 'log',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.char },
    },
    { name: 'maximum_trace_space', type: JasmType.size_t },
    { name: 'available_log_space', type: JasmType.size_t },
    { name: 'maximum_log_space', type: JasmType.size_t },
  ],
});

const as_parse_assembled_code = Jasm.struct({
  members: [
    { name: 'parsing_line_index', type: JasmType.size_t },
    { name: 'machine_code', type: JasmType.ushort },
  ],
});

const as_parse_info = Jasm.struct({
  members: [
    { name: 'equs_ht', type: hash_table },
    { name: 'log', type: trace_log },
    { name: 'parsing_line_index', type: JasmType.size_t },
    { name: 'max_sentences', type: JasmType.size_t },
    { name: 'sentences', type: JasmType.pointer },
    { name: 'sentence_index', type: JasmType.size_t },
    {
      name: 'assembled_code',
      type: JasmType.pointer,
      pointsTo: { type: as_parse_assembled_code },
    },
    { name: 'assembled_code_index', type: JasmType.size_t },
  ],
});

export const CS_ROM_SIZE = 256;
export const CS_RAM_SIZE = 256;

const cs_memory = Jasm.struct({
  members: [
    {
      name: 'rom',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.ushort, length: CS_ROM_SIZE },
    },
    {
      name: 'ram',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.uchar, length: CS_RAM_SIZE },
    },
  ],
});

const cs_registers = Jasm.struct({
  members: [
    { name: 'signals', type: JasmType.ulong },
    { name: 'regfile', type: JasmType.pointer, length: 8 },
    { name: 'ir', type: JasmType.ushort },
    { name: 'r0', type: JasmType.uchar },
    { name: 'r1', type: JasmType.uchar },
    { name: 'r2', type: JasmType.uchar },
    { name: 'r3', type: JasmType.uchar },
    { name: 'r4', type: JasmType.uchar },
    { name: 'r5', type: JasmType.uchar },
    { name: 'r6', type: JasmType.uchar },
    { name: 'r7', type: JasmType.uchar },
    { name: 'sp', type: JasmType.uchar },
    { name: 'pc', type: JasmType.uchar },
    { name: 'ac', type: JasmType.uchar },
    { name: 'sr', type: JasmType.uchar },
    { name: 'mdr', type: JasmType.uchar },
    { name: 'mar', type: JasmType.uchar },
  ],
});

const cs2010 = Jasm.struct({
  members: [
    { name: 'memory', type: cs_memory },
    { name: 'registers', type: cs_registers },
    { name: 'microop', type: JasmType.uchar },
    { name: 'is_stopped', type: JasmType.bool },
  ],
});

enum CS_LOAD_PROGRAM_RESULT {
  SUCCESS = 0,
  FAILED = 1,
  NOT_ENOUGH_ROM = 2,
  INVALID_INSTRUCTIONS = 3,
}

let malloc: (size: number) => number;
let free: (ptr: number) => void;
let as_parse_init: (parsing_info: number, max_sentences: number) => boolean;
let as_parse_source: (
  parsing_info: number,
  source: number,
  stop_on_error: boolean
) => number;
let as_parse_assemble: (parsing_info: number, stop_on_error: boolean) => number;
let as_parse_get_log: (parsing_info: number) => number;
let as_disassemble_sentence: (raw_sentence: number) => number;
let as_parse_free: (parsing_info: number) => void;
let cs_init: (cs: number) => boolean;
let wasm_set_custom_io_functions: (cs: number) => void;
let cs_hard_reset: (cs: number, clear_rom: boolean) => void;
let cs_soft_reset: (cs: number) => void;
let cs_load_program: (
  cs: number,
  sentences: number,
  sentences_length: number
) => CS_LOAD_PROGRAM_RESULT;
let cs_microstep: (cs: number) => void;
let cs_fullstep: (cs: number) => void;
let cs_blockstep: (cs: number, max_instructions: number) => boolean;

let jasm: Jasm;
let as_parse_info_t: TJasmObjectConstructor;
let cs2010_t: TJasmObjectConstructor;
let parsing_info: TJasmObjectProxy;
let cs: TJasmObjectProxy;
let cs_ptr: number;

export type TAsAssembledCode = {
  machineCode: number;
  matchingSourceLine: number;
};

export type TAsParseResult = {
  assembledCode: TAsAssembledCode[];
  log: string;
};

export enum CsRegisterName {
  IR = 'ir',
  R0 = 'r0',
  R1 = 'r1',
  R2 = 'r2',
  R3 = 'r3',
  R4 = 'r4',
  R5 = 'r5',
  R6 = 'r6',
  R7 = 'r7',
  SP = 'sp',
  PC = 'pc',
  AC = 'ac',
  SR = 'sr',
  MDR = 'mdr',
  MAR = 'mar',
}

export enum CsSignalName {
  WMAR = 'wmar',
  WMDR = 'wmdr',
  IOMDR = 'iomdr',
  WMEM = 'wmem',
  RMEM = 'rmem',
  WIR = 'wir',
  WPC = 'wpc',
  RPC = 'rpc',
  CPC = 'cpc',
  IPC = 'ipc',
  ISP = 'isp',
  DSP = 'dsp',
  CSP = 'csp',
  RSP = 'rsp',
  INM = 'inm',
  SRW = 'srw',
  WAC = 'wac',
  RAC = 'rac',
  ALUOP0 = 'aluop0',
  ALUOP1 = 'aluop1',
  ALUOP2 = 'aluop2',
  ALUOP3 = 'aluop3',
  WREG = 'wreg',
}

const CsSignalOffset: Record<CsSignalName, number> = {
  [CsSignalName.WMAR]: 1 << 0,
  [CsSignalName.WMDR]: 1 << 1,
  [CsSignalName.IOMDR]: 1 << 2,
  [CsSignalName.WMEM]: 1 << 3,
  [CsSignalName.RMEM]: 1 << 4,
  [CsSignalName.WIR]: 1 << 5,
  [CsSignalName.WPC]: 1 << 6,
  [CsSignalName.RPC]: 1 << 7,
  [CsSignalName.CPC]: 1 << 8,
  [CsSignalName.IPC]: 1 << 9,
  [CsSignalName.ISP]: 1 << 10,
  [CsSignalName.DSP]: 1 << 11,
  [CsSignalName.CSP]: 1 << 12,
  [CsSignalName.RSP]: 1 << 13,
  [CsSignalName.INM]: 1 << 14,
  [CsSignalName.SRW]: 1 << 15,
  [CsSignalName.WAC]: 1 << 16,
  [CsSignalName.RAC]: 1 << 17,
  [CsSignalName.ALUOP0]: 1 << 18,
  [CsSignalName.ALUOP1]: 1 << 19,
  [CsSignalName.ALUOP2]: 1 << 20,
  [CsSignalName.ALUOP3]: 1 << 21,
  [CsSignalName.WREG]: 1 << 22,
};

export type TCsRegisters = {
  [key in CsRegisterName]: number;
};

export type TCsSignals = {
  [key in CsSignalName]: number;
};

export type TCsStatus = {
  ram: number[];
  registers: TCsRegisters;
  signals: TCsSignals;
  microop: number;
  isStopped: boolean;
};

export async function loadAsm2010(): Promise<void> {
  const {
    instance: { exports },
  } = await WebAssembly.instantiateStreaming(fetch(asm2010), imports);

  as_parse_init = exports.as_parse_init as typeof as_parse_init;
  as_parse_source = exports.as_parse_source as typeof as_parse_source;
  as_parse_assemble = exports.as_parse_assemble as typeof as_parse_assemble;
  as_parse_free = exports.as_parse_free as typeof as_parse_free;
  as_disassemble_sentence =
    exports.as_disassemble_sentence as typeof as_disassemble_sentence;
  as_parse_get_log = exports.as_parse_get_log as typeof as_parse_get_log;
  cs_init = exports.cs_init as typeof cs_init;
  cs_hard_reset = exports.cs_hard_reset as typeof cs_hard_reset;
  cs_soft_reset = exports.cs_soft_reset as typeof cs_soft_reset;
  cs_load_program = exports.cs_load_program as typeof cs_load_program;
  cs_microstep = exports.cs_microstep as typeof cs_microstep;
  cs_fullstep = exports.cs_fullstep as typeof cs_fullstep;
  cs_blockstep = exports.cs_blockstep as typeof cs_blockstep;
  wasm_set_custom_io_functions =
    exports.wasm_set_custom_io_functions as typeof wasm_set_custom_io_functions;
  malloc = exports.malloc as typeof malloc;
  free = exports.free as typeof free;

  jasm = new Jasm(exports.memory as WebAssembly.Memory);
  as_parse_info_t = jasm.create({ type: as_parse_info });
  cs2010_t = jasm.create({ type: cs2010 });
  parsing_info = as_parse_info_t.at(malloc(as_parse_info.size));
  cs = cs2010_t.at(malloc(cs2010.size));
  cs_ptr = cs()[$getAddress]();
  cs_init(cs_ptr);
  wasm_set_custom_io_functions(cs_ptr);
}

export function asAssemble(sourceAssembly: string): TAsParseResult {
  const assembledCode: TAsAssembledCode[] = [];

  const finalSourceAssembly = `${sourceAssembly}\n`;
  const src_code_ptr = malloc(finalSourceAssembly.length);
  jasm.copyString(src_code_ptr, finalSourceAssembly);

  as_parse_init(parsing_info()[$getAddress](), CS_ROM_SIZE);
  as_parse_source(parsing_info()[$getAddress](), src_code_ptr, false);
  as_parse_assemble(parsing_info()[$getAddress](), false);

  const assembledCodeSize = parsing_info().assembled_code_index[$getValue]();
  for (let i = 0; i < assembledCodeSize; i++) {
    const assembledCodeLine = parsing_info()
      .assembled_code[$deref](i)
      [$getValue]();
    assembledCode[i] = {
      machineCode: assembledCodeLine.machine_code,
      matchingSourceLine: assembledCodeLine.parsing_line_index,
    };
  }
  const log = jasm.readString(as_parse_get_log(parsing_info()[$getAddress]()));

  as_parse_free(parsing_info()[$getAddress]());
  free(src_code_ptr);

  return {
    assembledCode: assembledCode,
    log,
  };
}

export function asDisassemble(machineCode: number): string {
  const pointerToDisassembly = as_disassemble_sentence(machineCode);
  const disassembly = jasm.readString(pointerToDisassembly);
  free(pointerToDisassembly);
  return disassembly;
}

export function csLoadProgram(machineCode: number[]): void {
  const machine_code_ptr = malloc(
    machineCode.length * Jasm.getTypeData(JasmType.ushort).size
  );
  jasm.ushort.setArray(machine_code_ptr, machineCode);
  cs_load_program(cs_ptr, machine_code_ptr, machineCode.length);
  free(machine_code_ptr);
}

export function csMicroStep(): void {
  cs_microstep(cs_ptr);
}

export function csFullStep(): void {
  cs_fullstep(cs_ptr);
}

export function csBlockStep(maxInstructions: number): boolean {
  return cs_blockstep(cs_ptr, maxInstructions);
}

export function csSoftReset(): void {
  cs_soft_reset(cs_ptr);
}

export function csHardReset(): void {
  cs_hard_reset(cs_ptr, false);
}

export function csGetStatus(): TCsStatus {
  const registers = cs().registers[$getValue]();
  return {
    ram: jasm.uchar.getArray(cs().memory.ram[$getValue](), CS_RAM_SIZE),
    registers,
    signals: Object.entries(CsSignalOffset).reduce(
      (acc, [signalName, signalOffset]) => ({
        ...acc,
        [signalName]: registers.signals & signalOffset,
      }),
      {}
    ) as TCsSignals,
    microop: cs().microop[$getValue](),
    isStopped: cs().is_stopped[$getValue](),
  };
}
