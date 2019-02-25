<template>
  <div class="info">
    <h2>This Server</h2>
    <div>
      <p> Name: {{workers['Server']["serverName"]}}
        <br/>HTTP port: {{workers['Server']['serverHTTPPort']}}
        <br/>IP address: {{workers['Server']['serverip']}}
      </p>
      <div class="myterm" ref="myterm">Term:</div>
      <div class="myrole" ref="myrole">Role:</div>
      <button v-on:click="sendReq(workers['Server']['serverip'],workers['Server']['serverHTTPPort'])">get</button>
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
let workers = require('../../../../config/config.json');
export default {
  name: 'Servers',
  props: ['server'],
  data:
    function(){
    return {
      workers: workers,
      allservers:workers['AllServers']
    }
  },
  methods: {
    sendReq: function (ip,port) {
      const http = require('http');
      let options = {
        "method": "GET",
        "hostname": ip,
        "port": port.toString(),
      };
      let data;
      var req = http.request(options, function (res) {
      var chunks = [];
      res.on("data", function (chunk) {
        chunks.push(chunk);
      });
      res.on("end", function () {
        data = Buffer.concat(chunks);
      });
      });
      req.end();
      let res = JSON.parse(data);
      console.log(res['term']);
      this.$refs.myterm.innerText=`Term : {res['term']}`;
      this.$refs.myrole.innerText= `Role : {res['role']}`;
      return {
        role:res['role'],
        term:res['term']
      }
  }
}
}

</script>

<style scoped>
p{
  font-size:16px;
}
</style>
