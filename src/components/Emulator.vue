<template>
  <div class="is-fullwidth is-viewheight">
    <b-modal v-model="isSettingsModalVisible" has-modal-card trap-focus>
      <SettingsModal
        :settings="settings"
        @close="isSettingsModalVisible = false"
        @save-changes="onSettingsModalSavedChanges"
      ></SettingsModal>
    </b-modal>
    <div class="is-flex is-flex-direction-column is-fullheight">
      <div class="asm--emulator-menu">
        <b-button
          v-if="!isClockRunning"
          type="is-light"
          icon-left="timer"
          @click="startClock"
          >Start clock</b-button
        >
        <b-button
          v-else
          type="is-light"
          icon-left="pause-octagon"
          @click="stopClock"
        >
          Stop clock
        </b-button>
        <b-button
          type="is-light"
          icon-left="reload"
          :disabled="isClockRunning"
          @click="hardReset"
          >Reset</b-button
        >
        <b-button
          type="is-light"
          icon-left="cog"
          :disabled="isClockRunning"
          @click="isSettingsModalVisible = true"
          >Settings</b-button
        >
      </div>
      <div class="asm--emulator-container">
        <div class="asm--emulator-rom has-background-white">
          <Rom
            :memory="rom"
            :highlightLineIdx="currentFetchedInstructionIdx"
            :displayableRadix="settings.romDisplayableRadix"
            :isStopped="stopped"
          ></Rom>
        </div>
        <div class="asm--emulator-registers has-background-white">
          <Registers
            :registers="registers"
            :displayableRadix="settings.registerDisplayableRadix"
          ></Registers>
        </div>
        <div class="asm--emulator-ram has-background-white">
          <Ram
            :memory="ram"
            :highlightWordIdx="registers.mar"
            :displayableRadix="settings.ramDisplayableRadix"
            :wordsPerRow="settings.ramWordsPerRow"
          ></Ram>
        </div>
        <div class="asm--emulator-signals has-background-white">
          <Signals :signals="signals" :signalsPerRow="4"></Signals>
        </div>
        <div class="asm--emulator-io has-background-white">
          <Io
            :mappedComponents="mappedIoComponents"
            :uiClockTick="lastUiTick"
          ></Io>
        </div>
      </div>
      <div class="asm--emulator-statusbar">
        <b-button
          type="is-light"
          icon-left="debug-step-into"
          :disabled="isClockRunning"
          @click="microStep"
          >Step microop</b-button
        >
        <b-button
          type="is-light"
          icon-left="debug-step-over"
          :disabled="isClockRunning"
          @click="fullStep"
          >Step op</b-button
        >
        <b-button
          type="is-light"
          icon-left="debug-step-out"
          :disabled="isClockRunning"
          @click="blockStep"
          >Step block</b-button
        >
      </div>
    </div>
  </div>
</template>

<style lang="scss">
.asm--emulator-menu {
  display: flex;
  align-items: center;
  height: 40px;
  flex-shrink: 0;
}

.asm--emulator-container {
  display: grid;
  height: 100%;
  grid-template-columns: repeat(3, 1fr);
  grid-template-rows: repeat(3, 1fr);
  grid-gap: 0.75rem;
  padding-right: 0.75rem;
  overflow: hidden;

  .asm--emulator-rom {
    grid-column: 1 / 2;
    grid-row: 1 / 3;
  }

  .asm--emulator-registers {
    grid-column: 2 / 3;
    grid-row: 1 / 3;
  }

  .asm--emulator-ram {
    grid-column: 1 / 3;
    grid-row: 3 / 4;
    overflow: hidden;
  }

  .asm--emulator-signals {
    grid-column: 3 / 4;
    grid-row: 1 / 2;
    overflow: hidden;
  }

  .asm--emulator-io {
    grid-column: 3 / 4;
    grid-row: 2 / 4;
    overflow: hidden;
  }
}

.asm--emulator-statusbar {
  width: 100%;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  transition: all 0.25s;
  height: 40px;
}
</style>

<script lang="ts">
import {
  csBlockStep,
  csFullStep,
  csGetStatus,
  csHardReset,
  csLoadAndStart,
  csMicroStep,
  CsRegisterName,
  CsSignalName,
  CS_RAM_SIZE,
  CS_ROM_SIZE,
  TAsAssembledCode,
  TCsRegisters,
  TCsSignals,
  TCsStatus,
} from '@/wasm/asm2010';
import { defineComponent, PropType } from '@vue/composition-api';
import Rom from '@/components/emulator/Rom.vue';
import Ram from '@/components/emulator/Ram.vue';
import Registers from '@/components/emulator/Registers.vue';
import Signals from '@/components/emulator/Signals.vue';
import Io from '@/components/emulator/Io.vue';
import SettingsModal from '@/components/emulator/SettingsModal.vue';
import { positiveMod } from '@/utils/math';
import { TEmulatorSettings } from '@/types';
import { IoComponentId } from './emulator/io';

export default defineComponent({
  props: {
    assembledCode: {
      required: true,
      type: Array as PropType<TAsAssembledCode[]>,
    },
    isRunningEmulation: {
      required: false,
      default: false,
      type: Boolean,
    },
  },
  data: () => ({
    ram: new Array(CS_RAM_SIZE).fill(0) as number[],
    rom: new Array(CS_ROM_SIZE).fill(0) as number[],
    registers: Object.values(CsRegisterName).reduce(
      (acc, registerName) => ({
        ...acc,
        [registerName]: 0,
      }),
      {}
    ) as TCsRegisters,
    signals: Object.values(CsSignalName).reduce(
      (acc, signalName) => ({
        ...acc,
        [signalName]: 0,
      }),
      {}
    ) as TCsSignals,
    stopped: false,
    isClockRunning: false,
    clockTimerId: null as any,
    lastClockTick: 0,
    clockCycleCounter: 0,
    lastUiTick: 0,
    settings: {
      clockRunningFrequency: 1000,
      uiRefreshFrequency: 10,
      skipMicroinstructions: false,
      maxInstructionsBeforeHaltingBlockStep: 10000,
      romDisplayableRadix: 16,
      registerDisplayableRadix: 16,
      ramDisplayableRadix: 16,
      ramWordsPerRow: 16,
    } as TEmulatorSettings,
    isSettingsModalVisible: false,
    mappedIoComponents: {
      0: IoComponentId.HexDisplay,
      1: IoComponentId.HexDisplay,
      2: IoComponentId.Buttons,
      7: IoComponentId.RandomGenerator,
    },
  }),
  computed: {
    currentFetchedInstructionIdx(): number {
      return positiveMod(this.registers[CsRegisterName.PC] - 1, 255);
    },
    uiRefreshMininumTime(): number {
      return 1000 / this.settings.uiRefreshFrequency;
    },
  },
  methods: {
    setup(): void {
      let machineCode: number[] = [];
      for (let i = 0; i < CS_ROM_SIZE; i++) {
        machineCode[i] = this.assembledCode[i]
          ? this.assembledCode[i].machineCode
          : 0;
      }
      this.rom = machineCode;
      csLoadAndStart(machineCode);
      this.resetIoComponents();
      this.updateUiToMatchCsStatus(csGetStatus());
    },
    updateUiToMatchCsStatus(
      { ram, reg, signals, stopped }: TCsStatus,
      uiClockTick: number = performance.now()
    ): void {
      this.ram = ram;
      this.registers = reg;
      this.signals = signals;
      this.stopped = stopped;
      this.$emit(
        'current-assembly-line-changed',
        this.assembledCode?.[this.currentFetchedInstructionIdx]
      );
      if (stopped) {
        this.$buefy.snackbar.open({
          duration: 4000,
          message: `Execution has finished`,
          position: 'is-top',
          type: 'is-info',
        });
      }
      this.lastUiTick = uiClockTick;
    },
    microStep(): void {
      csMicroStep();
      this.updateUiToMatchCsStatus(csGetStatus());
    },
    fullStep(): void {
      csFullStep();
      this.updateUiToMatchCsStatus(csGetStatus());
    },
    blockStep(): void {
      const isExecutionCompleted = csBlockStep(
        this.settings.maxInstructionsBeforeHaltingBlockStep
      );
      if (!isExecutionCompleted) {
        this.$buefy.snackbar.open({
          duration: 4000,
          message: `Block execution halted because no stopping condition was found after ${this.settings.maxInstructionsBeforeHaltingBlockStep} instructions`,
          position: 'is-top',
          type: 'is-warning',
        });
      }
      this.updateUiToMatchCsStatus(csGetStatus());
    },
    hardReset(): void {
      csHardReset();
      this.resetIoComponents();
      this.updateUiToMatchCsStatus(csGetStatus());
    },
    resetIoComponents(): void {
      this.mappedIoComponents = { ...this.mappedIoComponents };
    },
    startClock(): void {
      this.isClockRunning = true;
      this.clockCycleCounter = 0;
      this.clockTimerId = window.requestAnimationFrame((nextTickTime: number) =>
        this.firstClockTick(nextTickTime)
      );
    },
    firstClockTick(currentTickTime: number): void {
      this.lastClockTick = currentTickTime;
      this.lastUiTick = currentTickTime;
      this.clockTimerId = window.requestAnimationFrame((nextTickTime: number) =>
        this.clockTick(
          nextTickTime,
          this.settings.skipMicroinstructions ? csFullStep : csMicroStep
        )
      );
    },
    clockTick(currentTickTime: number, stepFn: () => void): void {
      const timeElapsedSinceLastClockTick =
        currentTickTime - this.lastClockTick;
      const timeElapsedSinceLastUiTick = currentTickTime - this.lastUiTick;
      const cyclesNeededToExecuteInstruction = Math.ceil(
        1000 / timeElapsedSinceLastClockTick
      );
      this.lastClockTick = currentTickTime;
      this.clockCycleCounter += this.settings.clockRunningFrequency;

      while (this.clockCycleCounter >= cyclesNeededToExecuteInstruction) {
        this.clockCycleCounter -= cyclesNeededToExecuteInstruction;
        stepFn();
      }

      this.clockTimerId = window.requestAnimationFrame((timeElapsed: number) =>
        this.clockTick(timeElapsed, stepFn)
      );

      if (timeElapsedSinceLastUiTick >= this.uiRefreshMininumTime) {
        const csStatus = csGetStatus();
        this.updateUiToMatchCsStatus(csStatus, currentTickTime);
        if (csStatus.stopped) {
          this.stopClock();
        }
      }
    },
    stopClock(): void {
      cancelAnimationFrame(this.clockTimerId);
      this.isClockRunning = false;
    },
    onSettingsModalSavedChanges(newSettings: TEmulatorSettings): void {
      this.settings = {
        ...newSettings,
      };
      this.isSettingsModalVisible = false;
    },
  },
  watch: {
    isRunningEmulation(value) {
      if (value) {
        this.setup();
      } else {
        this.stopClock();
      }
    },
  },
  components: {
    Rom,
    Ram,
    Registers,
    Signals,
    Io,
    SettingsModal,
  },
});
</script>
