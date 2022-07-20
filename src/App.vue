<template>
  <div class="is-flex has-background-white-bis">
    <div class="is-flex asm--column">
      <Editor></Editor>
    </div>
    <div class="is-flex asm--column"></div>
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
import { mapActions, mapState } from 'pinia';
import { TAsParseResult } from './wasm/asm2010';
import { useWasmStore } from './store/wasmStore';
import Vue from 'vue';
import Editor from './components/Editor.vue';

export default Vue.extend({
  data: () => ({
    sourceAssembly: '',
    outputMachineCode: '',
    log: '',
  }),
  computed: {
    ...mapState(useWasmStore, {
      isWasmLoaded: 'isLoaded',
    }),
    status(): string {
      return this.isWasmLoaded ? 'Ready' : 'Loading...';
    },
  },
  mounted() {
    this.loadWasm();
  },
  methods: {
    ...mapActions(useWasmStore, {
      loadWasm: 'load',
      assembleWasm: 'assemble',
    }),
    assemble() {
      const result: TAsParseResult = this.assembleWasm(this.sourceAssembly);
      this.outputMachineCode = result.machineCode
        .map((machineCodeLine) => machineCodeLine.toString(16))
        .join('\n');
      this.log = result.log;
    },
  },
  components: { Editor },
});
</script>
