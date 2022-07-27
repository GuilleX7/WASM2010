<template>
  <Table>
    <tr>
      <th>Register</th>
      <th>Content</th>
    </tr>
    <tr v-for="displayableRegister in displayableRegisters" :key="displayableRegister.name">
      <td>{{ displayableRegister.name }}</td>
      <td class="has-text-right">{{ displayableRegister.content }}</td>
    </tr>
  </Table>
</template>

<script lang="ts">
import { delimiteString, formatNumber } from '@/utils/format';
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
    displayableRadix: {
      required: false,
      default: 16,
      type: Number,
    },
  },
  computed: {
    displayableRegisters(): TRegisterLine[] {
      const prefix = this.displayableRadix === 16 ? '$' : '';
      const isBinary = this.displayableRadix === 2;
      return [
        {
          name: 'R0',
          content: formatNumber(this.registers[CsRegisterName.R0], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R1',
          content: formatNumber(this.registers[CsRegisterName.R1], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R2',
          content: formatNumber(this.registers[CsRegisterName.R2], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R3',
          content: formatNumber(this.registers[CsRegisterName.R3], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R4',
          content: formatNumber(this.registers[CsRegisterName.R4], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R5',
          content: formatNumber(this.registers[CsRegisterName.R5], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R6',
          content: formatNumber(this.registers[CsRegisterName.R6], this.displayableRadix, 8, prefix),
        },
        {
          name: 'R7',
          content: formatNumber(this.registers[CsRegisterName.R7], this.displayableRadix, 8, prefix),
        },
        {
          name: 'PC',
          content: formatNumber(this.registers[CsRegisterName.PC], this.displayableRadix, 8, prefix),
        },
        {
          name: 'SP',
          content: formatNumber(this.registers[CsRegisterName.SP], this.displayableRadix, 8, prefix),
        },
        {
          name: 'IR',
          content: delimiteString(
            formatNumber(
              this.registers[CsRegisterName.IR],
              this.displayableRadix,
              16,
              prefix
            ),
            isBinary ? 8 : 0,
            ' '
          ),
        },
        {
          name: 'AC',
          content: formatNumber(this.registers[CsRegisterName.AC], this.displayableRadix, 8, prefix),
        },
        {
          name: 'SR (VNZC)',
          content: formatNumber(this.registers[CsRegisterName.SR], 2, 4),
        },
        {
          name: 'MDR',
          content: formatNumber(this.registers[CsRegisterName.MDR], this.displayableRadix, 8, prefix),
        },
        {
          name: 'MAR',
          content: formatNumber(this.registers[CsRegisterName.MAR], this.displayableRadix, 8, prefix),
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
