<template>
  <b-loading v-if="isLoading" :is-full-page="true" :active="true" />
  <div v-else class="is-flex has-background-white-ter">
    <Multipane class="is-fullwidth">
      <div
        :class="{ 'is-fullwidth': !isEmulationRunning }"
        :style="{
          'min-width': '200px',
          width: isEmulationRunning ? '20%' : '100%',
        }"
      >
        <WasmAssembler
          :is-running-emulation="isEmulationRunning"
          :highlight-line-idx="currentAssemblyInstructionIdx"
          @start-emulation="startEmulation"
          @stop-emulation="stopEmulation"
        />
      </div>
      <MultipaneResizer v-show="isEmulationRunning" />
      <div
        v-show="isEmulationRunning"
        class="is-flex-grow-1"
        style="min-width: 50%"
      >
        <WasmEmulator
          :is-running-emulation="isEmulationRunning"
          :assembled-instructions="loadedAssemblyInstructions"
          @current-assembly-line-changed="onCurrentAssembledInstructionChanged"
        />
      </div>
    </Multipane>
  </div>
</template>

<script lang="ts">
import WasmAssembler from '@/components/assembler/WasmAssembler.vue';
import WasmEmulator from '@/components/emulator/WasmEmulator.vue';
import { TCsAssemblyInstruction } from '@/core/ts/types';
import { useGlobalStore } from '@/stores/global';
import { defineComponent } from '@vue/composition-api';
import { mapActions } from 'pinia';
import { Multipane, MultipaneResizer } from 'vue-multipane';

export default defineComponent({
  components: { WasmAssembler, WasmEmulator, Multipane, MultipaneResizer },
  data: () => ({
    isLoading: true,
    isEmulationRunning: false,
    loadedAssemblyInstructions: [] as TCsAssemblyInstruction[],
    currentAssemblyInstructionIdx: undefined as number | undefined,
  }),
  async mounted() {
    await this.init();
    this.isLoading = false;
  },
  methods: {
    ...mapActions(useGlobalStore, ['init']),
    startEmulation(assemblyInstructions: TCsAssemblyInstruction[]): void {
      this.loadedAssemblyInstructions = assemblyInstructions;
      this.isEmulationRunning = true;
    },
    stopEmulation(): void {
      this.isEmulationRunning = false;
    },
    onCurrentAssembledInstructionChanged(
      newAssemblyInstruction: TCsAssemblyInstruction
    ): void {
      this.currentAssemblyInstructionIdx =
        newAssemblyInstruction?.matchingSourceLine;
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
