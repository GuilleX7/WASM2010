<template>
  <Table>
    <tr>
      <th>Register</th>
      <th>Content</th>
    </tr>
    <tr v-for="line in data" :key="line.name">
      <td>{{ line.name }}</td>
      <td>{{ line.content }}</td>
    </tr>
  </Table>
</template>

<script lang="ts">
import { formatNumber } from '@/utils/format';
import { CsRegisterName, TCsRegisters } from '@/wasm/asm2010';
import { defineComponent, PropType } from '@vue/composition-api';
import Table from './Table.vue';

type TRegisterLine = {
  name: string;
  content: string;
};

export default defineComponent({
  props: {
    registers: {
      required: true,
      type: Object as PropType<TCsRegisters>,
    },
  },
  computed: {
    data(): TRegisterLine[] {
      return [
        {
          name: 'R0',
          content: formatNumber(this.registers[CsRegisterName.R0], 16, 8, '$'),
        },
        {
          name: 'R1',
          content: formatNumber(this.registers[CsRegisterName.R1], 16, 8, '$'),
        },
        {
          name: 'R2',
          content: formatNumber(this.registers[CsRegisterName.R2], 16, 8, '$'),
        },
        {
          name: 'R3',
          content: formatNumber(this.registers[CsRegisterName.R3], 16, 8, '$'),
        },
        {
          name: 'R4',
          content: formatNumber(this.registers[CsRegisterName.R4], 16, 8, '$'),
        },
        {
          name: 'R5',
          content: formatNumber(this.registers[CsRegisterName.R5], 16, 8, '$'),
        },
        {
          name: 'R6',
          content: formatNumber(this.registers[CsRegisterName.R6], 16, 8, '$'),
        },
        {
          name: 'R7',
          content: formatNumber(this.registers[CsRegisterName.R7], 16, 8, '$'),
        },
        {
          name: 'PC',
          content: formatNumber(this.registers[CsRegisterName.PC], 16, 8, '$'),
        },
        {
          name: 'SP',
          content: formatNumber(this.registers[CsRegisterName.SP], 16, 8, '$'),
        },
        {
          name: 'IR',
          content: formatNumber(this.registers[CsRegisterName.IR], 16, 16, '$'),
        },
        {
          name: 'AC',
          content: formatNumber(this.registers[CsRegisterName.AC], 16, 8, '$'),
        },
        {
          name: 'SR (VNZC)',
          content: formatNumber(this.registers[CsRegisterName.SR], 2, 4),
        },
        {
          name: 'MDR',
          content: formatNumber(this.registers[CsRegisterName.MDR], 16, 8, '$'),
        },
        {
          name: 'MAR',
          content: formatNumber(this.registers[CsRegisterName.MAR], 16, 8, '$'),
        },
      ];
    },
  },
  methods: {
    formatNumber,
  },
  components: { Table },
});
</script>
