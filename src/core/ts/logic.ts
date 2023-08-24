import {
  $getValue,
  Jasm,
  JasmType,
  TJasmObjectConstructor,
  TJasmObjectProxy,
} from '@guillex7/jasm';
import wasm2010 from '../wasm/bin/WASM2010.wasm';
import { Asm2010IoManager, ICsIoHandler } from './io';
import {
  CS_RAM_SIZE,
  CS_ROM_SIZE,
  CS_SIGNAL_OFFSET,
  CsPlatform,
  IAsm2010Exports,
  TCsAssemblyCode,
  TCsSignals,
  TCsStatus,
  csAsMachineCodeStruct,
  csMachineStruct,
} from './types';

export class Asm2010InstanceBuilder {
  private module: WebAssembly.Module | null = null;

  private getInternalAsm2010Imports(asm2010IoManager: Asm2010IoManager) {
    return {
      env: asm2010IoManager.getInternalCsIoFunctions(),
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
  }

  public async createInstance(
    csPlatform?: CsPlatform
  ): Promise<Asm2010Instance> {
    if (this.module === null) {
      this.module = await WebAssembly.compileStreaming(fetch(wasm2010));
    }

    const asm2010InstanceIoManager = new Asm2010IoManager();
    const asm2010InstanceModule = await WebAssembly.instantiate(
      this.module,
      this.getInternalAsm2010Imports(asm2010InstanceIoManager)
    );
    const asm2010InstanceExports =
      asm2010InstanceModule.exports as unknown as IAsm2010Exports;

    return new Asm2010Instance(
      asm2010InstanceExports,
      asm2010InstanceIoManager,
      csPlatform
    );
  }
}

export class Asm2010Instance {
  private jasm: Jasm;

  private csAsMachineCodeType: TJasmObjectConstructor;
  private csMachineType: TJasmObjectConstructor;

  private csMachinePointer: number;
  private csMachine: TJasmObjectProxy;

  constructor(
    private exports: IAsm2010Exports,
    private ioManager: Asm2010IoManager,
    private csPlatform = CsPlatform.Cs2010
  ) {
    this.jasm = new Jasm(exports.memory);

    this.csAsMachineCodeType = this.jasm.create({
      type: csAsMachineCodeStruct,
    });
    this.csMachineType = this.jasm.create({ type: csMachineStruct });

    this.csMachinePointer = 0;
    this.csMachine = this.csMachineType.at(this.csMachinePointer);

    this.switchCsPlatform(csPlatform);
  }

  public switchCsPlatform(csPlatform: CsPlatform): void {
    if (this.csMachinePointer) {
      this.exports.cs_free(this.csMachinePointer);
    }

    this.csPlatform = csPlatform;
    this.csMachinePointer = this.exports.cs_create();
    this.csMachine = this.csMachineType.at(this.csMachinePointer);
    this.exports.cs_init(this.csMachinePointer, this.csPlatform);
    this.exports.wasm_set_custom_io_functions(this.csMachinePointer);
  }

  public asAssemble(sourceAssembly: string): TCsAssemblyCode {
    const sourceAssemblyPointer = this.exports.malloc(
      sourceAssembly.length + 1
    );
    this.jasm.copyString(sourceAssemblyPointer, sourceAssembly);

    const parsingInfoPointer = this.exports.cs_as_parse_create();

    this.exports.cs_as_parse_init(
      parsingInfoPointer,
      CS_ROM_SIZE,
      this.csPlatform
    ),
      this.exports.cs_as_parse_source(
        parsingInfoPointer,
        sourceAssemblyPointer,
        false
      );
    this.exports.cs_as_parse_assemble(parsingInfoPointer, false);

    const csAsMachineCode = this.csAsMachineCodeType.at(
      this.exports.cs_as_get_machine_code(parsingInfoPointer)
    );
    const log = this.jasm.readString(
      this.exports.cs_as_parse_get_log(parsingInfoPointer)
    );

    const machineInstructionsAmount: number =
      csAsMachineCode().machineInstructionsAmount[$getValue]();
    const machineInstructions: number[] = this.jasm.ushort.getArray(
      csAsMachineCode().machineInstructions[$getValue](),
      machineInstructionsAmount
    );
    const matchingSourceLines: number[] = this.jasm.size_t.getArray(
      csAsMachineCode().matchingSourceLines[$getValue](),
      machineInstructionsAmount
    );

    this.exports.cs_as_parse_free(parsingInfoPointer);
    this.exports.free(sourceAssemblyPointer);

    return {
      assembledInstructions: machineInstructions.map(
        (machineInstruction, i) => ({
          machineInstruction,
          matchingSourceLine: matchingSourceLines[i],
        })
      ),
      log,
    };
  }

  public asDisassemble(machineInstructions: number[]): string[] {
    if (!machineInstructions.length) {
      return [];
    }

    const machineInstructionsPointer = this.exports.malloc(
      Jasm.getTypeData(JasmType.ushort).size * machineInstructions.length
    );
    this.jasm.ushort.setArray(machineInstructionsPointer, machineInstructions);

    const disassembledInstructionsPointer =
      this.exports.cs_as_disassemble_instructions(
        machineInstructionsPointer,
        machineInstructions.length,
        this.csPlatform
      );

    const disassembledInstructions = this.jasm.readString(
      disassembledInstructionsPointer
    );

    this.exports.free(disassembledInstructionsPointer);
    this.exports.free(machineInstructionsPointer);

    return disassembledInstructions.split('\n');
  }

  public csLoadProgram(machineInstructions: number[]): void {
    const machineInstructionsPointer = this.exports.malloc(
      Jasm.getTypeData(JasmType.ushort).size * machineInstructions.length
    );
    this.jasm.ushort.setArray(machineInstructionsPointer, machineInstructions);

    this.exports.cs_load_machine_instructions(
      this.csMachinePointer,
      machineInstructionsPointer,
      machineInstructions.length
    );

    this.exports.free(machineInstructionsPointer);
  }

  public csMicroStep(): void {
    this.exports.cs_microstep(this.csMachinePointer);
  }

  public csFullStep(): void {
    this.exports.cs_fullstep(this.csMachinePointer);
  }

  public csBlockStep(maxInstructions: number): boolean {
    return this.exports.cs_blockstep(this.csMachinePointer, maxInstructions);
  }

  public csSoftReset(): void {
    this.exports.cs_soft_reset(this.csMachinePointer);
  }

  public csHardReset(): void {
    this.exports.cs_hard_reset(this.csMachinePointer, false);
  }

  public csGetStatus(): TCsStatus {
    const signals: number = this.csMachine().signals[$getValue]();

    return {
      ram: this.jasm.uchar.getArray(
        this.csMachine().memory.ram[$getValue](),
        CS_RAM_SIZE
      ),
      registers: this.csMachine().registers[$getValue](),
      signals: Object.entries(CS_SIGNAL_OFFSET).reduce(
        (acc, [signalName, signalOffset]) => ({
          ...acc,
          [signalName]: signals & signalOffset,
        }),
        {}
      ) as TCsSignals,
      microop: this.csMachine().microop[$getValue](),
      isStopped: this.csMachine().stopped[$getValue](),
    };
  }

  public csRegisterIoHandlers(ioHandlers: Record<number, ICsIoHandler>): void {
    this.ioManager.registerHandlers(ioHandlers);
  }
}
