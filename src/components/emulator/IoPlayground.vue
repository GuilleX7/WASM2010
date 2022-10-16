<template>
  <div class="is-fullwidth is-fullheight asm--emulator-io-content">
    <b-field
      grouped
      label="Input / output"
    >
      <div class="is-flex is-fullwidth is-flex-direction-column">
        <template v-if="isThereIoComponents">
          <div
            v-for="(componentInstance, address) in componentsInstances"
            :key="address"
            class="mb-3"
          >
            <label>at {{ formatNumber(address, 16, 8, '0x') }}</label>
            <component
              :is="componentInstance.controllerComponentName"
              v-if="componentInstance.controllerComponentName"
              :controller="componentInstance.controllerInstance"
              :ui-clock-tick="uiClockTick"
            />
            <div v-else>
              ({{ componentInstance.id }})
            </div>
          </div>
        </template>
        <template v-else>
          <span>No mapped IO components yet.</span>
        </template>
      </div>
    </b-field>
  </div>
</template>

<script lang="ts">
import { csRegisterIoHandlers } from '@/asm2010/wrapper/io';
import { defineComponent, PropType } from '@vue/composition-api';
import {
  IoComponentId,
  IUiIoController,
  registeredIoComponents,
  TIoComponentInstance,
} from './io';
import HexDisplayController from '@/components/emulator/io/hex-display/HexDisplayController.vue';
import ButtonsController from '@/components/emulator/io/buttons/ButtonsController.vue';
import KeyboardController from '@/components/emulator/io/keyboard/KeyboardController.vue';
import { formatNumber } from '@/utils/format';

export default defineComponent({
  components: {
    HexDisplayController,
    ButtonsController,
    KeyboardController,
  },
  props: {
    mappedComponents: {
      type: Object as PropType<Record<number, IoComponentId>>,
      default: () => ({}),
    },
    uiClockTick: {
      type: Number,
      default: 0,
    },
  },
  computed: {
    componentsInstances(): Record<number, TIoComponentInstance> {
      return Object.entries(this.mappedComponents).reduce(
        (acc, [address, componentId]) => ({
          ...acc,
          [address]: {
            id: componentId,
            controllerComponentName:
              registeredIoComponents[componentId].controllerComponentName,
            controllerInstance: new registeredIoComponents[
              componentId
            ].controllerConstructor(),
          },
        }),
        {}
      );
    },
    componentsControllers(): Record<number, IUiIoController<unknown, unknown>> {
      return Object.entries(this.componentsInstances).reduce(
        (acc, [address, { controllerInstance }]) => ({
          ...acc,
          [address]: controllerInstance,
        }),
        {}
      );
    },
    isThereIoComponents(): boolean {
      return Boolean(Object.keys(this.componentsInstances).length)
    }
  },
  watch: {
    componentsControllers(
      value: Record<number, IUiIoController<unknown, unknown>>
    ) {
      csRegisterIoHandlers(value);
    },
  },
  mounted(): void {
    csRegisterIoHandlers(this.componentsControllers);
  },
  methods: {
    formatNumber,
  },
});
</script>

<style lang="scss">
.asm--emulator-io-content {
  overflow: auto;
  padding: 0.75rem;
}
</style>
