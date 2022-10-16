<template>
  <b-loading
    v-if="!isLoaded"
    :is-full-page="true"
    :active="!isLoaded"
  />
  <div
    v-else
    class="is-flex has-background-white-ter"
  >
    <Multipane class="is-fullwidth">
      <div
        ref="assemblerContainer"
        :class="{ 'is-fullwidth': !isRunningEmulation }"
        style="min-width: 200px"
      >
        <WasmAssembler
          :is-running-emulation="isRunningEmulation"
          :highlight-line-idx="currentRunningAssemblyLineIdx"
          @start-emulation="startEmulation"
          @stop-emulation="stopEmulation"
        />
      </div>
      <MultipaneResizer v-show="isRunningEmulation" />
      <div
        v-show="isRunningEmulation"
        class="is-flex-grow-1"
        style="min-width: 50%"
      >
        <WasmEmulator
          :is-running-emulation="isRunningEmulation"
          :assembled-code="loadedAssembledCode"
          @current-assembly-line-changed="onCurrentAssemblyLineChanged"
        />
      </div>
    </Multipane>
  </div>
</template>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';
import { Multipane, MultipaneResizer } from 'vue-multipane';
import WasmAssembler from '@/components/WasmAssembler.vue';
import WasmEmulator from '@/components/WasmEmulator.vue';
import { loadAsm2010, TAsAssembledCode } from '@/asm2010/wrapper';

export default defineComponent({
  components: { WasmAssembler, WasmEmulator, Multipane, MultipaneResizer },
  data: () => ({
    isLoaded: false,
    isRunningEmulation: false,
    loadedAssembledCode: [] as TAsAssembledCode[],
    currentRunningAssemblyLineIdx: undefined as number | undefined,
  }),
  computed: {},
  async mounted() {
    await loadAsm2010();
    this.isLoaded = true;
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
});
</script>

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
