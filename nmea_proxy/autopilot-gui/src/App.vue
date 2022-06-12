<template>

  <div class="autopilot_main"> 
    
    <b-container class="autopilot_main">
        
  <b-row class="mb-2">
      <b-col >  
          <b-button  class="w-100" size="lg"  @click="b_init" :disabled=b_disabled>Initialisieren</b-button>
      </b-col>
      <b-col >  
        <b-button class="w-100" size="lg" @click="b_park" :disabled=b_disabled>Parken</b-button>
      </b-col>  
    </b-row>

    <b-row class="mb-2">
      <b-col >  
          <b-button class="w-100" size="lg" @click="b_re_init" :disabled=b_disabled>
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Re-init&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>

      <b-col >  
          <b-button  class="w-100" size="lg" @click="b_unlock">
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Entsperren&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Ziel[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_goal}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Fehler[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_lasterror}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Wind[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data["m_wind.apparentAngle"]}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Wind[kn]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.tws}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Kurs[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.yaw}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Fahrt[kn]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.gps_vel}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Temp[°C]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data["m_speed.waterTemp"]}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Tiefe[m]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data["m_depth.depthBelowTransductor"]}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Ruder[tiks]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.CurrentPosition}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Integral[°s]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_errorSum}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Krängung[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.roll}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Pitch[°]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.pitch}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">Spannung[V]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_voltage}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Strom[A]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_amperage}}</h1></b-col>
    </b-row>


    <b-row class="mb-2">
      <b-col ><h1 style="font-size: 2em;">VMG[kn]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.vmg}}</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">Leistung[W]:</h1></b-col>
      <b-col ><h1 style="font-size: 2em;">{{data.m_power}}</h1></b-col>
    </b-row>

    <b-row class="mb-2">
      <b-col>  
          <b-button class="w-100" size="lg" @click="b_pos">
                  &nbsp;&nbsp;&nbsp;&nbsp;Position&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>

      <b-col>  
        <b-button class="w-100" size="lg" @click="b_komp">
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Kompass&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        </b-button>
      </b-col>
      <b-col>  
          <b-button class="w-100" size="lg" @click="b_wind">
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Wind&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>
    </b-row>


    <b-row class="mb-2">
      <b-col>
          <b-button class="w-100" size="lg"  @click="b_m10">
                  &nbsp;&nbsp;&nbsp;&nbsp;-10&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>

      <b-col>  
        <b-button class="w-100" size="lg" @click="b_m1">
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        </b-button>
      </b-col>
      <b-col>  
          <b-button class="w-100" size="lg" @click="b_p1">
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>
      <b-col>  
          <b-button class="w-100" size="lg" @click="b_p10">
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;+10&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          </b-button>
      </b-col>
</b-row>

</b-container>
</div>
</template>

<style>
.autopilot_main {
  transform: rotate(0deg);
  background: "red"
}
</style>

<script>

export default {
  name: 'App',
  components: {
  },
  data() {
    return {
      b_disabled: true,
      b_timeout: null,
      data: {}
    }
  },
  //this.$options.sockets.onmessage = (data) => console.log(data)
  mounted: function () {
    this.$options.sockets.onmessage = data => {
        data.data.text().then(dat =>{
            const obj = JSON.parse(dat);
            console.log(obj)
            this.data = obj
          })
      }
    },
  methods: {
    b_init() {
      this.$socket.sendObj({cmds: ["I"]})
    },
    b_park() {
      this.$socket.sendObj({cmds: ["GP"]})
    },
    b_re_init() {
      this.$socket.sendObj({cmds: ["RI"]})
    },
    b_pos() {
      this.$socket.sendObj({cmds: ["P"]})
    },
    b_komp() {
      this.$socket.sendObj({cmds: ["K"]})
    },
    b_wind() {
      this.$socket.sendObj({cmds: ["W"]})
    },
    b_m10() {
      this.$socket.sendObj({cmds: ["-10"]})
    },
    b_m1() {
      this.$socket.sendObj({cmds: ["-1"]})
    },
    b_p1() {
      this.$socket.sendObj({cmds: ["+1"]})
    },
    b_p10() {
      this.$socket.sendObj({cmds: ["+10"]})
    },
    b_unlock() {
      this.b_disabled = false
      this.b_timeout = setTimeout(() => {
                            this.b_disabled = true;
                        }, 5000);
    }
  }
}

</script>

