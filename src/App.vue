<template>
  <div class="is-flex has-background-white-bis">
    <div class="is-flex asm--column">
      <Editor @export="openLoadAssembledCodeDialog"></Editor>
    </div>
    <div class="is-flex asm--column">
      <Simulator :machineCode="loadedMachineCode"></Simulator>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.asm--column {
  min-height: 100vh;

  &:first-child {
    flex-basis: 50%;
    max-width: 50%;
  }

  &:last-child {
    flex-basis: 50%;
    max-width: 50%;
  }
}
</style>

<script lang="ts">
import Vue from 'vue';
import Editor from './components/Editor.vue';
import Simulator from './components/Simulator.vue';
import { loadAsm2010, TAsAssembledCode } from './wasm/asm2010';

export default Vue.extend({
  data: () => ({
    loadedMachineCode: [] as number[],
  }),
  async mounted() {
    const loadingComponent = this.$buefy.loading.open({});
    await loadAsm2010();  
    loadingComponent.close();
  },
  methods: {
    openLoadAssembledCodeDialog(assembledCode: TAsAssembledCode[]): void {
      this.$buefy.dialog.confirm({
        title: 'Load machine code',
        message:
          'Do you want to load new machine code? This will stop the current simulation.',
        trapFocus: true,
        onConfirm: () => {
          this.loadedMachineCode = assembledCode.map(
            ({ machineCode }) => machineCode
          );
        },
      });
    },
  },
  components: { Editor, Simulator },
});
</script>
