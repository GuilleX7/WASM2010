<template>
  <ResponsiveTable>
    <tr>
      <th>Register</th>
      <th>Content</th>
    </tr>
    <tr
      v-for="displayableRegister in displayableRegisters"
      :key="displayableRegister.name"
    >
      <td>{{ displayableRegister.name }}</td>
      <td>{{ displayableRegister.content }}</td>
    </tr>
  </ResponsiveTable>
</template>

<script lang="ts">
import ResponsiveTable from '@/components/emulator/modules/ResponsiveTable.vue';
import { CsPlatform, CsRegister, TCsRegisters } from '@/core/ts/types';
import { useGlobalStore } from '@/stores/global';
import { chunkString, formatNumber } from '@/utils/format';
import { PropType, defineComponent } from '@vue/composition-api';
import { mapState } from 'pinia';

type TRegisterLine = {
  name: string;
  content: string;
};

export default defineComponent({
  components: { ResponsiveTable },
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
    ...mapState(useGlobalStore, ['csPlatform']),
    displayableRegisters(): TRegisterLine[] {
      const prefix = this.displayableRadix === 16 ? '$' : '';
      const isBinary = this.displayableRadix === 2;
      return [
        {
          name: 'R0',
          content: formatNumber(
            this.registers[CsRegister.R0],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'R1',
          content: formatNumber(
            this.registers[CsRegister.R1],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'R2',
          content: formatNumber(
            this.registers[CsRegister.R2],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'R3',
          content: formatNumber(
            this.registers[CsRegister.R3],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'R4',
          content: formatNumber(
            this.registers[CsRegister.R4],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'R5',
          content: formatNumber(
            this.registers[CsRegister.R5],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: this.csPlatform === CsPlatform.Cs2010 ? 'R6' : 'R6 (Y)',
          content: formatNumber(
            this.registers[CsRegister.R6],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: this.csPlatform === CsPlatform.Cs2010 ? 'R7' : 'R7 (Z)',
          content: formatNumber(
            this.registers[CsRegister.R7],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'PC',
          content: formatNumber(
            this.registers[CsRegister.PC],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'SP',
          content: formatNumber(
            this.registers[CsRegister.SP],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'IR',
          content: chunkString(
            formatNumber(
              this.registers[CsRegister.IR],
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
          content: formatNumber(
            this.registers[CsRegister.AC],
            this.displayableRadix,
            8,
            prefix
          ),
        },
        {
          name: 'SR (VNZC)',
          content: formatNumber(this.registers[CsRegister.SR], 2, 4),
        },
        ...(this.csPlatform === CsPlatform.Cs2010
          ? [
              {
                name: 'MDR',
                content: formatNumber(
                  this.registers[CsRegister.MDR],
                  this.displayableRadix,
                  8,
                  prefix
                ),
              },
            ]
          : []),
        {
          name: 'MAR',
          content: formatNumber(
            this.registers[CsRegister.MAR],
            this.displayableRadix,
            8,
            prefix
          ),
        },
      ];
    },
  },
  methods: {
    formatNumber,
  },
});
</script>
