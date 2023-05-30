declare module '*.vue' {
  import { VueConstructor } from 'vue';
  const component: VueConstructor
  export default component;
}

declare module '*.wasm' {
  const url: string;
  export default url;
}

declare module 'vue-multipane' {
  import { VueConstructor } from 'vue/types/umd';
  const Multipane: VueConstructor;
  const MultipaneResizer: VueConstructor;
  export { Multipane, MultipaneResizer };
}

declare module 'buefy' {
  import { PluginObject } from 'vue/types/plugin';
  const plugin: PluginObject<unknown>;
  export default plugin;
}
