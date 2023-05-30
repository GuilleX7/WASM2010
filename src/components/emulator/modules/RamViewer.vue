<template>
  <div class="is-fullwidth is-fullheight asm--emulator-ram-content">
    <table class="is-fullwidth is-fullheight">
      <tr
        v-for="displayableMemoryLine in displayableMemoryLines"
        :key="`r${displayableMemoryLine.baseAddress}`"
        class="has-text-centered"
      >
        <th class="has-text-light has-text-weight-normal">
          {{ displayableMemoryLine.formattedBaseAddress }}
        </th>
        <td
          v-for="(displayableByte, byteIdxInRow) in displayableMemoryLine.bytes"
          :key="`c${displayableByte.address}`"
          ref="words"
          :class="{
            first: byteIdxInRow === 2,
            last: byteIdxInRow === 16,
            'has-background-info-light':
              displayableByte.address === currentAccessedWordIdx,
            'has-background-primary-light':
              displayableByte.address === stackPointerWordIdx,
            'has-text-danger': ioMappedAddresses.has(displayableByte.address),
          }"
        >
          {{ displayableByte.formattedContent }}
        </td>
      </tr>
    </table>
  </div>
</template>

<script lang="ts">
import { CS_RAM_SIZE } from '@/core/ts/types';
import { formatNumber } from '@/utils/format';
import { PropType, defineComponent } from '@vue/composition-api';

type TRamWord = {
  address: number;
  formattedContent: string;
};

type TRamLine = {
  baseAddress: number;
  formattedBaseAddress: string;
  bytes: TRamWord[];
};

export default defineComponent({
  props: {
    memory: {
      required: true,
      type: Array as PropType<number[]>,
    },
    currentAccessedWordIdx: {
      required: false,
      default: null,
      type: Number,
    },
    stackPointerWordIdx: {
      required: false,
      default: null,
      type: Number,
    },
    ioMappedAddresses: {
      required: false,
      default: () => new Set(),
      type: Set as PropType<Set<number>>,
    },
    displayableRadix: {
      required: false,
      default: 16,
      type: Number,
    },
    wordsPerRow: {
      required: false,
      default: 256,
      type: Number,
    },
  },
  computed: {
    amountOfRows(): number {
      return Math.ceil(CS_RAM_SIZE / this.wordsPerRow);
    },
    displayableMemoryLines(): TRamLine[] {
      const displayableRamLines: TRamLine[] = [];
      for (let i = 0; i < this.amountOfRows; i++) {
        const lineBaseAddress = i * this.wordsPerRow;
        displayableRamLines[i] = {
          baseAddress: lineBaseAddress,
          formattedBaseAddress: formatNumber(lineBaseAddress, 16, 8, '0x'),
          bytes: [],
        };
        for (let j = 0; j < this.wordsPerRow; j++) {
          const byteAddress = lineBaseAddress + j;
          displayableRamLines[i].bytes[j] = {
            address: byteAddress,
            formattedContent: formatNumber(
              this.memory?.[byteAddress] ?? 0,
              this.displayableRadix,
              8
            ),
          };
        }
      }
      return displayableRamLines;
    },
  },
  watch: {
    currentAccessedWordIdx(): void {
      (this.$refs.words as HTMLTableRowElement[])[
        this.currentAccessedWordIdx
      ]?.scrollIntoView({
        behavior: 'auto',
        block: 'center',
      });
    },
  },
  methods: {
    formatNumber,
  },
});
</script>

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
        td {
          background-color: $white-bis;
        }
      }

      th {
        background-color: $grey-dark !important;
      }
    }
  }
}
</style>
