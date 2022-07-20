import { createPinia, PiniaVuePlugin } from 'pinia';
import Vue from 'vue';
import Buefy from 'buefy';
import 'buefy/dist/buefy.css'
import '@/app.scss'
import App from '@/App.vue';

Vue.use(PiniaVuePlugin);
Vue.use(Buefy);
new Vue({
  el: '#root',
  pinia: createPinia(),
  render: (h) => h(App),
});
