import App from '@/components/App.vue';
import Buefy from 'buefy';
import Vue from 'vue';
import { PiniaVuePlugin, createPinia } from 'pinia';
import 'buefy/dist/buefy.css';
import '@/assets/styles/general.scss';

Vue.use(Buefy);
Vue.use(PiniaVuePlugin);

new Vue({
  el: '#root',
  render: (h) => h(App),
  pinia: createPinia(),
});
