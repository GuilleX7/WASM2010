<template>
  <ResponsiveTable>
    <tr>
      <th>ROM</th>
      <th>Disassembly</th>
      <th>Content</th>
    </tr>
    <tr
      v-for="(displayableMemoryWord, i) in displayableMemoryWords"
      :key="displayableMemoryWord.address"
      ref="lines"
      :class="{
        [currentInstructionBackgroundColor]: i === currentInstructionIdx,
      }"
    >
      <td>{{ displayableMemoryWord.address }}</td>
      <td>{{ displayableMemoryWord.disassembly }}</td>
      <td>{{ displayableMemoryWord.content }}</td>
    </tr>
  </ResponsiveTable>
</template>

<script lang="ts">
import ResponsiveTable from '@/components/emulator/modules/ResponsiveTable.vue';
import { getAsm2010Instance } from '@/core/ts';
import { CS_ROM_SIZE } from '@/core/ts/types';
import { chunkString, formatNumber } from '@/utils/format';
import { PropType, defineComponent } from '@vue/composition-api';

type TRomLine = {
  address: string;
  disassembly: string;
  content: string;
};

export default defineComponent({
  components: { ResponsiveTable },
  props: {
    memory: {
      required: true,
      type: Array as PropType<number[]>,
    },
    assembledInstructionsLength: {
      required: false,
      default: CS_ROM_SIZE,
      type: Number,
    },
    currentInstructionIdx: {
      required: false,
      default: null,
      type: Number,
    },
    displayableRadix: {
      required: false,
      default: 16,
      type: Number,
    },
    isStopped: {
      required: false,
      default: false,
      type: Boolean,
    },
  },
  computed: {
    displayableMemoryWords(): TRomLine[] {
      const asm2010 = getAsm2010Instance();
      const prefix = this.displayableRadix === 16 ? '$' : '';
      const isBinary = this.displayableRadix === 2;
      const disassembly = asm2010.asDisassemble(this.memory);
      return this.memory.map((memoryWord, i) => ({
        address: formatNumber(i, 16, 8, '0x'),
        disassembly:
          i < this.assembledInstructionsLength ? disassembly[i] : '-',
        content: chunkString(
          formatNumber(memoryWord, this.displayableRadix, 16, prefix),
          isBinary ? 8 : 0,
          ' '
        ),
      }));
    },
    isOutOfAssembly(): boolean {
      return (
        this.currentInstructionIdx !== undefined &&
        this.currentInstructionIdx >= this.assembledInstructionsLength
      );
    },
    currentInstructionBackgroundColor(): string {
      switch (true) {
        case this.isStopped:
          return 'has-background-primary-light';
        case this.isOutOfAssembly:
          return 'has-background-danger-light';
        default:
          return 'has-background-info-light';
      }
    },
  },
  watch: {
    currentInstructionIdx(): void {
      (this.$refs.lines as HTMLTableRowElement[])[
        this.currentInstructionIdx
      ]?.scrollIntoView({
        behavior: 'auto',
        block: 'center',
      });
    },
    isOutOfAssembly(): void {
      if (this.isOutOfAssembly) {
        this.$buefy.snackbar.open({
          duration: 4000,
          message: `Beware! Program counter is not pointing at the assembled instructions`,
          position: 'is-top',
          type: 'is-warning',
        });
      }
    },
  },
  methods: {
    formatNumber,
  },
});
</script>
