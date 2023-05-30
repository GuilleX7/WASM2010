<template>
  <div class="is-flex">
    <HexDisplay :value="firstDigit" />
    <HexDisplay :value="secondDigit" />
  </div>
</template>

<script lang="ts">
import { PropType, defineComponent } from '@vue/composition-api';
import HexDisplay from '../atoms/HexDisplay.vue';
import { UiHexDisplayController } from './HexDisplayController';

export default defineComponent({
  components: {
    HexDisplay,
  },
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
});
</script>
