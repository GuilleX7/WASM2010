<template>
  <div class="is-fullwidth is-viewheight">
    <div class="is-flex is-flex-direction-column is-fullheight">
      <div class="asm--emulator-menu"></div>
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
          <Signals :signals="signals"></Signals>
        </div>
      </div>
      <div class="asm--emulator-statusbar">
        <b-button type="is-light" icon-left="debug-step-into" @click="microStep"
          >Step microop</b-button
        >
        <b-button type="is-light" icon-left="debug-step-over" @click="fullStep"
          >Step op</b-button
        >
        <b-button type="is-light" icon-left="debug-step-out" @click="blockStep"
          >Step block</b-button
        >
        <b-button type="is-light" icon-left="reload" @click="hardReset"
          >Reset</b-button
        >
      </div>
    </div>
  </div>
</template>

<style lang="scss">
.asm--emulator-menu {
  display: flex;
  padding-left: 0.75rem;
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
    grid-row: 1 / 3;
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
import { formatNumber } from '@/utils/format';
import { positiveMod } from '@/utils/math';

export default defineComponent({
  props: {
    assembledCode: {
      required: true,
      type: Array as PropType<TAsAssembledCode[]>,
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
    setup() {
      csLoadAndStart(
        this.assembledCode.map(
          (assembledCodeLine) => assembledCodeLine.machineCode
        )
      );
      this.updateUi();
    },
    updateUi() {
      const { ram, reg, signals } = csGetStatus();
      this.ram = ram;
      this.registers = reg;
      this.signals = signals;
      this.$emit(
        'current-assembly-line-changed',
        this.assembledCode?.[this.currentFetchedInstructionIdx]
      );
    },
    microStep() {
      csMicroStep();
      this.updateUi();
    },
    fullStep() {
      csFullStep();
      this.updateUi();
    },
    blockStep() {
      csBlockStep();
      this.updateUi();
    },
    hardReset() {
      csHardReset();
      this.updateUi();
    },
  },
  watch: {
    assembledCode() {
      this.setup();
    },
  },
  components: {
    Rom,
    Ram,
    Registers,
    Signals,
  },
});
</script>
