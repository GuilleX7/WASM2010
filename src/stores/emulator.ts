import { defineStore } from 'pinia';
import { restoreStoreState, saveStoreState } from '.';
import { IoComponentId } from '@/components/emulator/modules/io';

export type TEmulatorStoreState = {
  clockRunningFrequency: number;
  uiRefreshFrequency: number;
  skipMicroinstructions: boolean;
  maxInstructionsBeforeHaltingBlockStep: number;
  romDisplayableRadix: number;
  registerDisplayableRadix: number;
  ramDisplayableRadix: number;
  ramWordsPerRow: number;
  reduceVisualMotion: boolean;
  mappedIoComponents: Record<number, IoComponentId>;
};

export const getEmulatorStoreDefaultState = (): TEmulatorStoreState => ({
  clockRunningFrequency: 1000,
  uiRefreshFrequency: 24,
  skipMicroinstructions: false,
  maxInstructionsBeforeHaltingBlockStep: 10000,
  romDisplayableRadix: 16,
  registerDisplayableRadix: 16,
  ramDisplayableRadix: 16,
  ramWordsPerRow: 16,
  reduceVisualMotion: true,
  mappedIoComponents: {
    0: IoComponentId.HexDisplay,
    1: IoComponentId.Buttons,
    2: IoComponentId.Keyboard,
    3: IoComponentId.RandomGenerator,
  },
});

export const useEmulatorStore = defineStore('emulator', {
  state: () => getEmulatorStoreDefaultState(),
  actions: {
    async init(): Promise<void> {
      const savedState = restoreStoreState(this.$id);
      if (savedState) {
        this.$patch(savedState);
      } else {
        this.$reset();
      }

      this.$subscribe((_, state) => saveStoreState(this.$id, state));
    },
  },
});
