import Vue from 'vue';
import Buefy from 'buefy';
import 'buefy/dist/buefy.css'
import '@/app.scss'
import App from '@/App.vue';

Vue.use(Buefy);
new Vue({
  el: '#root',
  render: (h) => h(App),
});
