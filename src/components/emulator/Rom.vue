<template>
  <Table>
    <tr>
      <th>ROM</th>
      <th>Content</th>
    </tr>
    <tr
      v-for="(displayableMemoryWord, i) in displayableMemoryWords"
      :key="displayableMemoryWord.address"
      :class="{ [!isStopped ? 'has-background-info-light' : 'has-background-primary-light']: i === highlightLineIdx }"
      ref="lines"
    >
      <td>{{ displayableMemoryWord.address }}</td>
      <td class="has-text-right">{{ displayableMemoryWord.content }}</td>
    </tr>
  </Table>
</template>

<script lang="ts">
import { delimiteString, formatNumber } from '@/utils/format';
import { defineComponent, PropType } from '@vue/composition-api';
import Table from './Table.vue';

type TRomLine = {
  address: string;
  content: string;
};

export default defineComponent({
  props: {
    memory: {
      required: true,
      type: Array as PropType<number[]>,
    },
    highlightLineIdx: {
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
      const prefix = this.displayableRadix === 16 ? '$' : '';
      const isBinary = this.displayableRadix === 2;
      return this.memory.map((memoryWord, i) => ({
        address: formatNumber(i, 16, 8, '0x'),
        content: delimiteString(
          formatNumber(memoryWord, this.displayableRadix, 16, prefix),
          isBinary ? 8 : 0,
          ' '
        ),
      }));
    },
  },
  methods: {
    formatNumber,
  },
  watch: {
    highlightLineIdx(): void {
      (this.$refs.lines as HTMLTableRowElement[])[
        this.highlightLineIdx
      ]?.scrollIntoView({
        behavior: 'auto',
        block: 'center',
      });
    },
  },
  components: { Table },
});
</script>
