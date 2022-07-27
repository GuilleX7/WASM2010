<template>
  <form action="">
    <div class="modal-card">
      <header class="modal-card-head">
        <p class="modal-card-title">Settings</p>
        <button type="button" class="delete" @click="$emit('close')" />
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
                <b-input v-model="newSettings.clockRunningFrequency"></b-input>
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
                <b-input v-model="newSettings.uiRefreshFrequency"></b-input>
              </b-field>
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
              ></b-input>
            </b-field>
          </b-tab-item>

          <b-tab-item label="User interface">
            <b-field grouped>
              <b-field label="ROM content radix" expanded>
                <b-select v-model="newSettings.romDisplayableRadix" expanded>
                  <option
                    v-for="radix in availableRadices"
                    :value="radix.value"
                    :key="radix.value"
                  >
                    {{ radix.name }}
                  </option>
                </b-select>
              </b-field>
              <b-field label="RAM content radix" expanded>
                <b-select v-model="newSettings.ramDisplayableRadix" expanded>
                  <option
                    v-for="radix in availableRadices"
                    :value="radix.value"
                    :key="radix.value"
                  >
                    {{ radix.name }}
                  </option>
                </b-select>
              </b-field>
              <b-field label="Register content radix" expanded>
                <b-select
                  v-model="newSettings.registerDisplayableRadix"
                  expanded
                >
                  <option
                    v-for="radix in availableRadices"
                    :value="radix.value"
                    :key="radix.value"
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
              <b-input v-model="newSettings.ramWordsPerRow" expanded> </b-input>
            </b-field>
          </b-tab-item>
        </b-tabs>
      </section>
      <footer class="modal-card-foot is-justify-content-end">
        <b-button label="Close" @click="$emit('close')" />
        <b-button
          label="Save changes"
          type="is-primary"
          :disabled="!areSettingsValid"
          @click="$emit('save-changes', newSettings)"
        />
      </footer>
    </div>
  </form>
</template>

<script lang="ts">
import { TEmulatorSettings } from '@/types';
import { defineComponent, PropType } from '@vue/composition-api';

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
      activeTab: undefined as any,
    };
  },
  computed: {
    areSettingsValid(): boolean {
      return (
        this.isClockRunningFrequencyValid &&
        this.isUiRefreshFrequencyValid &&
        this.isMaxInstructionsBeforeHaltingBlockStepValid
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
});
</script>
