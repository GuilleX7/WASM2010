<template>
  <div class="is-fullwidth is-viewheight">
    <ExamplesSidebar
      :open="showExamplesSidebar"
      @close="showExamplesSidebar = false"
      @load="showLoadFileDialog"
    >
    </ExamplesSidebar>
    <div class="is-flex is-flex-direction-column is-fullheight">
      <div class="asm--assembler-menu">
        <span class="mr-4 is-text-ellipsable">WASM2010</span>
        <template v-if="!isRunningEmulation">
          <b-dropdown>
            <template #trigger>
              <b-button type="is-text">File</b-button>
            </template>
            <b-dropdown-item @click="showNewFileDialog">New</b-dropdown-item>
            <b-dropdown-item @click="showOpenFileDialog">Open</b-dropdown-item>
            <b-dropdown-item @click="showSaveFileDialog">Save</b-dropdown-item>
            <b-dropdown-item @click="showSaveAsFileDialog"
              >Save as</b-dropdown-item
            >
            <b-dropdown-item separator></b-dropdown-item>
            <b-dropdown-item @click="showExamplesSidebar = true"
              >Examples</b-dropdown-item
            >
          </b-dropdown>
          <b-dropdown class="m-0">
            <template #trigger>
              <b-button type="is-text">View</b-button>
            </template>
            <b-dropdown-item @click="showOutput = !showOutput">{{
              `Toggle (${showOutput ? 'hide' : 'show'}) output`
            }}</b-dropdown-item>
            <b-dropdown-item @click="showOutputInBase(2)"
              >Show output in binary</b-dropdown-item
            >
            <b-dropdown-item @click="showOutputInBase(10)"
              >Show output in decimal</b-dropdown-item
            >
            <b-dropdown-item @click="showOutputInBase(16)"
              >Show output in hexadecimal</b-dropdown-item
            >
          </b-dropdown>
        </template>
      </div>
      <div class="asm--assembler-editor">
        <div class="asm--assembler-code-metadata" ref="sourceMetadataContainer">
          <div
            v-for="(
              assembledMachineCode, lineIdx
            ) in assembledMachineCodePerLine"
            :key="lineIdx"
            class="asm--assembler-code-metadata-line"
            :class="{
              'has-background-danger has-text-light': wrongLineNumbers.includes(
                lineIdx + 1
              ),
              'has-background-dark has-text-light':
                isRunningEmulation &&
                currentRunningAssemblyLineIdx === lineIdx + 1,
            }"
          >
            <span>{{ lineIdx + 1 }}</span>
            <span v-show="showOutput && !isRunningEmulation">{{
              assembledMachineCode
            }}</span>
          </div>
        </div>
        <div
          class="asm--assembler-code-container"
          ref="sourceCodeContainer"
          @scroll="onSourceScrolled"
        >
          <div
            class="asm--assembler-code-wrapper"
            :data-replicated-value="sourceAssembly"
          >
            <textarea
              spellcheck="false"
              :readonly="isRunningEmulation"
              v-model="sourceAssembly"
              @keydown.tab.prevent="onSourceTabbed"
            ></textarea>
          </div>
        </div>
      </div>
      <div
        class="asm--assembler-statusbar"
        :class="{
          'has-background-danger has-text-light': isAssemblyFailed,
        }"
      >
        <div
          v-if="!isAssemblySuccessful"
          class="asm--assembler-statusbar-text is-flex is-align-items-center"
          :class="{ 'is-flex-grow-1': isAssemblyFailed }"
          :title="currentStatus"
        >
          <span v-if="isAssemblyPending" class="bulma-loader-mixin mr-3"></span>
          <span class="is-text-ellipsable">
            {{ currentStatus }}
          </span>
        </div>
        <div
          class="asm--assembler-statusbar-action-buttons"
          :class="{ 'is-flex-grow-1': !isAssemblyFailed }"
        >
          <template v-if="isAssemblyFailed && logLines.length > 1">
            <b-button
              type="is-danger"
              size="is-small"
              icon-left="chevron-left"
              @click="navigateLog(-1)"
            ></b-button>
            <b-button
              type="is-danger"
              size="is-small"
              icon-left="chevron-right"
              @click="navigateLog(1)"
            ></b-button>
            <span>{{ currentLogLineIndex + 1 }} / {{ logLines.length }}</span>
          </template>
          <template v-if="isAssemblySuccessful">
            <b-button
              v-if="!isRunningEmulation"
              type="is-light"
              icon-left="play"
              :expanded="true"
              @click="$emit('start-emulation', assembledCode)"
              >Start emulation</b-button
            >
            <b-button
              v-else
              type="is-light"
              icon-left="stop"
              :expanded="true"
              @click="$emit('stop-emulation')"
              >Stop emulation</b-button
            >
          </template>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="scss">
.asm--assembler-menu {
  display: flex;
  padding-left: 0.75rem;
  align-items: center;
  height: 40px;
  flex-shrink: 0;
}

.asm--assembler-editor {
  display: flex;
  flex-direction: row;
  flex-grow: 1;
  font-family: monospace;
  overflow: hidden;

  .asm--assembler-code-metadata {
    display: flex;
    flex-direction: column;
    overflow: hidden;
    flex-shrink: 0;

    .asm--assembler-code-metadata-line {
      display: flex;
      width: 100%;
      padding: 0 0.75rem;
      justify-content: space-between;
      transition: background-color 0.25s;

      & > span:nth-child(2n) {
        margin-left: 0.75rem;
      }

      &:first-child {
        margin-top: 0.75rem;
      }

      &:last-child {
        margin-bottom: 0.75rem;
      }
    }
  }

  .asm--assembler-code-container {
    width: 100%;
    overflow: auto;
  }

  .asm--assembler-code-wrapper {
    display: grid;
    width: 100%;
    height: 100%;
  }

  .asm--assembler-code-wrapper::after {
    content: attr(data-replicated-value) ' ';
    visibility: hidden;
  }

  .asm--assembler-code-wrapper > textarea {
    resize: none;
    overflow: hidden;
  }

  .asm--assembler-code-wrapper > textarea,
  .asm--assembler-code-wrapper::after {
    border: none;
    padding: 0.75rem;
    font: inherit;
    grid-area: 1 / 1 / 2 / 2;
    white-space: pre;
  }
}

.asm--assembler-statusbar {
  width: 100%;
  flex-shrink: 0;
  display: flex;
  align-items: center;
  transition: all 0.25s;
  height: 40px;

  .asm--assembler-statusbar-text {
    margin: 0px 0.75rem;
  }

  .asm--assembler-statusbar-action-buttons {
    display: flex;
    flex-shrink: 0;
    align-items: center;

    & > span {
      margin: 0px 0.75rem;
    }
  }
}
</style>

<script lang="ts">
import {
  formatNumber,
  maxAmountOfDigitsToRepresentNumber,
  stringFilledWith,
} from '@/utils/format';
import ExamplesSidebar from './ExamplesSidebar.vue';
import { TAsAssembledCode, asAssemble } from '@/wasm/asm2010';
import { positiveMod } from '@/utils/math';
import { downloadFile } from '@/utils/file';
import { defineComponent } from '@vue/composition-api';

export default defineComponent({
  props: {
    isRunningEmulation: {
      default: false,
      type: Boolean,
    },
    currentRunningAssemblyLineIdx: {
      default: undefined,
      type: Number,
    },
  },
  data: () => ({
    sourceAssembly: '',
    debouncedSourceAssembly: '',
    debounceSourceAssemblyTimer: undefined as undefined | number,
    assembledCode: [] as TAsAssembledCode[],
    isAssembling: false,
    log: '',
    currentLogLineIndex: 0,
    showOutput: true,
    outputBase: 16,
    showExamplesSidebar: false,
  }),
  computed: {
    sourceAssemblyLines(): string[] {
      return this.sourceAssembly.split('\n');
    },
    assembledMachineCodePerLine(): string[] {
      let currentAssembledCodeLineIdx = 0;
      return this.sourceAssemblyLines.map((_, i) => {
        const currentAssembledCodeLine =
          this.assembledCode[currentAssembledCodeLineIdx];
        if (currentAssembledCodeLine?.matchingSourceLine === i + 1) {
          currentAssembledCodeLineIdx++;
          return formatNumber(
            currentAssembledCodeLine.machineCode,
            this.outputBase,
            16,
            this.outputBase === 16 ? '0x' : ''
          );
        } else {
          return stringFilledWith(
            '-',
            maxAmountOfDigitsToRepresentNumber(this.outputBase, 16) +
              (this.outputBase === 16 ? 2 : 0)
          );
        }
      });
    },
    logLines(): string[] {
      return !this.isAssemblyPending
        ? this.log.split('\n').filter(Boolean)
        : [];
    },
    currentStatus(): string {
      switch (true) {
        case this.isAssembling:
          return 'Pending...';
        case this.isAssemblyFailed:
          return this.logLines[this.currentLogLineIndex];
        default:
          return '';
      }
    },
    wrongLineNumbers(): number[] {
      const wrongLineNumbers: number[] = [];
      for (const logLine of this.logLines) {
        const wrongLineNumber = logLine.match(/at line (\d+)/)?.[1];
        if (wrongLineNumber) {
          wrongLineNumbers.push(Number.parseInt(wrongLineNumber));
        }
      }
      return wrongLineNumbers;
    },
    isAssemblySuccessful(): boolean {
      return !this.logLines.length && !this.isAssemblyPending;
    },
    isAssemblyFailed(): boolean {
      return Boolean(this.logLines.length) && !this.isAssemblyPending;
    },
    isAssemblyPending(): boolean {
      return this.isAssembling;
    },
  },
  methods: {
    showOutputInBase(base: number): void {
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
    onSourceScrolled(): void {
      const { sourceCodeContainer, sourceMetadataContainer } = this
        .$refs as Record<string, HTMLDivElement>;

      sourceMetadataContainer.scrollTop = sourceCodeContainer.scrollTop;
    },
    navigateLog(offset: number): void {
      this.currentLogLineIndex = positiveMod(
        this.currentLogLineIndex + offset,
        this.logLines.length
      );
    },
    assemble(): void {
      const { assembledCode, log } = asAssemble(this.sourceAssembly);
      this.assembledCode = assembledCode;
      this.log = log;
    },
    showLoadFileDialog(fileName: string, source: string) {
      this.$buefy.dialog.confirm({
        title: 'Load file',
        message: `Are you sure you want to load <b>${fileName}</b>? You will lose your changes.`,
        type: 'is-danger',
        hasIcon: true,
        onConfirm: () => (this.sourceAssembly = source),
      });
    },
    showNewFileDialog(): void {
      this.$buefy.dialog.confirm({
        title: 'New file',
        message:
          'Do you really want to create a new file? You will lose your changes.',
        type: 'is-danger',
        hasIcon: true,
        onConfirm: () => (this.sourceAssembly = ''),
      });
    },
    showOpenFileDialog(): void {
      const fileInput = document.createElement('input');
      fileInput.type = 'file';
      fileInput.accept = '.asm,text/plain';
      fileInput.onchange = (fileSelectedEvent: any) => {
        const file = fileSelectedEvent.target.files[0];
        const fileReader = new FileReader();
        fileReader.readAsText(file, 'UTF-8');
        fileReader.onload = (fileLoadedEvent: any) => {
          this.showLoadFileDialog(file.name, fileLoadedEvent.target.result);
        };
      };
      fileInput.click();
    },
    showSaveFileDialog(): void {
      downloadFile(this.sourceAssembly, 'assembly.asm', 'text/plain');
    },
    showSaveAsFileDialog(): void {
      this.$buefy.dialog.prompt({
        title: 'Save file',
        message: `Enter the name of the file:`,
        inputAttrs: {
          placeholder: 'e.g. assembly.asm',
        },
        onConfirm: (filename) =>
          downloadFile(
            this.sourceAssembly,
            `${filename}${!filename.includes('.') ? '.asm' : ''}`,
            'text/plain'
          ),
      });
    },
    formatNumber,
  },
  watch: {
    sourceAssembly() {
      this.isAssembling = true;
      clearTimeout(this.debounceSourceAssemblyTimer);
      this.debounceSourceAssemblyTimer = setTimeout(
        () => (this.debouncedSourceAssembly = this.sourceAssembly),
        250
      ) as any;
    },
    debouncedSourceAssembly() {
      this.assemble();
      this.isAssembling = false;
    },
    logLines() {
      if (
        this.logLines.length &&
        this.currentLogLineIndex > this.logLines.length - 1
      ) {
        this.currentLogLineIndex = this.logLines.length - 1;
      }
    },
  },
  components: {
    ExamplesSidebar,
  },
});
</script>
