<template>
  <b-input
    type="text"
    :value="buffer"
    @keydown.native.prevent="onKeyDown"
  />
</template>

<script lang="ts">
import { PropType, defineComponent } from '@vue/composition-api';
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
  watch: {
    uiClockTick(): void {
      this.buffer = this.controller.uiGetState();
    },
  },
  methods: {
    onKeyDown(event: KeyboardEvent): void {
      const char = String.fromCharCode(event.which & 0xff);
      this.buffer += char;
      this.controller.uiUpdateState(char);
    },
  },
});
</script>
