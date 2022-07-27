<template>
  <Table>
    <tr>
      <th>Signal</th>
      <th v-for="_ in signalsPerRow - 1"></th>
    </tr>
    <tr v-for="(signalLine, i) in data" :key="`r${i}`">
      <td
        v-for="(signal, e) in signalLine"
        :key="`c${i * signalsPerRow + e}`"
        :class="{
          'has-background-white': !signal.active,
          'has-background-info-light': signal.active,
        }"
      >
        {{ signal.name }}
      </td>
    </tr>
  </Table>
</template>

<script lang="ts">
import { chunks } from '@/utils/array';
import { CsSignalName, TCsSignals } from '@/wasm/asm2010';
import { defineComponent, PropType } from '@vue/composition-api';
import Table from './Table.vue';

type TSignalLine = {
  name: string;
  active: boolean;
};

export default defineComponent({
  props: {
    signals: {
      required: true,
      type: Object as PropType<TCsSignals>,
    },
    signalsPerRow: {
      required: false,
      default: 3,
      type: Number,
    },
  },
  computed: {
    data(): TSignalLine[][] {
      return chunks(
        Object.entries(CsSignalName).map(([signalName, signalValue]) => ({
          name: signalName,
          active: Boolean(this.signals[signalValue]),
        })),
        this.signalsPerRow
      );
    },
  },
  components: { Table },
});
</script>
