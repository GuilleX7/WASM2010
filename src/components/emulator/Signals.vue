<template>
  <Table>
    <tr>
      <th>Signal</th>
      <th></th>
    </tr>
    <tr v-for="signal in data" :key="signal.name">
      <td>{{ signal.name }}</td>
      <td
        :class="{
          'has-background-info-light': !signal.active,
          'has-background-info': signal.active,
        }"
      >{{ signal.active ? 'ON' : 'OFF' }}</td>
    </tr>
  </Table>
</template>

<script lang="ts">
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
  },
  computed: {
    data(): TSignalLine[] {
      return Object.entries(CsSignalName).map(([signalName, signalValue]) => ({
        name: signalName,
        active: Boolean(this.signals[signalValue]),
      }));
    },
  },
  components: { Table },
});
</script>
