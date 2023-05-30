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
import {
Cs2010Signal,
Cs3Signal,
CsPlatform,
CsSignal,
TCsSignals,
} from '@/core/ts/types';
import { useGlobalStore } from '@/stores/global';
import { PropType, defineComponent } from '@vue/composition-api';
import { mapState } from 'pinia';

type TSignal =
  | CsSignal
  | {
      name: CsSignal;
      label: string;
    };

type TDisplayableSignal = {
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
    ...mapState(useGlobalStore, ['csPlatform']),
    signalsByGroups(): TSignal[][] {
      return [
        [CsSignal.WIR, CsSignal.CPC, CsSignal.RPC, CsSignal.IPC, CsSignal.WPC],
        [
          CsSignal.WMDR,
          CsSignal.IOMDR,
          CsSignal.WMAR,
          CsSignal.WMEM,
          CsSignal.RMEM,
        ],
        [CsSignal.WAC, CsSignal.RAC, CsSignal.INM, CsSignal.SRW, CsSignal.WREG],
        [CsSignal.CSP, CsSignal.RSP, CsSignal.ISP, CsSignal.DSP],
        this.csPlatform === CsPlatform.Cs2010
          ? [
              { label: Cs2010Signal.ALUOP3, name: CsSignal.IMPL3 },
              { label: Cs2010Signal.ALUOP2, name: CsSignal.IMPL2 },
              { label: Cs2010Signal.ALUOP1, name: CsSignal.IMPL1 },
              { label: Cs2010Signal.ALUOP0, name: CsSignal.IMPL0 },
            ]
          : [
              { label: Cs3Signal.ALUS, name: CsSignal.IMPL3 },
              { label: Cs3Signal.ALUR, name: CsSignal.IMPL2 },
              { label: Cs3Signal.ALUTA, name: CsSignal.IMPL1 },
              { label: Cs3Signal.ALUTB, name: CsSignal.IMPL0 },
            ],
      ];
    },
    displayableSignalGroups(): TDisplayableSignal[][] {
      return this.signalsByGroups.map((signalGroup) =>
        signalGroup.map((signal) => ({
          name: typeof signal === 'string' ? signal : signal.label,
          active: Boolean(
            this.signals[typeof signal === 'string' ? signal : signal.name]
          ),
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
