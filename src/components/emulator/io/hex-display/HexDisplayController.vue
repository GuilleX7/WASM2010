<template>
  <div class="is-flex">
    <HexDisplay :value="firstDigit"></HexDisplay>
    <HexDisplay :value="secondDigit"></HexDisplay>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';
import HexDisplay from '../atoms/HexDisplay.vue';
import { UiHexDisplayController } from './HexDisplayController';

export default defineComponent({
  props: {
    controller: {
      required: true,
      type: Object as PropType<UiHexDisplayController>,
    },
    uiClockTick: {
      required: true,
      type: Number,
    },
  },
  data: () => ({
    firstDigit: 0,
    secondDigit: 0,
  }),
  watch: {
    uiClockTick(): void {
        const { firstDigit, secondDigit } = this.controller.uiGetState();
        this.firstDigit = firstDigit;
        this.secondDigit = secondDigit;
    },
  },
  components: {
    HexDisplay,
  },
});
</script>
