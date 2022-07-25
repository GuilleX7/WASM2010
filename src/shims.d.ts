declare module '*.vue' {
  import type { DefineComponent, VueConstructor, VueConstructor } from 'vue';
  const component: DefineComponent<{}, {}, any>;
  export default component;
}

declare module '*.wasm' {
  const url: string;
  export default url;
}

declare namespace JSX {
  interface IntrinsicElements {
    [tagName: string]: any;
  }
}

declare module 'vue-multipane' {
  const Multipane: VueConstructor;
  const MultipaneResizer: VueConstructor;
  export { Multipane, MultipaneResizer };
}
