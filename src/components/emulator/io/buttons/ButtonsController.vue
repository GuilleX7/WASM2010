<template>
  <div class="is-flex is-flex-direction-column">
    <b-field
      grouped
      expanded
      class="mr-0"
    >
      <b-button
        expanded
        @mousedown="onButtonDown(1)"
        @mouseup="onButtonUp(1)"
      >
        A
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(2)"
        @mouseup="onButtonUp(2)"
      >
        B
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(4)"
        @mouseup="onButtonUp(4)"
      >
        C
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(8)"
        @mouseup="onButtonUp(8)"
      >
        D
      </b-button>
    </b-field>
    <b-field
      grouped
      expanded
    >
      <b-button
        expanded
        @mousedown="onButtonDown(16)"
        @mouseup="onButtonUp(16)"
      >
        E
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(32)"
        @mouseup="onButtonUp(32)"
      >
        F
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(64)"
        @mouseup="onButtonUp(64)"
      >
        G
      </b-button>
      <b-button
        expanded
        @mousedown="onButtonDown(128)"
        @mouseup="onButtonUp(128)"
      >
        H
      </b-button>
    </b-field>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';
import { TUiButtonsEventName, UiButtonsController } from './ButtonsController';

export default defineComponent({
  props: {
    controller: {
      required: true,
      type: Object as PropType<UiButtonsController>,
    },
    uiClockTick: {
      required: true,
      type: Number,
    },
  },
  methods: {
    onButtonDown(buttonIdx: number): void {
      this.controller.uiUpdateState({
        type: TUiButtonsEventName.Down,
        buttonIdx,
      });
    },
    onButtonUp(buttonIdx: number): void {
      this.controller.uiUpdateState({
        type: TUiButtonsEventName.Up,
        buttonIdx,
      });
    },
  },
});
</script>
