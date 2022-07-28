<template>
  <div class="hexdisplay">
    <span class="hexdisplay-backlight">8</span>
    <span class="hexdisplay-content">{{ displayableValue }}</span>
  </div>
</template>

<style lang="scss">
.hexdisplay {
  font-family: DSEG7Classic;
  font-size: 50px;
  font-weight: bold;
  display: grid;

  .hexdisplay-backlight {
    color: $white-ter;
    grid-row: 1 / 2;
    grid-column: 1 / 2;
  }

  .hexdisplay-content {
    color: $danger-dark;
    grid-row: 1 / 2;
    grid-column: 1 / 2;
  }
}
</style>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';

export default defineComponent({
  props: {
    value: {
      required: false,
      default: 0,
      type: [Number, String],
    },
  },
  computed: {
    displayableValue(): string {
      return this.value.toString(16)[0];
    },
  },
});
</script>
