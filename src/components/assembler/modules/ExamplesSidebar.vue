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
            :key="exampleFileName"
            icon="file-document"
            class="is-text-ellipsable"
            :label="exampleFileName"
            @click="$emit('load', exampleFileName, exampleFile)"
          />
        </b-menu-list>
      </b-menu>
    </div>
  </b-sidebar>
</template>

<script lang="ts">
import { CsPlatform } from '@/core/ts/types';
import { useGlobalStore } from '@/stores/global';
import { defineComponent } from '@vue/composition-api';
import { mapState } from 'pinia';

export default defineComponent({
  props: {
    open: {
      default: false,
      required: true,
      type: Boolean,
    },
  },
  data: () => {
    const cs2010ExampleFilesContext = require.context(
      '@/core/wasm/examples/asm/cs2010',
      true,
      /.asm$/
    );
    const cs3ExampleFilesContext = require.context(
      '@/core/wasm/examples/asm/cs3',
      true,
      /.asm$/
    );

    const [cs2010ExampleFiles, cs3ExampleFiles]: Record<string, string>[] = [
      cs2010ExampleFilesContext,
      cs3ExampleFilesContext,
    ].map((context) =>
      context.keys().reduce(
        (acc, fileName) => ({
          ...acc,
          [fileName.substring(2)]: context(fileName),
        }),
        {}
      )
    );

    return {
      cs2010ExampleFiles,
      cs3ExampleFiles,
    };
  },
  computed: {
    ...mapState(useGlobalStore, ['csPlatform']),
    exampleFiles(): Record<string, string> {
      switch (this.csPlatform) {
        case CsPlatform.Cs2010:
        default:
          return this.cs2010ExampleFiles;
        case CsPlatform.Cs3:
          return this.cs3ExampleFiles;
      }
    },
  },
});
</script>
