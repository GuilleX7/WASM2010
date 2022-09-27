declare module '*.vue' {
  import type { DefineComponent } from 'vue';
  const component: DefineComponent<unknown, unknown, unknown>;
  export default component;
}

declare module '*.wasm' {
  const url: string;
  export default url;
}

declare namespace JSX {
  interface IntrinsicElements {
    [tagName: string]: unknown;
  }
}

declare module 'vue-multipane' {
  const Multipane: VueConstructor;
  const MultipaneResizer: VueConstructor;
  export { Multipane, MultipaneResizer };
}

declare module 'buefy' {
  export default {} as PluginObject
}
