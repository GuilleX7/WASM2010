<template>
  <b-sidebar
    type="is-light"
    :fullheight="true"
    :overlay="false"
    :open="open"
    @close="$emit('close')"
  >
    <div class="px-5 py-5">
      <b-menu :activable="false">
        <b-menu-list label="Examples">
          <b-menu-item
            v-for="(exampleFile, exampleFileName) in exampleFiles"
            icon="file-document"
            class="is-text-ellipsable"
            :label="exampleFileName"
            :key="exampleFileName"
            @click="$emit('load', exampleFileName, exampleFile)"
          ></b-menu-item>
        </b-menu-list>
      </b-menu>
    </div>
  </b-sidebar>
</template>

<script lang="ts">
import { defineComponent } from '@vue/composition-api';

export default defineComponent({
  props: {
    open: {
      default: false,
      required: true,
      type: Boolean,
    },
  },
  data: () => {
    const exampleFilesContext = require.context(
      '@/wasm/asm2010/examples',
      true,
      /.asm$/
    );
    const exampleFiles: Record<string, string> = exampleFilesContext
      .keys()
      .reduce(
        (acc, fileName) => ({
          ...acc,
          [fileName.substring(2)]: exampleFilesContext(fileName),
        }),
        {}
      );
    return {
      exampleFiles,
    };
  },
});
</script>
