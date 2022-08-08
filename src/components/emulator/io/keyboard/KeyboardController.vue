<template>
  <b-input
    type="text"
    :value="buffer"
    @keydown.native.prevent="onKeyDown"
  ></b-input>
</template>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';
import { UiKeyboardController } from './KeyboardController';

export default defineComponent({
  props: {
    controller: {
      required: true,
      type: Object as PropType<UiKeyboardController>,
    },
    uiClockTick: {
      required: true,
      type: Number,
    },
  },
  data: () => ({
    buffer: '',
  }),
  methods: {
    onKeyDown(event: KeyboardEvent): void {
      const char = String.fromCharCode(event.which & 0xff);
      this.buffer += char;
      this.controller.uiUpdateState(char);
    },
  },
  watch: {
    uiClockTick(): void {
      this.buffer = this.controller.uiGetState();
    },
  },
});
</script>
