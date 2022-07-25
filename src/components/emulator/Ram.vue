<template>
  <div class="is-fullwidth is-fullheight asm--emulator-ram-content">
    <table class="is-fullwidth is-fullheight">
      <tr v-for="i in 16" class="has-text-centered" :key="i">
        <th class="has-text-light has-text-weight-normal">
          {{ formatNumber((i - 1) * 16, 16, 8, '0x') }}
        </th>
        <td
          v-for="e in 16"
          :class="{ first: e === 1, last: e === 16 }"
          :key="`td${i * e}`"
        >
          {{ formatNumber(content[(i - 1) * 16 + (e - 1)], 16, 8) }}
        </td>
      </tr>
    </table>
  </div>
</template>

<style lang="scss" scoped>
.asm--emulator-ram-content {
  overflow: auto;

  table {
    border-collapse: separate;
    font-family: monospace;

    th {
      padding: 5px 10px;
      vertical-align: middle;
    }

    tr {
      td {
        padding: 5px;
        vertical-align: middle;

        &.first {
          padding-left: 10px;
        }

        &.last {
          padding-right: 10px;
        }
      }

      &:nth-child(odd) {
        & td {
          background-color: $white-ter;
        }
      }

      & th {
        background-color: $grey-dark !important;
      }
    }
  }
}
</style>

<script lang="ts">
import { formatNumber } from '@/utils/format';
import { defineComponent, PropType } from '@vue/composition-api';

export default defineComponent({
  props: {
    content: {
      required: true,
      type: Array as PropType<number[]>,
    },
  },
  methods: {
    formatNumber,
  },
});
</script>
