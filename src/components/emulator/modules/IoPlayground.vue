<template>
  <div class="is-fullwidth is-fullheight asm--emulator-io-content">
    <b-field grouped label="Input / output">
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
              {{ componentInstance.name }}
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
import ButtonsController from '@/components/emulator/modules/io/buttons/ButtonsController.vue';
import HexDisplayController from '@/components/emulator/modules/io/hex-display/HexDisplayController.vue';
import KeyboardController from '@/components/emulator/modules/io/keyboard/KeyboardController.vue';
import { getAsm2010Instance } from '@/core/ts';
import { formatNumber } from '@/utils/format';
import { PropType, defineComponent } from '@vue/composition-api';
import {
IUiIoController,
IoComponentId,
TIoComponentInstance,
registeredIoComponents,
} from './io';

export default defineComponent({
  components: {
    HexDisplayController,
    ButtonsController,
    KeyboardController,
  },
  props: {
    mappedComponents: {
      type: Object as PropType<Record<number, IoComponentId>>,
      default: () => ({} as Record<number, IoComponentId>),
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
            name: registeredIoComponents[componentId].name,
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
      return Boolean(Object.keys(this.componentsInstances).length);
    },
  },
  watch: {
    componentsControllers(
      value: Record<number, IUiIoController<unknown, unknown>>
    ) {
      getAsm2010Instance().csRegisterIoHandlers(value);
    },
  },
  mounted(): void {
    getAsm2010Instance().csRegisterIoHandlers(this.componentsControllers);
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
