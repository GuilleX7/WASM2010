<template>
  <div class="is-fullwidth is-viewheight">
    <b-modal
      v-model="showSettingsModal"
      has-modal-card
      trap-focus
      :destroy-on-hide="false"
    >
      <SettingsModal @close="showSettingsModal = false"></SettingsModal>
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
          icon-left="cog"
          :disabled="isClockRunning"
          @click="showSettingsModal = true"
          >Settings</b-button
        >
      </div>
      <div class="asm--emulator-container">
        <div class="asm--emulator-rom has-background-white">
          <Rom
            :content="rom"
            :highlightLineIdx="currentFetchedInstructionIdx"
          ></Rom>
        </div>
        <div class="asm--emulator-registers has-background-white">
          <Registers :registers="registers"></Registers>
        </div>
        <div class="asm--emulator-ram has-background-white">
          <Ram :content="ram"></Ram>
        </div>
        <div class="asm--emulator-signals has-background-white">
          <Signals :signals="signals" :signalsPerLine="4"></Signals>
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
        <b-button
          type="is-light"
          icon-left="reload"
          :disabled="isClockRunning"
          @click="hardReset"
          >Reset</b-button
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
  CS_ROM_SIZE,
  TAsAssembledCode,
  TCsRegisters,
  TCsSignals,
} from '@/wasm/asm2010';
import { defineComponent, PropType } from '@vue/composition-api';
import Rom from '@/components/emulator/Rom.vue';
import Ram from '@/components/emulator/Ram.vue';
import Registers from '@/components/emulator/Registers.vue';
import Signals from '@/components/emulator/Signals.vue';
import SettingsModal from '@/components/emulator/SettingsModal.vue';
import { formatNumber } from '@/utils/format';
import { positiveMod } from '@/utils/math';

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
    ram: new Array(256).fill(0) as number[],
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
    isClockRunning: false,
    clockTimerId: null as any,
    lastClockTick: 0,
    settings: {
      clockRunningSpeed: 1000, // in Hz
      maxInstructionsBeforeHaltingBlockStep: 10000,
    },
    showSettingsModal: false,
  }),
  computed: {
    rom(): string[] {
      return new Array(CS_ROM_SIZE)
        .fill(0)
        .map((_, i) =>
          formatNumber(
            this.assembledCode[i] ? this.assembledCode[i].machineCode : 0,
            16,
            16
          )
        );
    },
    currentFetchedInstructionIdx(): number {
      return positiveMod(this.registers[CsRegisterName.PC] - 1, 255);
    },
  },
  methods: {
    setup(): void {
      csLoadAndStart(
        this.assembledCode.map(
          (assembledCodeLine) => assembledCodeLine.machineCode
        )
      );
      this.updateUi();
    },
    updateUi(): void {
      const { ram, reg, signals } = csGetStatus();
      this.ram = ram;
      this.registers = reg;
      this.signals = signals;
      this.$emit(
        'current-assembly-line-changed',
        this.assembledCode?.[this.currentFetchedInstructionIdx]
      );
    },
    microStep(): void {
      csMicroStep();
      this.updateUi();
    },
    fullStep(): void {
      csFullStep();
      this.updateUi();
    },
    blockStep(): void {
      if (!csBlockStep(this.settings.maxInstructionsBeforeHaltingBlockStep)) {
        this.$buefy.snackbar.open({
          duration: 3000,
          message: `Block execution halted because no stopping condition was found after ${this.settings.maxInstructionsBeforeHaltingBlockStep} instructions`,
          position: 'is-top',
          type: 'is-warning',
        });
      }
      this.updateUi();
    },
    hardReset(): void {
      csHardReset();
      this.updateUi();
    },
    startClock(): void {
      this.isClockRunning = true;
      this.lastClockTick = performance.now();
      this.clockTimerId = window.requestAnimationFrame((nextTickTime: number) =>
        this.clockTick(nextTickTime)
      );
    },
    clockTick(nextTickTime: number): void {
      const timeElapsed = nextTickTime - this.lastClockTick;
      this.lastClockTick = nextTickTime;
      if (timeElapsed < 0) {
        this.clockTimerId = window.requestAnimationFrame(
          (timeElapsed: number) => this.clockTick(timeElapsed)
        );
        return;
      }

      for (let i = 0; i < this.settings.clockRunningSpeed; i++) {
        csFullStep();
      }
      this.updateUi();

      this.clockTimerId = window.requestAnimationFrame((timeElapsed: number) =>
        this.clockTick(timeElapsed)
      );
    },
    stopClock(): void {
      cancelAnimationFrame(this.clockTimerId);
      this.isClockRunning = false;
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
    SettingsModal,
  },
});
</script>
