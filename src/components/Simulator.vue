<template>
  <div class="is-fullwidth is-viewheight asm--assembler-column">
    <div class="is-flex is-flex-direction-column is-fullheight">
      <p>sdfdfsdsf</p>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';

export default defineComponent({
  props: {
    machineCode: {
      required: true,
      type: Array as PropType<number[]>,
    },
  },
});
</script>
