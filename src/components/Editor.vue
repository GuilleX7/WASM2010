<template>
  <div class="is-fullheight is-fullwidth">
    <ExamplesSidebar
      :open="showExamplesSidebar"
      @close="showExamplesSidebar = false"
      @load="loadSourceAssembly"
    ></ExamplesSidebar>
    <div class="is-flex is-flex-direction-column is-fullheight">
      <div class="asm--source-menu">
        <span class="mr-4 is-text-ellipsable">Source assembly</span>
        <b-dropdown>
          <template #trigger>
            <b-button type="is-text">File</b-button>
          </template>
          <b-dropdown-item @click="sourceAssembly = ''">New</b-dropdown-item>
          <b-dropdown-item>Open</b-dropdown-item>
          <b-dropdown-item>Save as</b-dropdown-item>
          <b-dropdown-item separator></b-dropdown-item>
          <b-dropdown-item @click="showExamplesSidebar = true"
            >Examples</b-dropdown-item
          >
          <b-dropdown-item separator></b-dropdown-item>
          <b-dropdown-item>Load into CS2010</b-dropdown-item>
        </b-dropdown>
        <b-dropdown class="m-0">
          <template #trigger>
            <b-button type="is-text">View</b-button>
          </template>
          <b-dropdown-item @click="showOutput = !showOutput">{{
            `Toggle (${showOutput ? 'hide' : 'show'}) output`
          }}</b-dropdown-item>
          <b-dropdown-item @click="changeOutputBase(2)"
            >Show output in binary</b-dropdown-item
          >
          <b-dropdown-item @click="changeOutputBase(10)"
            >Show output in decimal</b-dropdown-item
          >
          <b-dropdown-item @click="changeOutputBase(16)"
            >Show output in hexadecimal</b-dropdown-item
          >
        </b-dropdown>
      </div>
      <div class="asm--source-editor">
        <div class="asm--source-line-numbers" ref="sourceLineNumbers">
          <span
            v-for="i in numberOfSourceLines"
            :class="{ 'has-text-danger': wrongLineNumbers.includes(i) }"
            >{{ i }}</span
          >
        </div>
        <div
          class="asm--source-code-container"
          ref="sourceContainer"
          @scroll="onSourceScrolled"
        >
          <div
            class="asm--source-code-wrapper"
            :data-replicated-value="sourceAssembly"
          >
            <textarea
              spellcheck="false"
              v-model="sourceAssembly"
              @keydown.tab.prevent="onSourceTabbed"
            ></textarea>
          </div>
        </div>
        <div v-if="showOutput" class="asm--output-code" ref="outputContainer">
          <template v-if="outputMachineCode.length">
            <span v-for="machineCodeLine in outputMachineCode">{{
              formatNumber(machineCodeLine, outputBase, 16)
            }}</span>
          </template>
          <template v-else>
            <span v-for="i in numberOfSourceLines">?</span>
          </template>
        </div>
      </div>
      <div
        class="asm--output-log is-text-ellipsable"
        :class="{
          'has-background-danger-dark': assemblyHasFailed,
          'has-text-info-light': assemblyHasFailed,
        }"
      >
        {{ outputLog || 'Syntax OK' }}
      </div>
    </div>
  </div>
</template>

<style lang="scss">
.asm--source-menu {
  display: flex;
  padding-left: 2.5rem;
  align-items: center;
}

.asm--source-editor {
  display: flex;
  flex-direction: row;
  height: calc(100vh - 40px - 44px);

  .asm--source-line-numbers,
  .asm--output-code {
    display: flex;
    flex-direction: column;
    padding: calc(0.75em - 1px);
    overflow: hidden;
    flex-shrink: 0;
  }

  .asm--source-code-container {
    width: 100%;
    overflow: auto;
  }

  .asm--source-code-wrapper {
    display: grid;
    width: 100%;
    height: 100%;
  }

  .asm--source-code-wrapper::after {
    content: attr(data-replicated-value) ' ';
    white-space: pre-wrap;
    visibility: hidden;
  }

  .asm--source-code-wrapper > textarea {
    resize: none;
    overflow: hidden;
  }

  .asm--source-code-wrapper > textarea,
  .asm--source-code-wrapper::after {
    padding: 0.75rem;
    font: inherit;
    grid-area: 1 / 1 / 2 / 2;
    font-family: monospace;
  }
}

.asm--output-log {
  width: 100%;
  padding: 10px 2.5rem;
  flex-shrink: 0;
}
</style>

<script lang="ts">
import { useWasmStore } from '@/store/wasmStore';
import { mapActions } from 'pinia';
import Vue from 'vue';
import { formatNumber } from '@/utils/format';
import ExamplesSidebar from './ExamplesSidebar.vue';

export default Vue.extend({
  data: () => ({
    sourceAssembly: '',
    outputMachineCode: [] as number[],
    outputLog: '',
    showOutput: true,
    outputBase: 16,
    showExamplesSidebar: false,
  }),
  computed: {
    numberOfSourceLines(): number {
      return this.sourceAssembly.split('\n').length;
    },
    assemblyHasFailed(): boolean {
      return !this.outputMachineCode.length && Boolean(this.outputLog.length);
    },
    wrongLineNumbers(): number[] {
      const wrongLines: number[] = [];
      if (!this.outputMachineCode.length && this.outputLog) {
        for (const outputLogLine of this.outputLog.split('\n')) {
          const wrongLineNumber = outputLogLine.match(/at line (\d+)/)?.[1];
          if (wrongLineNumber) {
            wrongLines.push(Number.parseInt(wrongLineNumber));
          }
        }
      }
      return wrongLines;
    },
  },
  methods: {
    ...mapActions(useWasmStore, {
      assembleSourceCode: 'assemble',
    }),
    loadSourceAssembly(sourceAssembly: string) {
      this.sourceAssembly = sourceAssembly;
    },
    changeOutputBase(base: number): void {
      this.showOutput = true;
      this.outputBase = base;
    },
    onSourceTabbed(event: any): void {
      const start = event.target.selectionStart;
      const end = event.target.selectionEnd;
      const tab = '    ';

      this.sourceAssembly =
        this.sourceAssembly.substring(0, start) +
        tab +
        this.sourceAssembly.substring(end);

      event.target.value = this.sourceAssembly;
      event.target.selectionEnd = event.target.selectionStart =
        start + tab.length;
    },
    onSourceScrolled(event: any): void {
      const { sourceContainer, sourceLineNumbers, outputContainer } = this
        .$refs as Record<string, HTMLDivElement>;

      sourceLineNumbers.scrollTop = sourceContainer.scrollTop;
      if (outputContainer) {
        outputContainer.scrollTop = sourceContainer.scrollTop;
      }
    },
    assemble(): void {
      const { machineCode, log } = this.assembleSourceCode(this.sourceAssembly);
      this.outputMachineCode = machineCode;
      this.outputLog = log;
    },
    formatNumber,
  },
  watch: {
    sourceAssembly() {
      this.assemble();
    },
  },
  components: {
    ExamplesSidebar,
  },
});
</script>
