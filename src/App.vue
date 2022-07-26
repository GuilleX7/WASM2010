<template>
  <div class="is-flex has-background-white-ter">
    <Multipane class="is-fullwidth">
      <div
        :class="{ 'is-fullwidth': !isRunningEmulation }"
        style="min-width: 200px"
        ref="assemblerContainer"
      >
        <Assembler
          :isRunningEmulation="isRunningEmulation"
          :highlightLineIdx="currentRunningAssemblyLineIdx"
          @start-emulation="startEmulation"
          @stop-emulation="stopEmulation"
        ></Assembler>
      </div>
      <MultipaneResizer v-show="isRunningEmulation"></MultipaneResizer>
      <div
        v-show="isRunningEmulation"
        class="is-flex-grow-1"
        style="min-width: 50%"
      >
        <Emulator
          :isRunningEmulation="isRunningEmulation"
          :assembledCode="loadedAssembledCode"
          @current-assembly-line-changed="onCurrentAssemblyLineChanged"
        ></Emulator>
      </div>
    </Multipane>
  </div>
</template>

<style lang="scss">
.layout-v .multipane-resizer {
  margin: 0 !important;
  left: 0 !important;
  width: 0.75rem;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;

  &:before {
    content: '';
    width: 3px;
    height: 40px;
    border-left: 1px solid #ccc;
    border-right: 1px solid #ccc;
  }

  &:hover {
    &:before {
      border-color: #999;
    }
  }
}
</style>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';
import { Multipane, MultipaneResizer } from 'vue-multipane';
import Assembler from './components/Assembler.vue';
import Emulator from './components/Emulator.vue';
import { loadAsm2010, TAsAssembledCode } from './wasm/asm2010';

export default defineComponent({
  data: () => ({
    isRunningEmulation: false,
    loadedAssembledCode: [] as TAsAssembledCode[],
    currentRunningAssemblyLineIdx: undefined as number | undefined,
  }),
  computed: {},
  async mounted() {
    const loadingComponent = this.$buefy.loading.open({});
    await loadAsm2010();
    loadingComponent.close();
  },
  methods: {
    startEmulation(assembledCode: TAsAssembledCode[]): void {
      this.loadedAssembledCode = assembledCode;
      this.isRunningEmulation = true;
      (this.$refs.assemblerContainer as HTMLDivElement).style.width = '20%';
    },
    stopEmulation(): void {
      this.isRunningEmulation = false;
      (this.$refs.assemblerContainer as HTMLDivElement).style.width = '100%';
    },
    onCurrentAssemblyLineChanged(
      newRunningAssemblyLine: TAsAssembledCode
    ): void {
      this.currentRunningAssemblyLineIdx =
        newRunningAssemblyLine?.matchingSourceLine;
    },
  },
  components: { Assembler, Emulator, Multipane, MultipaneResizer },
});
</script>
