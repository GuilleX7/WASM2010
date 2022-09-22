import { IoComponentId } from "../io";

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
