<template>
  <div
    :class="{
      'asm--cheatsheet-container': true,
      open: open,
    }"
  >
    <b-icon
      v-show="!open"
      class="asm--cheatsheet-hint"
      icon="chevron-left"
      @click.native="$emit('open')"
    ></b-icon>
    <div class="is-fullwidth px-5 py-5">
      <div class="is-flex is-align-items-center">
        <b-icon
          icon="chevron-right"
          class="is-clickable mr-1"
          @click.native="$emit('close')"
        />
        <p class="menu-label m-0">Cheatsheet</p>
      </div>
      <b-table :data="data" :columns="columns"></b-table>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';

type TInstructionDescription = {
  nmemonic: string;
  syntax: string;
  effect: string;
};

export default defineComponent({
  props: {
    open: {
      type: Boolean,
      default: false,
    },
  },
  data: () => ({
    data: [
      {
        nmemonic: 'ST',
        syntax: 'ST (Rbase), Rsource',
        effect: 'MEM[Rbase] <- Rsource',
      },
      {
        nmemonic: 'LD',
        syntax: 'LD Rdest, (Rbase)',
        effect: 'Rdest <- MEM[Rbase]',
      },
    ] as TInstructionDescription[],
    columns: [
      {
        field: 'nmemonic',
        label: '',
      },
      {
        field: 'syntax',
        label: 'Syntax',
      },
      {
        field: 'effect',
        label: 'Effect',
      },
    ],
  }),
});
</script>

<style lang="scss" scoped>
.asm--cheatsheet-hint {
  position: fixed;
  top: calc(50% - 36px / 2);
  right: 0%;
  width: 32px;
  cursor: pointer;
}

.asm--cheatsheet-container {
  z-index: 0;
  width: 0px;

  &.open {
    display: flex;
    height: 100%;
    width: 500px;
    box-shadow: 0px 0px 13px 3px rgba(10, 10, 10, 0.1);
  }
}
</style>
