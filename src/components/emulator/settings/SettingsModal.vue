<template>
  <form action="">
    <div class="modal-card">
      <header class="modal-card-head">
        <p class="modal-card-title">
          Settings
        </p>
        <button
          type="button"
          class="delete"
          @click="$emit('close')"
        />
      </header>
      <section class="modal-card-body">
        <b-tabs
          v-model="activeTab"
          type="is-toggle-rounded"
          expanded
          :animated="false"
        >
          <b-tab-item label="Emulation">
            <b-field grouped>
              <b-field
                label="Clock frequency (Hz)"
                expanded
                :type="{ 'is-danger': !isClockRunningFrequencyValid }"
                :message="{
                  'Invalid clock frequency (min. 1, max. 100000)':
                    !isClockRunningFrequencyValid,
                }"
              >
                <b-input
                  v-model="newSettings.clockRunningFrequency"
                  type="number"
                />
              </b-field>
              <b-field
                label="Interface refresh speed (Hz)"
                expanded
                :type="{ 'is-danger': !isUiRefreshFrequencyValid }"
                :message="{
                  'Invalid refresh speed (min. 1, max. 60)':
                    !isUiRefreshFrequencyValid,
                }"
              >
                <b-input
                  v-model="newSettings.uiRefreshFrequency"
                  type="number"
                />
              </b-field>
            </b-field>
            <b-field label="Execute instructions in a single clock cycle">
              <b-switch v-model="newSettings.skipMicroinstructions">
                {{ newSettings.skipMicroinstructions ? 'Yes' : 'No' }}
              </b-switch>
            </b-field>
            <b-field
              label="Max instructions before halting block step"
              :type="{
                'is-danger': !isMaxInstructionsBeforeHaltingBlockStepValid,
              }"
              :message="{
                'Invalid amount of instructions (min. 1, max. 100000)':
                  !isMaxInstructionsBeforeHaltingBlockStepValid,
              }"
            >
              <b-input
                v-model="newSettings.maxInstructionsBeforeHaltingBlockStep"
                type="number"
              />
            </b-field>
          </b-tab-item>

          <b-tab-item label="User interface">
            <b-field grouped>
              <b-field
                label="ROM content radix"
                expanded
              >
                <b-select
                  v-model="newSettings.romDisplayableRadix"
                  expanded
                >
                  <option
                    v-for="radix in availableRadices"
                    :key="radix.value"
                    :value="radix.value"
                  >
                    {{ radix.name }}
                  </option>
                </b-select>
              </b-field>
              <b-field
                label="RAM content radix"
                expanded
              >
                <b-select
                  v-model="newSettings.ramDisplayableRadix"
                  expanded
                >
                  <option
                    v-for="radix in availableRadices"
                    :key="radix.value"
                    :value="radix.value"
                  >
                    {{ radix.name }}
                  </option>
                </b-select>
              </b-field>
              <b-field
                label="Register content radix"
                expanded
              >
                <b-select
                  v-model="newSettings.registerDisplayableRadix"
                  expanded
                >
                  <option
                    v-for="radix in availableRadices"
                    :key="radix.value"
                    :value="radix.value"
                  >
                    {{ radix.name }}
                  </option>
                </b-select>
              </b-field>
            </b-field>
            <b-field
              label="RAM words per row"
              :type="{
                'is-danger': !isRamWordsPerRowValid,
              }"
              :message="{
                'Invalid amount of words (min. 1, max. 32)':
                  !isRamWordsPerRowValid,
              }"
            >
              <b-input
                v-model="newSettings.ramWordsPerRow"
                type="number"
                expanded
              />
            </b-field>
          </b-tab-item>

          <b-tab-item label="Input / output">
            <b-table :data="mappedIoComponents">
              <b-table-column
                v-slot="{ row: { address } }"
                field="address"
                label="Address"
                sortable
              >
                {{ formatNumber(address, 16, 8, '0x') }}
              </b-table-column>
              <b-table-column
                v-slot="{ row: { componentId } }"
                field="componentId"
                label="Component"
                sortable
              >
                {{ getIoComponentName(componentId) }}
              </b-table-column>
              <b-table-column
                v-slot="{ row: { address } }"
                label="Actions"
              >
                <b-button
                  type="is-danger is-light"
                  icon-right="delete"
                  @click="removeIoComponentAtAddress(address)"
                />
              </b-table-column>
              <template #empty>
                <p>No mapped IO components yet.</p>
              </template>
            </b-table>
            <br>
            <b-field grouped>
              <b-field
                :type="{
                  'is-danger': !isNewComponentAddressValid,
                }"
              >
                <p class="control">
                  <span class="button is-static">0x</span>
                </p>
                <b-input
                  v-model="newComponentAddress"
                  expanded
                  :style="{ width: '100px' }"
                />
              </b-field>
              <b-select
                v-model="newComponentId"
                expanded
              >
                <option
                  v-for="ioComponent in availableIoComponents"
                  :key="ioComponent.id"
                  :value="ioComponent.id"
                >
                  {{ ioComponent.name }}
                </option>
              </b-select>
              <b-button
                label="Add component"
                @click="addIoComponent"
              />
            </b-field>
            <b-field
              :type="{
                'is-danger': !isNewComponentAddressValid,
              }"
              :message="{
                'Invalid address (up to two hexadecimal digits)':
                  !isNewComponentAddressValid,
              }"
            />
          </b-tab-item>
        </b-tabs>
      </section>
      <footer class="modal-card-foot is-justify-content-end">
        <b-button
          label="Close"
          @click="$emit('close')"
        />
        <b-button
          label="Save changes"
          type="is-primary"
          :disabled="!areSettingsValid"
          @click="saveChanges"
        />
      </footer>
    </div>
  </form>
</template>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';
import {
  TEmulatorSettings,
  TMappedIoComponent,
} from '@/components/emulator/settings';
import {
  IoComponentId,
  registeredIoComponents,
  TIoComponentDefinition,
} from '../io';
import { formatNumber } from '@/utils/format';

export default defineComponent({
  props: {
    settings: {
      required: true,
      type: Object as PropType<TEmulatorSettings>,
    },
  },
  data: function () {
    return {
      newSettings: {
        ...this.settings,
      } as TEmulatorSettings,
      activeTab: undefined as unknown,
      newComponentAddress: '00',
      newComponentId: IoComponentId.Buttons,
      areNewSettingsDirty: false,
    };
  },
  computed: {
    areSettingsValid(): boolean {
      return (
        this.isClockRunningFrequencyValid &&
        this.isUiRefreshFrequencyValid &&
        this.isMaxInstructionsBeforeHaltingBlockStepValid &&
        this.isRamWordsPerRowValid
      );
    },
    isClockRunningFrequencyValid(): boolean {
      return (
        this.newSettings.clockRunningFrequency >= 1 &&
        this.newSettings.clockRunningFrequency <= 100000
      );
    },
    isUiRefreshFrequencyValid(): boolean {
      return (
        this.newSettings.uiRefreshFrequency >= 1 &&
        this.newSettings.uiRefreshFrequency <= 60
      );
    },
    isMaxInstructionsBeforeHaltingBlockStepValid(): boolean {
      return (
        this.newSettings.maxInstructionsBeforeHaltingBlockStep >= 1 &&
        this.newSettings.maxInstructionsBeforeHaltingBlockStep <= 100000
      );
    },
    isRamWordsPerRowValid(): boolean {
      return (
        this.newSettings.ramWordsPerRow >= 1 &&
        this.newSettings.ramWordsPerRow <= 32
      );
    },
    isNewComponentAddressValid(): boolean {
      return /^[0-9A-F]{1,2}$/gi.test(this.newComponentAddress);
    },
    mappedIoComponents(): TMappedIoComponent[] {
      return Object.entries(this.newSettings.mappedIoComponents).map(
        ([address, componentId]) => ({
          address: Number.parseInt(address),
          componentId: componentId as IoComponentId,
        })
      );
    },
    availableIoComponents(): TIoComponentDefinition[] {
      return Object.values(registeredIoComponents);
    },
    availableRadices(): {
      name: string;
      value: number;
    }[] {
      return [
        { name: 'Hexadecimal', value: 16 },
        { name: 'Decimal', value: 10 },
        { name: 'Binary', value: 2 },
      ];
    },
  },
  methods: {
    saveChanges(): void {
      const sanitizedSettings: TEmulatorSettings = {
        ...this.newSettings,
        clockRunningFrequency: Number.parseInt(
          this.newSettings.clockRunningFrequency as unknown as string
        ),
        maxInstructionsBeforeHaltingBlockStep: Number.parseInt(
          this.newSettings
            .maxInstructionsBeforeHaltingBlockStep as unknown as string
        ),
        ramWordsPerRow: Number.parseInt(
          this.newSettings.ramWordsPerRow as unknown as string
        ),
      };
      this.$emit('save-changes', sanitizedSettings);
    },
    getIoComponentName(componentId: IoComponentId): string {
      return registeredIoComponents[componentId].name;
    },
    removeIoComponentAtAddress(addressToRemove: number): void {
      this.newSettings.mappedIoComponents = Object.fromEntries(
        Object.entries(this.newSettings.mappedIoComponents).filter(
          ([address]) => parseInt(address) !== addressToRemove
        )
      );
    },
    addIoComponent(): void {
      this.newSettings.mappedIoComponents = {
        ...this.newSettings.mappedIoComponents,
        [parseInt(this.newComponentAddress, 16)]: this.newComponentId,
      };
    },
    formatNumber,
  },
});
</script>
