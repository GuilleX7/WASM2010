import {
    Jasm,
    JasmType,
    TJasmObjectConstructor,
    $getAddress,
    $getValue,
    $deref,
    TJasmObjectProxy,
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

const CS_ROM_SIZE = 256;

let malloc: (size: number) => number;
let free: (ptr: number) => void;
let as_parse_init: (pinfo: number, max_sentences: number) => boolean;
let as_parse_source: (
    pinfo: number,
    source: number,
    stop_on_error: boolean
) => number;
let as_parse_assemble: (pinfo: number, stop_on_error: boolean) => number;
let as_parse_free: (pinfo: number) => void;
let trace_log_get: (log: number) => number;

let jasm: Jasm;
let as_parse_info_t: TJasmObjectConstructor;
let pinfo: TJasmObjectProxy;

export type TAsAssembledCode = {
    machineCode: number;
    matchingSourceLine: number;
};

export type TAsParseResult = {
    assembledCode: TAsAssembledCode[];
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
    trace_log_get = exports.trace_log_get as any;
    malloc = exports.malloc as any;
    free = exports.free as any;

    jasm = new Jasm(exports.memory as WebAssembly.Memory);
    as_parse_info_t = jasm.create({ type: as_parse_info });
    pinfo = as_parse_info_t.at(malloc(as_parse_info.size));
}

export function asAssemble(sourceAssembly: string): TAsParseResult {
    let assembledCode: TAsAssembledCode[] = [];

    const finalSourceAssembly = `${sourceAssembly}\n`;
    const src_code_ptr = malloc(finalSourceAssembly.length);
    jasm.copyString(src_code_ptr, finalSourceAssembly);

    as_parse_init(pinfo()[$getAddress](), CS_ROM_SIZE);
    as_parse_source(pinfo()[$getAddress](), src_code_ptr, false);
    as_parse_assemble(pinfo()[$getAddress](), false);

    const assembledCodeSize = pinfo().assembled_code_index[$getValue]();
    for (let i = 0; i < assembledCodeSize; i++) {
        let assembledCodeLine = pinfo().assembled_code[$deref](i)[$getValue]();
        assembledCode[i] = {
            machineCode: assembledCodeLine.machine_code,
            matchingSourceLine: assembledCodeLine.parsing_line_index,
        };
    }
    const log = jasm.readString(trace_log_get(pinfo().log[$getAddress]()));

    as_parse_free(pinfo()[$getAddress]());
    free(src_code_ptr);

    return {
        assembledCode: assembledCode,
        log,
    };
}
