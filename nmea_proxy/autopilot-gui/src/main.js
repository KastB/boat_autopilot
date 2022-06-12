import Vue from 'vue'
import App from './App.vue'
import { BootstrapVue, IconsPlugin } from 'bootstrap-vue'
import { Row, Column, Hidden } from 'vue-grid-responsive';


// Import Bootstrap and BootstrapVue CSS files (order is important)
import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue/dist/bootstrap-vue.css'

// Make BootstrapVue available throughout your project
Vue.use(BootstrapVue)
// Optionally install the BootstrapVue icon components plugin
Vue.use(IconsPlugin)

Vue.config.productionTip = false

// eslint-disable-next-line 
Vue.component('row', Row);
// eslint-disable-next-line 
Vue.component('column', Column);
// eslint-disable-next-line 
Vue.component('hidden', Hidden);

// This imports all the layout components such as <b-container>, <b-row>, <b-col>:
import { LayoutPlugin } from 'bootstrap-vue'
Vue.use(LayoutPlugin)

// This imports <b-modal> as well as the v-b-modal directive as a plugin:
import { ModalPlugin } from 'bootstrap-vue'
Vue.use(ModalPlugin)

// This imports <b-card> along with all the <b-card-*> sub-components as a plugin:
import { CardPlugin } from 'bootstrap-vue'
Vue.use(CardPlugin)

// This imports directive v-b-scrollspy as a plugin:
import { VBScrollspyPlugin } from 'bootstrap-vue'
Vue.use(VBScrollspyPlugin)

// This imports the dropdown and table plugins
import { DropdownPlugin, TablePlugin } from 'bootstrap-vue'
Vue.use(DropdownPlugin)
Vue.use(TablePlugin)

import VueNativeSock from 'vue-native-websocket'
Vue.use(VueNativeSock, 'ws://10.10.10.1:8000', {
  reconnection: true, // (Boolean) whether to reconnect automatically (false)
  format: 'json',
  //reconnectionAttempts: 5, // (Number) number of reconnection attempts before giving up (Infinity),
  //reconnectionDelay: 3000, // (Number) how long to initially wait before attempting a new (1000)
})

new Vue({
  render: h => h(App),
}).$mount('#app')
