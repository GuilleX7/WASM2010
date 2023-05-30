<template>
  <div class="is-fullwidth is-viewheight">
    <b-modal
      v-model="isSettingsModalVisible"
      has-modal-card
      trap-focus
      size="is-medium"
    >
      <SettingsModal @close="isSettingsModalVisible = false" />
    </b-modal>
    <div class="is-flex is-flex-direction-column is-fullheight">
      <div class="asm--emulator-menu">
        <b-button
          v-if="!isClockRunning"
          type="is-light"
          icon-left="timer"
          @click="startClock"
        >
          Start clock
        </b-button>
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
        >
          Hard reset
        </b-button>
        <b-button
          type="is-light"
          icon-left="restart"
          :disabled="isClockRunning"
          @click="softReset"
        >
          Soft reset
        </b-button>
        <b-button
          type="is-light"
          icon-left="cog"
          :disabled="isClockRunning"
          @click="isSettingsModalVisible = true"
        >
          Settings
        </b-button>
      </div>
      <div class="asm--emulator-container">
        <div class="asm--emulator-rom has-background-white">
          <RomViewer
            :memory="rom"
            :current-instruction-idx="
              !isVisualMotionReduced ? currentFetchedInstructionIdx : undefined
            "
            :displayable-radix="romDisplayableRadix"
            :is-stopped="isStopped"
          />
        </div>
        <div class="asm--emulator-registers has-background-white">
          <RegistersViewer
            :registers="registers"
            :displayable-radix="registerDisplayableRadix"
          />
        </div>
        <div class="asm--emulator-ram has-background-white">
          <RamViewer
            :memory="ram"
            :current-accessed-word-idx="
              !isVisualMotionReduced ? registers.mar : undefined
            "
            :stack-pointer-word-idx="
              !isVisualMotionReduced ? registers.sp : undefined
            "
            :io-mapped-addresses="ioMappedAddresses"
            :displayable-radix="ramDisplayableRadix"
            :words-per-row="ramWordsPerRow"
          />
        </div>
        <div class="asm--emulator-signals has-background-white">
          <SignalsViewer
            v-if="!isVisualMotionReduced"
            :signals="signals"
            :microop="microop"
            :signals-per-row="4"
          />
          <ViewerCurtains v-else />
        </div>
        <div class="asm--emulator-io has-background-white">
          <IoPlayground
            :mapped-components="mappedIoComponents"
            :ui-clock-tick="lastUiTick"
          />
        </div>
      </div>
      <div class="asm--emulator-statusbar">
        <b-button
          type="is-light"
          icon-left="debug-step-into"
          :disabled="isClockRunning"
          @click="microStep"
        >
          Step microop
        </b-button>
        <b-button
          type="is-light"
          icon-left="debug-step-over"
          :disabled="isClockRunning"
          @click="fullStep"
        >
          Step op
        </b-button>
        <b-button
          type="is-light"
          icon-left="debug-step-out"
          :disabled="isClockRunning"
          @click="blockStep"
        >
          Step block
        </b-button>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import IoPlayground from '@/components/emulator/modules/IoPlayground.vue';
import RamViewer from '@/components/emulator/modules/RamViewer.vue';
import RegistersViewer from '@/components/emulator/modules/RegistersViewer.vue';
import RomViewer from '@/components/emulator/modules/RomViewer.vue';
import SettingsModal from '@/components/emulator/modules/SettingsModal.vue';
import SignalsViewer from '@/components/emulator/modules/SignalsViewer.vue';
import ViewerCurtains from '@/components/emulator/modules/ViewerCurtains.vue';
import { getAsm2010Instance } from '@/core/ts';
import {
CS_RAM_SIZE,
CS_ROM_SIZE,
CsRegister,
CsSignal,
TCsAssemblyInstruction,
TCsRegisters,
TCsSignals,
TCsStatus,
} from '@/core/ts/types';
import { useEmulatorStore } from '@/stores/emulator';
import { positiveMod } from '@/utils/math';
import { PropType, defineComponent } from '@vue/composition-api';
import { mapWritableState } from 'pinia';

export default defineComponent({
  components: {
    RomViewer,
    RamViewer,
    RegistersViewer,
    SignalsViewer,
    IoPlayground,
    SettingsModal,
    ViewerCurtains,
  },
  props: {
    assembledInstructions: {
      required: true,
      type: Array as PropType<TCsAssemblyInstruction[]>,
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
    registers: Object.values(CsRegister).reduce(
      (acc, registerName) => ({
        ...acc,
        [registerName]: 0,
      }),
      {}
    ) as TCsRegisters,
    signals: Object.values(CsSignal).reduce(
      (acc, signalName) => ({
        ...acc,
        [signalName]: 0,
      }),
      {}
    ) as TCsSignals,
    microop: 0,
    isStopped: false,
    isClockRunning: false,
    clockTimerId: 0,
    lastClockTick: 0,
    clockCycleCounter: 0,
    lastUiTick: 0,
    isSettingsModalVisible: false,
  }),
  computed: {
    ...mapWritableState(useEmulatorStore, [
      'clockRunningFrequency',
      'mappedIoComponents',
      'maxInstructionsBeforeHaltingBlockStep',
      'ramDisplayableRadix',
      'ramWordsPerRow',
      'reduceVisualMotion',
      'registerDisplayableRadix',
      'romDisplayableRadix',
      'skipMicroinstructions',
      'uiRefreshFrequency',
    ]),
    currentFetchedInstructionIdx(): number {
      return positiveMod(this.registers[CsRegister.PC] - 1, 256);
    },
    nextInstructionToFetchIdx(): number {
      return this.registers[CsRegister.PC];
    },
    ioMappedAddresses(): Set<number> {
      return new Set(
        Object.keys(this.mappedIoComponents).map((address) => parseInt(address))
      );
    },
    uiRefreshMinimumTime(): number {
      return 1000 / this.uiRefreshFrequency;
    },
    isVisualMotionReduced(): boolean {
      return this.isClockRunning && this.reduceVisualMotion;
    },
  },
  watch: {
    isRunningEmulation(): void {
      if (this.isRunningEmulation) {
        this.setup();
      } else {
        this.stopClock();
      }
    },
    isVisualMotionReduced() {
      this.$emit(
        'current-assembly-line-changed',
        this.isVisualMotionReduced
          ? undefined
          : this.assembledInstructions?.[this.currentFetchedInstructionIdx]
      );
    },
  },
  methods: {
    setup(): void {
      const asm2010 = getAsm2010Instance();
      let machineCode: number[] = [];
      for (let i = 0; i < CS_ROM_SIZE; i++) {
        machineCode[i] = this.assembledInstructions[i]
          ? this.assembledInstructions[i].machineInstruction
          : 0;
      }
      this.rom = machineCode;
      asm2010.csLoadProgram(machineCode);
      this.resetIoComponents();
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    updateUiToMatchCsStatus(
      { ram, registers, signals, microop, isStopped }: TCsStatus,
      uiClockTick: number = performance.now()
    ): void {
      this.ram = ram;
      this.registers = registers;
      this.signals = signals;
      this.microop = microop;
      this.isStopped = isStopped;
      if (!this.isVisualMotionReduced) {
        this.$emit(
          'current-assembly-line-changed',
          this.assembledInstructions?.[this.currentFetchedInstructionIdx]
        );
      }
      if (isStopped) {
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
      const asm2010 = getAsm2010Instance();
      asm2010.csMicroStep();
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    fullStep(): void {
      const asm2010 = getAsm2010Instance();
      asm2010.csFullStep();
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    blockStep(): void {
      const asm2010 = getAsm2010Instance();
      const isExecutionCompleted = asm2010.csBlockStep(
        this.maxInstructionsBeforeHaltingBlockStep
      );
      if (!isExecutionCompleted) {
        this.$buefy.snackbar.open({
          duration: 4000,
          message: `Block execution halted because no stopping condition was found after ${this.maxInstructionsBeforeHaltingBlockStep} instructions`,
          position: 'is-top',
          type: 'is-warning',
        });
      }
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    softReset(): void {
      const asm2010 = getAsm2010Instance();
      asm2010.csSoftReset();
      this.resetIoComponents();
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    hardReset(): void {
      const asm2010 = getAsm2010Instance();
      asm2010.csHardReset();
      this.resetIoComponents();
      this.updateUiToMatchCsStatus(asm2010.csGetStatus());
    },
    resetIoComponents(): void {
      this.mappedIoComponents = {
        ...this.mappedIoComponents,
      };
    },
    startClock(): void {
      this.isClockRunning = true;
      this.clockCycleCounter = 0;
      this.clockTimerId = window.requestAnimationFrame((nextTickTime: number) =>
        this.firstClockTick(nextTickTime)
      );
    },
    firstClockTick(currentTickTime: number): void {
      const asm2010 = getAsm2010Instance();
      this.lastClockTick = currentTickTime;
      this.lastUiTick = currentTickTime;
      this.clockTimerId = window.requestAnimationFrame((nextTickTime: number) =>
        this.clockTick(
          nextTickTime,
          this.skipMicroinstructions ? asm2010.csFullStep : asm2010.csMicroStep
        )
      );
    },
    clockTick(currentTickTime: number, stepFn: () => void): void {
      const asm2010 = getAsm2010Instance();
      const timeElapsedSinceLastClockTick =
        currentTickTime - this.lastClockTick;
      const timeElapsedSinceLastUiTick = currentTickTime - this.lastUiTick;
      const cyclesNeededToExecuteInstruction = Math.ceil(
        1000 / timeElapsedSinceLastClockTick
      );
      this.lastClockTick = currentTickTime;
      this.clockCycleCounter += this.clockRunningFrequency;

      while (this.clockCycleCounter >= cyclesNeededToExecuteInstruction) {
        this.clockCycleCounter -= cyclesNeededToExecuteInstruction;
        stepFn();
      }

      this.clockTimerId = window.requestAnimationFrame((timeElapsed: number) =>
        this.clockTick(timeElapsed, stepFn)
      );

      if (timeElapsedSinceLastUiTick >= this.uiRefreshMinimumTime) {
        const csStatus = asm2010.csGetStatus();
        this.updateUiToMatchCsStatus(csStatus, currentTickTime);
        if (csStatus.isStopped) {
          this.stopClock();
        }
      }
    },
    stopClock(): void {
      cancelAnimationFrame(this.clockTimerId);
      this.isClockRunning = false;
    },
  },
});
</script>

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
