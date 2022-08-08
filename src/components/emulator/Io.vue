<template>
  <div class="is-fullwidth is-fullheight asm--emulator-io-content">
    <b-field grouped label="Input / output">
      <div class="is-flex is-fullwidth is-flex-direction-column">
        <div
          v-for="(componentInstance, address) in componentsInstances"
          :key="address"
          class="mb-3"
        >
          <label>at {{ formatNumber(address, 16, 8, '0x') }}</label>
          <component
            v-if="componentInstance.controllerComponentName"
            :is="componentInstance.controllerComponentName"
            :controller="componentInstance.controllerInstance"
            :uiClockTick="uiClockTick"
          ></component>
          <div v-else>({{ componentInstance.id }})</div>
        </div>
      </div>
    </b-field>
  </div>
</template>

<style lang="scss">
.asm--emulator-io-content {
  overflow: auto;
  padding: 0.75rem;
}
</style>

<script lang="ts">
import { IUiIoController } from '@/types';
import { csRegisterIoHandlers } from '@/wasm/asm2010/io';
import { defineComponent, PropType } from '@vue/composition-api';
import {
  IoComponentId,
  registeredIoComponents,
  TIoComponentInstance,
} from './io';
import HexDisplayController from './io/hex-display/HexDisplayController.vue';
import ButtonsController from './io/buttons/ButtonsController.vue';
import KeyboardController from './io/keyboard/KeyboardController.vue';
import { formatNumber } from '@/utils/format';

export default defineComponent({
  props: {
    mappedComponents: {
      type: Object as PropType<Record<number, IoComponentId>>,
      default: () => ({}),
    },
    uiClockTick: {
      type: Number,
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
  },
  mounted(): void {
    csRegisterIoHandlers(this.componentsControllers);
  },
  methods: {
    formatNumber,
  },
  watch: {
    componentsControllers(
      value: Record<number, IUiIoController<unknown, unknown>>
    ) {
      csRegisterIoHandlers(value);
    },
  },
  components: {
    HexDisplayController,
    ButtonsController,
    KeyboardController,
  },
});
</script>
