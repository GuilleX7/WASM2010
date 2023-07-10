<template>
  <b-loading v-if="isLoading" :is-full-page="true" :active="true" />
  <div
    v-else
    class="is-fullwidth is-viewheight"
    :class="{ 'is-flex': !isRunningEmulation }"
  >
    <ExamplesSidebar
      :open="isExamplesSidebarOpen"
      @close="showExamplesSidebar = false"
      @load="showLoadFileDialog"
    />
    <div class="is-flex is-flex-direction-column is-fullheight is-flex-grow-1">
      <div class="asm--assembler-menu">
        <span class="asm--asembler-menu-title is-text-ellipsable">{{
          displayableCsPlatform
        }}</span>
        <template v-if="!isRunningEmulation">
          <b-dropdown>
            <template #trigger>
              <b-button type="is-text">File</b-button>
            </template>
            <b-dropdown-item @click="showNewFileDialog"> New </b-dropdown-item>
            <b-dropdown-item @click="showOpenFileDialog">
              Open
            </b-dropdown-item>
            <b-dropdown-item @click="showSaveFileDialog">
              Save
            </b-dropdown-item>
            <b-dropdown-item @click="showSaveAsFileDialog">
              Save as
            </b-dropdown-item>
            <b-dropdown-item @click="showExamplesSidebar = true">
              Examples
            </b-dropdown-item>
          </b-dropdown>
          <b-dropdown class="m-0">
            <template #trigger>
              <b-button type="is-text">View</b-button>
            </template>
            <b-dropdown-item @click="showOutput = !showOutput">
              {{ `Toggle output (${showOutput ? 'hide' : 'show'})` }}
            </b-dropdown-item>
            <b-dropdown-item @click="showOutputInRadix(2)">
              Show output in binary
            </b-dropdown-item>
            <b-dropdown-item @click="showOutputInRadix(10)">
              Show output in decimal
            </b-dropdown-item>
            <b-dropdown-item @click="showOutputInRadix(16)">
              Show output in hexadecimal
            </b-dropdown-item>
          </b-dropdown>
          <b-dropdown class="m-0">
            <template #trigger>
              <b-button type="is-text">Platform</b-button>
            </template>
            <b-dropdown-item @click="switchToCs2010">
              Switch to CS2010
            </b-dropdown-item>
            <b-dropdown-item :active="true" @click="switchToCs3">
              Switch to CS3
            </b-dropdown-item>
          </b-dropdown>
        </template>
      </div>
      <div ref="sourceEditorContainer" class="asm--assembler-editor">
        <div ref="sourceMetadataContainer" class="asm--assembler-code-metadata">
          <div
            v-for="(assembledMachineCode, lineIdx) in sourceAssemblyMetadata"
            :key="lineIdx"
            ref="sourceLinesMetadata"
            class="asm--assembler-code-metadata-line"
            :class="{
              'has-background-danger has-text-light': wrongLineNumbers.includes(
                lineIdx + 1
              ),
              'has-background-dark has-text-light':
                isRunningEmulation && highlightLineIdx === lineIdx + 1,
            }"
          >
            <span>{{ lineIdx + 1 }}</span>
            <span v-show="showOutput && !isRunningEmulation">{{
              assembledMachineCode
            }}</span>
          </div>
        </div>
        <div
          ref="sourceCodeContainer"
          class="asm--assembler-code-container"
          @scroll="onSourceScrolled"
        >
          <div
            class="asm--assembler-code-wrapper"
            :data-replicated-value="sourceAssembly"
          >
            <textarea
              v-model="sourceAssembly"
              spellcheck="false"
              :readonly="isRunningEmulation"
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
          <span v-if="isAssembling" class="bulma-loader-mixin mr-3" />
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
            />
            <b-button
              type="is-danger"
              size="is-small"
              icon-left="chevron-right"
              @click="navigateLog(1)"
            />
            <span>{{ currentLogLineIndex + 1 }} / {{ logLines.length }}</span>
          </template>
          <template v-if="isAssemblySuccessful">
            <b-button
              v-if="!isRunningEmulation"
              type="is-light"
              icon-left="play"
              :expanded="true"
              @click="$emit('start-emulation', assembledInstructions)"
            >
              Start emulation
            </b-button>
            <b-button
              v-else
              type="is-light"
              icon-left="stop"
              :expanded="true"
              @click="$emit('stop-emulation')"
            >
              Stop emulation
            </b-button>
          </template>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import ExamplesSidebar from '@/components/assembler/modules/ExamplesSidebar.vue';
import { getAsm2010Instance } from '@/core/ts';
import { CsPlatform, TCsAssemblyInstruction } from '@/core/ts/types';
import { useAssemblerStore } from '@/stores/assembler';
import { useGlobalStore } from '@/stores/global';
import { downloadFile } from '@/utils/file';
import {
formatNumber,
maxAmountOfDigitsToRepresentNumber,
stringFilledWith,
} from '@/utils/format';
import { positiveMod } from '@/utils/math';
import { countLines } from '@/utils/string';
import { defineComponent } from '@vue/composition-api';
import { mapActions, mapWritableState } from 'pinia';

export default defineComponent({
  components: {
    ExamplesSidebar,
  },
  props: {
    isRunningEmulation: {
      default: false,
      type: Boolean,
    },
    highlightLineIdx: {
      default: undefined,
      type: Number,
    },
  },
  data: () => ({
    sourceAssembly: '',
    debounceAssemblyTimer: undefined as
      | undefined
      | ReturnType<typeof setTimeout>,
    assembledInstructions: [] as TCsAssemblyInstruction[],
    log: '',
    isAssembling: false,
    currentLogLineIndex: 0,
    showExamplesSidebar: false,
    isLoading: false,
  }),
  computed: {
    ...mapWritableState(useGlobalStore, ['csPlatform']),
    ...mapWritableState(useAssemblerStore, ['outputRadix', 'showOutput']),
    displayableCsPlatform(): string {
      switch (this.csPlatform) {
        case CsPlatform.Cs2010:
        default:
          return 'CS2010';
        case CsPlatform.Cs3:
          return 'CS3';
      }
    },
    isExamplesSidebarOpen(): boolean {
      return this.showExamplesSidebar && !this.isRunningEmulation;
    },
    sourceAssemblyMetadata(): string[] {
      let currentAssembledInstructionIdx = 0;
      return [...Array(countLines(this.sourceAssembly) + 1)].map(
        (_, sourceLineIdx) => {
          const currentAssembledCodeLine =
            this.assembledInstructions[currentAssembledInstructionIdx];
          if (
            !this.isAssembling &&
            currentAssembledCodeLine?.matchingSourceLine === sourceLineIdx + 1
          ) {
            currentAssembledInstructionIdx++;
            return formatNumber(
              currentAssembledCodeLine.machineInstruction,
              this.outputRadix,
              16,
              this.outputRadix === 16 ? '0x' : ''
            );
          } else {
            return stringFilledWith(
              '-',
              maxAmountOfDigitsToRepresentNumber(this.outputRadix, 16) +
                (this.outputRadix === 16 ? 2 : 0)
            );
          }
        }
      );
    },
    logLines(): string[] {
      return !this.isAssembling ? this.log.split('\n').filter(Boolean) : [];
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
      return !this.logLines.length && !this.isAssembling;
    },
    isAssemblyFailed(): boolean {
      return Boolean(this.logLines.length) && !this.isAssembling;
    },
  },
  watch: {
    sourceAssembly() {
      this.isAssembling = true;
      clearTimeout(this.debounceAssemblyTimer);
      this.debounceAssemblyTimer = setTimeout(() => {
        this.assemble();
        this.isAssembling = false;
      }, 300);
    },
    logLines() {
      if (
        this.logLines.length &&
        this.currentLogLineIndex > this.logLines.length - 1
      ) {
        this.currentLogLineIndex = this.logLines.length - 1;
      }
    },
    highlightLineIdx(): void {
      const sourceLineMetadata = (
        this.$refs.sourceLinesMetadata as HTMLTableRowElement[]
      )?.[this.highlightLineIdx];
      if (!sourceLineMetadata) {
        return;
      }

      const {
        sourceCodeContainer,
        sourceMetadataContainer,
        sourceEditorContainer,
      } = this.$refs as Record<string, HTMLDivElement>;

      const lineTopOffset = sourceLineMetadata.offsetTop;
      const finalOffset =
        lineTopOffset - sourceEditorContainer.clientHeight / 2;

      sourceMetadataContainer.scrollTo({
        top: finalOffset,
      });

      sourceCodeContainer.scrollTo({
        top: finalOffset,
      });
    },
  },
  methods: {
    ...mapActions(useGlobalStore, ['switchCsPlatform']),
    async switchToCsPlatform(csPlatform: CsPlatform): Promise<void> {
      this.isLoading = true;
      await this.switchCsPlatform(csPlatform);
      this.assemble();
      this.isLoading = false;
    },
    switchToCs2010(): void {
      this.switchToCsPlatform(CsPlatform.Cs2010);
    },
    switchToCs3(): void {
      this.switchToCsPlatform(CsPlatform.Cs3);
    },
    showOutputInRadix(radix: number): void {
      this.showOutput = true;
      this.outputRadix = radix;
    },
    onSourceTabbed(event: { target: HTMLTextAreaElement }): void {
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
      const { assembledInstructions, log } = getAsm2010Instance().asAssemble(
        this.sourceAssembly
      );
      this.assembledInstructions = assembledInstructions;
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
      fileInput.onchange = (fileSelectedEvent: Event) => {
        const eventInput = fileSelectedEvent.target as HTMLInputElement;
        if (!eventInput) {
          return;
        }
        const file = eventInput.files?.item(0);
        if (!file) {
          return;
        }
        const fileReader = new FileReader();
        fileReader.readAsText(file, 'UTF-8');
        fileReader.onload = () => {
          const fileContent = fileReader.result;
          if (typeof fileContent !== 'string') {
            return;
          }
          this.showLoadFileDialog(file.name, fileContent);
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
});
</script>

<style lang="scss">
.asm--assembler-menu {
  display: flex;
  align-items: center;
  height: 40px;
  flex-shrink: 0;

  .asm--asembler-menu-title {
    margin: 0 1.25rem;
    width: 60px;
    text-align: center;

    &.full {
      margin: 0;
      width: 100%;
    }
  }
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
