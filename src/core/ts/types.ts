import { Jasm, JasmType } from '@guillex7/jasm';

/* Definitions from ASM2010 that enable interoperability
  between it and WASM2010 */

export const CS_ROM_SIZE = 256;
export const CS_RAM_SIZE = 256;

export const CS_IO_READ_NOT_CONTROLLED = 0x100;
export const CS_IO_WRITE_NOT_CONTROLLED = 0;
export const CS_IO_WRITE_CONTROLLED = 1;

export const csAsMachineCodeStruct = Jasm.struct({
  members: [
    { name: 'machineInstructionsAmount', type: JasmType.size_t },
    {
      name: 'machineInstructions',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.ushort },
    },
    {
      name: 'matchingSourceLines',
      type: JasmType.pointer,
      pointsTo: { type: JasmType.size_t },
    },
  ],
});

export const csMemoryStruct = Jasm.struct({
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

export const csRegistersStruct = Jasm.struct({
  members: [
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

export const csMachineStruct = Jasm.struct({
  members: [
    { name: 'memory', type: csMemoryStruct },
    { name: 'registers', type: csRegistersStruct },
    { name: 'signals', type: JasmType.ulong },
    { name: 'io_read_fn', type: JasmType.pointer },
    { name: 'io_write_fn', type: JasmType.pointer },
    { name: 'opcodes', type: JasmType.pointer },
    { name: 'platform', type: JasmType.uchar },
    { name: 'microop', type: JasmType.uchar },
    { name: 'stopped', type: JasmType.bool },
  ],
});

export type TCsAssemblyInstruction = {
  machineInstruction: number;
  matchingSourceLine: number;
};

export type TCsAssemblyCode = {
  assembledInstructions: TCsAssemblyInstruction[];
  log: string;
};

export enum CsRegister {
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

export enum CsSignal {
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
  IMPL0 = 'impl0',
  IMPL1 = 'impl1',
  IMPL2 = 'impl2',
  IMPL3 = 'impl3',
  WREG = 'wreg',
}

export enum Cs2010Signal {
  ALUOP0 = 'aluop0',
  ALUOP1 = 'aluop1',
  ALUOP2 = 'aluop2',
  ALUOP3 = 'aluop3',
}
export enum Cs3Signal {
  ALUS = 'alus',
  ALUR = 'alur',
  ALUTA = 'aluta',
  ALUTB = 'alutb',
}

export const CS_SIGNAL_OFFSET: Record<CsSignal, number> = {
  [CsSignal.WMAR]: 1 << 0,
  [CsSignal.WMDR]: 1 << 1,
  [CsSignal.IOMDR]: 1 << 2,
  [CsSignal.WMEM]: 1 << 3,
  [CsSignal.RMEM]: 1 << 4,
  [CsSignal.WIR]: 1 << 5,
  [CsSignal.WPC]: 1 << 6,
  [CsSignal.RPC]: 1 << 7,
  [CsSignal.CPC]: 1 << 8,
  [CsSignal.IPC]: 1 << 9,
  [CsSignal.ISP]: 1 << 10,
  [CsSignal.DSP]: 1 << 11,
  [CsSignal.CSP]: 1 << 12,
  [CsSignal.RSP]: 1 << 13,
  [CsSignal.INM]: 1 << 14,
  [CsSignal.SRW]: 1 << 15,
  [CsSignal.WAC]: 1 << 16,
  [CsSignal.RAC]: 1 << 17,
  [CsSignal.IMPL0]: 1 << 18,
  [CsSignal.IMPL1]: 1 << 19,
  [CsSignal.IMPL2]: 1 << 20,
  [CsSignal.IMPL3]: 1 << 21,
  [CsSignal.WREG]: 1 << 22,
};

export type TCsRegisters = Record<CsRegister, number>;

export type TCsSignals = Record<CsSignal, number>;

export type TCsStatus = {
  ram: number[];
  registers: TCsRegisters;
  signals: TCsSignals;
  microop: number;
  isStopped: boolean;
};

export enum CsInitStatus {
  Ok = 0,
  NotEnoughMemory = 1,
  InvalidPlatform = 2,
}

export enum CsLoadStatus {
  Ok = 0,
  Failed = 1,
  NotEnoughROM = 2,
  InvalidInstructions = 3,
}

export enum CsPlatform {
  Cs2010 = 1,
  Cs3 = 2,
}

export enum CsAsParseInitStatus {
  Ok = 0,
  NotEnoughMemory = 1,
  InvalidPlatform = 2,
}

export enum CsAsParseStatus {
  Ok = 0,
  Warning = 1,
  Error = 2,
}

export interface IAsm2010Exports {
  /* Memory */
  memory: WebAssembly.Memory;
  /* ASParse */
  cs_as_parse_create(): number;
  cs_as_parse_init(
    parsing_info_ptr: number,
    max_parsed_sentences_amount: number,
    platform: CsPlatform
  ): CsAsParseInitStatus;
  cs_as_parse_source(
    parsing_info_ptr: number,
    source_ptr: number,
    stop_on_error: boolean
  ): CsAsParseStatus;
  cs_as_parse_assemble(
    parsing_info_ptr: number,
    stop_on_error: boolean
  ): CsAsParseStatus;
  cs_as_get_machine_code(parsing_info_ptr: number): number;
  cs_as_parse_get_log(parsing_info_ptr: number): number;
  cs_as_disassemble_instructions(
    machine_instructions_ptr: number,
    machine_instructions_amount: number,
    platform: CsPlatform
  ): number;
  cs_as_parse_free(parsing_info_ptr: number): void;
  /* CS2010 */
  cs_create(): number;
  cs_init(cs_ptr: number, platform: CsPlatform): number;
  cs_load_machine_instructions(
    cs_ptr: number,
    machine_instructions_ptr: number,
    machine_instructions_amount: number
  ): CsLoadStatus;
  cs_microstep(cs_ptr: number): void;
  cs_fullstep(cs_ptr: number): void;
  cs_blockstep(cs_ptr: number, max_instructions: number): boolean;
  cs_hard_reset(cs_ptr: number, clear_rom: boolean): void;
  cs_soft_reset(cs_ptr: number): void;
  cs_free(cs_ptr: number): void;
  /* Others */
  malloc(size: number): number;
  free(ptr: number): void;
}

export type TAsm2010IoImports = {
  wasm_custom_io_read(address: number): number;
  wasm_custom_io_write(address: number, content: number): number;
};

export type TAsm2010Imports = {
  env: TAsm2010IoImports;
  wasi_snapshot_preview1: {
    fd_close: () => void;
    fd_seek: () => void;
    fd_write: () => void;
  };
};
