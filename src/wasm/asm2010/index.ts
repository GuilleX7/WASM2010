import {
  Jasm,
  JasmType,
  TJasmObjectConstructor,
  $address,
  $value,
} from '@guillex7/jasm';
import asm2010 from './asm2010.wasm';

const imports = {
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
    { name: 'log', type: JasmType.pointer, pointsTo: { type: JasmType.char } },
    { name: 'maximum_trace_space', type: JasmType.size_t },
    { name: 'available_log_space', type: JasmType.size_t },
    { name: 'maximum_log_space', type: JasmType.size_t },
  ],
});

const as_parse_info = Jasm.struct({
  members: [
    { name: 'equs_ht', type: hash_table },
    { name: 'log', type: trace_log },
    { name: 'parsing_line_index', type: JasmType.size_t },
    { name: 'sentence_index', type: JasmType.size_t },
    { name: 'max_sentences', type: JasmType.size_t },
    { name: 'sentences', type: JasmType.pointer },
    { name: 'machine_code', type: JasmType.pointer },
  ],
});

const AS_PARSE_ERROR = 2;
const CS_ROM_SIZE = 256;

let malloc: (size: number) => number;
let free: (ptr: number) => void;
let as_parse_init: (pinfo: number, max_sentences: number) => boolean;
let as_parse_source: (pinfo: number, source: number) => number;
let as_parse_assemble: (pinfo: number) => number;
let as_parse_free: (pinfo: number) => void;
let as_disassemble_sentence: (raw_sentence: number) => number;
let trace_log_get: (log: number) => number;

let jasm: Jasm;
let as_parse_info_t: TJasmObjectConstructor;

export type TAsParseResult = {
  machineCode: number[];
  log: string;
};

export async function loadAsm2010(): Promise<void> {
  const {
    instance: { exports },
  } = await WebAssembly.instantiateStreaming(fetch(asm2010), imports);

  as_parse_init = exports.as_parse_init as any;
  as_parse_source = exports.as_parse_source as any;
  as_parse_assemble = exports.as_parse_assemble as any;
  as_parse_free = exports.as_parse_free as any;
  as_disassemble_sentence = exports.as_disassemble_sentence as any;
  trace_log_get = exports.trace_log_get as any;
  malloc = exports.malloc as any;
  free = exports.free as any;

  jasm = new Jasm(exports.memory as WebAssembly.Memory);
  as_parse_info_t = jasm.create({ type: as_parse_info });
}

export function asmAssemble(sourceAssembly: string): TAsParseResult {
  const finalSourceAssembly = `${sourceAssembly}\n` 
  const src_code = jasm
    .create({
      type: JasmType.char,
    })
    .at(malloc(finalSourceAssembly.length));
  jasm.copyString(src_code()[$address], finalSourceAssembly);

  const pinfo = as_parse_info_t.at(malloc(as_parse_info.size));
  as_parse_init(pinfo()[$address], CS_ROM_SIZE);

  let machineCode: number[] = [];
  if (
    as_parse_source(pinfo()[$address], src_code()[$address]) !==
      AS_PARSE_ERROR &&
    as_parse_assemble(pinfo()[$address]) !== AS_PARSE_ERROR
  ) {
    machineCode = jasm.ushort.getArray(
      pinfo().machine_code[$value],
      pinfo().sentence_index[$value]
    );
  }

  const log = jasm.readString(trace_log_get(pinfo().log[$address]));
  as_parse_free(pinfo()[$address]);
  free(pinfo()[$address]);
  free(src_code()[$address]);

  return {
    machineCode,
    log,
  };
}
