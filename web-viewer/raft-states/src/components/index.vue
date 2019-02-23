<template>
  <div class="info">
    <h2>This Server</h2>
    <div>
      <p> Name: {{workers['Server']["serverName"]}}
        <br/>HTTP port: {{workers['Server']['serverHTTPPort']}}
        <br/>IP address: {{workers['Server']['serverip']}}
      </p>
      <button>get</button>
    </div>
    <h2>All Servers</h2>
    <div v-for="server in allservers" v-bind:key="server">
      <p> Name:{{server["serverName"]}}
        <br/>HTTP port:{{ server["serverHTTPPort"] }}
        <br/>IP address:{{ server['serverip'] }}
      </p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Servers',
  props: ['server'],
  data:
    function(){
    let workers = require('../../../../config/config.json');
    return {
      workers: workers,
      allservers:workers['AllServers']
    }
  },
  methods: {
    curl: function () {
      this.message = this.message.split('').reverse().join('')
  }
}
}

</script>

<style scoped>

</style>
