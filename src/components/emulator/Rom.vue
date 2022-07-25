<template>
  <Table>
    <tr>
      <th>ROM</th>
      <th>Content</th>
    </tr>
    <tr
      v-for="(line, i) in data"
      :key="line.address"
      :class="{ 'is-selected': i === highlightLineIdx }"
      ref="lines"
    >
      <td>{{ line.address }}</td>
      <td>{{ line.content }}</td>
    </tr>
  </Table>
</template>

<script lang="ts">
import { formatNumber } from '@/utils/format';
import { defineComponent, PropType } from '@vue/composition-api';
import Table from './Table.vue';

type TRomLine = {
  address: string;
  content: string;
};

export default defineComponent({
  props: {
    content: {
      required: true,
      type: Array as PropType<number[]>,
    },
    highlightLineIdx: {
      required: false,
      default: null,
      type: Number,
    },
  },
  computed: {
    data(): TRomLine[] {
      return this.content.map((contentLine, i) => ({
        address: formatNumber(i, 16, 8, '0x'),
        content: formatNumber(contentLine, 16, 16, '$'),
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
        behavior: 'smooth',
        block: 'center',
      });
    },
  },
  components: { Table },
});
</script>
