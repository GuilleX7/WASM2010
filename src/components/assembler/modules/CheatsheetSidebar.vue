<template>
  <b-sidebar
    type="is-light"
    :right="true"
    :fullheight="true"
    :overlay="false"
    :open="open"
    :can-cancel="false"
  >
    <div class="px-5 py-5">
      <div class="is-flex is-justify-content-space-between">
        <p class="menu-label">Cheatsheet</p>
        <button type="button" class="delete" @click="$emit('close')" />
      </div>
      <p>Lorem ipsum</p>
    </div>
  </b-sidebar>
</template>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';

export default defineComponent({
  props: {
    open: {
      type: Boolean,
      default: false,
    },
  },
});
</script>
