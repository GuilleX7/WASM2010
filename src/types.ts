import { IoComponentId } from './components/emulator/io';
import { ICsIoHandler } from './wasm/asm2010/io';

export type TEmulatorSettings = {
  clockRunningFrequency: number;
  uiRefreshFrequency: number;
  skipMicroinstructions: boolean;
  maxInstructionsBeforeHaltingBlockStep: number;
  romDisplayableRadix: number;
  registerDisplayableRadix: number;
  ramDisplayableRadix: number;
  ramWordsPerRow: number;
  mappedIoComponents: Record<number, IoComponentId>;
};

export interface IUiIoController<I, O> extends ICsIoHandler {
  uiUpdateState(input: I): void;
  uiGetState(): O;
}

export type TCtor<C> = new (...args: any) => C;
