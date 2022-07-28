<template>
  <div class="ssd">
    <div class="ssd__tl" :class="{ 'seg--lit': value.tl }"></div>
    <div class="ssd__tm" :class="{ 'seg--lit': value.tm }"></div>
    <div class="ssd__tr" :class="{ 'seg--lit': value.tr }"></div>
    <div class="ssd__mm" :class="{ 'seg--lit': value.mm }"></div>
    <div class="ssd__bl" :class="{ 'seg--lit': value.bl }"></div>
    <div class="ssd__bm" :class="{ 'seg--lit': value.bm }"></div>
    <div class="ssd__br" :class="{ 'seg--lit': value.br }"></div>
  </div>
</template>

<style lang="scss">
/* Credits to Todd Moy
  See https://codepen.io/toddmoy/pen/pePPbO */

$segment-color-unlit: rgba(0, 0, 0, 0.1);
$segment-color-lit: #e74c3c;
$segment-width: 20px;
$segment-height: 4px;
$segment-spacing: 0px;

$container-width: $segment-width + (2 * $segment-height) +
  ($segment-spacing * 3);
$container-height: ($container-width * 2) - $segment-height;
$segment-half: ($segment-width + $segment-height) * 0.5;
$triangle-width: $segment-height * 0.5;

.ssd {
  width: $container-width;
  height: $container-height;
  position: relative;

  /** sides **/
  .ssd__tl,
  .ssd__tr,
  .ssd__bl,
  .ssd__br {
    transform: rotate(90deg);
  }

  /** left side **/
  .ssd__tl,
  .ssd__bl {
    left: $triangle-width + ($segment-half * -1);
  }

  //** right side **/
  .ssd__tr,
  .ssd__br {
    left: $segment-half + $triangle-width + (2 * $segment-spacing);
  }

  /** top sides **/
  .ssd__tl,
  .ssd__tr {
    top: $segment-half + $segment-spacing;
  }

  /** bottom sides **/
  .ssd__bl,
  .ssd__br {
    top: ($segment-half + $segment-spacing) * 3;
  }

  /** middles **/
  .ssd__tm,
  .ssd__mm,
  .ssd__bm {
    left: $triangle-width + $segment-spacing;
  }

  /** middle **/
  .ssd__mm {
    top: ($segment-half + $segment-spacing) * 2;
  }

  /** bottom middle **/
  .ssd__bm {
    top: ($segment-half + $segment-spacing) * 4;
  }

  [class*='ssd__'] {
    position: absolute;
    background-color: $segment-color-unlit;
    content: '';
    width: $segment-width;
    height: $segment-height;
    margin-left: $triangle-width;

    &:before,
    &:after {
      width: 0;
      height: 0;
      content: '';
      display: block;
      border-top: $segment-height * 0.5 solid transparent;
      border-bottom: $segment-height * 0.5 solid transparent;
    }

    &:before {
      margin-left: ($segment-height * 0.5) * -1;
      border-right: $segment-height * 0.5 solid $segment-color-unlit;
    }

    &:after {
      margin-top: $segment-height * -1;
      margin-left: $segment-width;
      border-left: $segment-height * 0.5 solid $segment-color-unlit;
    }
  }

  .seg--lit {
    background-color: $segment-color-lit;

    &:before {
      border-right-color: $segment-color-lit;
    }

    &:after {
      border-left-color: $segment-color-lit;
    }
  }
}
</style>

<script lang="ts">
import { defineComponent, PropType } from '@vue/composition-api';

export type TSSegDisplayValue = {
  tl?: boolean;
  tm?: boolean;
  tr?: boolean;
  mm?: boolean;
  bl?: boolean;
  bm?: boolean;
  br?: boolean;
};

export default defineComponent({
  props: {
    value: {
      type: Object as PropType<TSSegDisplayValue>,
      default: (): TSSegDisplayValue => ({
        bl: false,
        bm: false,
        br: false,
        mm: false,
        tl: false,
        tm: false,
        tr: false,
      }),
    },
  },
});
</script>
