<template>
  <div
    class="is-fullwidth is-fullheight asm--emulator-signals-content is-flex is-flex-direction-column"
  >
    <div class="asm--emulator-signals-header">
      Signals (Cycle {{ microop + 1 }})
    </div>
    <div
      v-for="(
        displayableSignalGroup, displayableSignalGroupIdx
      ) in displayableSignalGroups"
      :key="displayableSignalGroupIdx"
      class="is-fullwidth is-fullheight asm--emulator-signals-row"
    >
      <div
        v-for="displayableSignal in displayableSignalGroup"
        :key="displayableSignal.name"
        :class="{ 'has-background-info-light': displayableSignal.active }"
        class="is-flex p-2 has-text-centered is-fullwidth is-fullheight is-justify-content-center is-align-items-center asm--emulator-signals-cell"
      >
        <span>{{ displayableSignal.name }}</span>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import { CsSignalName, TCsSignals } from '@/wasm/asm2010';
import { defineComponent, PropType } from '@vue/composition-api';

type TSignal = {
  name: string;
  active: boolean;
};

export default defineComponent({
  props: {
    signals: {
      required: true,
      type: Object as PropType<TCsSignals>,
    },
    microop: {
      required: true,
      type: Number,
    },
  },
  computed: {
    signalGroupsContent(): CsSignalName[][] {
      return [
        [
          CsSignalName.WIR,
          CsSignalName.CPC,
          CsSignalName.RPC,
          CsSignalName.IPC,
          CsSignalName.WPC,
        ],
        [
          CsSignalName.CSP,
          CsSignalName.RSP,
          CsSignalName.ISP,
          CsSignalName.DSP,
        ],
        [
          CsSignalName.WMDR,
          CsSignalName.IOMDR,
          CsSignalName.WMAR,
          CsSignalName.WMEM,
          CsSignalName.RMEM,
        ],
        [
          CsSignalName.WAC,
          CsSignalName.RAC,
          CsSignalName.INM,
          CsSignalName.SRW,
          CsSignalName.WREG,
        ],
        [
          CsSignalName.ALUOP3,
          CsSignalName.ALUOP2,
          CsSignalName.ALUOP1,
          CsSignalName.ALUOP0,
        ],
      ];
    },
    displayableSignalGroups(): TSignal[][] {
      return this.signalGroupsContent.map((signalGroupContent) =>
        signalGroupContent.map((signalName) => ({
          name: signalName,
          active: this.signals[signalName] as unknown as boolean,
        }))
      );
    },
  },
});
</script>

<style lang="scss">
.asm--emulator-signals-content {
  display: grid;
  grid-auto-rows: max-content;
  overflow: auto;

  .asm--emulator-signals-header {
    background-color: $white;
    color: $dark;
    position: sticky;
    top: 0;
    z-index: 2;
    font-weight: 600;
    padding: 0.25rem 0.5rem;
    border: 2px solid $grey-lighter;
    border-collapse: separate;
    border-width: 0 0 2px;
  }

  .asm--emulator-signals-row {
    display: flex;
    font-family: monospace;
  }

  .asm--emulator-signals-cell {
    flex-grow: 1;
    width: 100%;
  }
}
</style>
