<template>
  <div class="is-fullwidth is-fullheight asm--emulator-io-content">
    <b-field grouped label="Input / output">
      <div class="is-flex is-align-items-center is-fullwidth is-flex-direction-column">
        <component
          v-for="(componentInstance, address) in componentsInstances"
          :key="address"
          :is="componentInstance.controllerComponentName"
          :controller="componentInstance.controllerInstance"
          :uiClockTick="uiClockTick"
        ></component>
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
  watch: {
    componentsControllers(
      value: Record<number, IUiIoController<unknown, unknown>>
    ) {
      csRegisterIoHandlers(value);
    },
  },
  components: {
    HexDisplayController,
    ButtonsController
  },
});
</script>
