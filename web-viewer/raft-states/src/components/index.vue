<template>
  <div class="info">
    <h2>This Server</h2>
    <div>
      <p> Name: {{workers['Server']["serverName"]}}
        <br/>HTTP port: {{workers['Server']['serverHTTPPort']}}
        <br/>IP address: {{workers['Server']['serverip']}}
      </p>
      <div>
      <p class="myterm" >Term:{{workers['Server']['term']}}<br/>Role:{{workers['Server']['role']}}</p>
      <button v-on:click="sendReq(workers['Server'])">get</button>
      </div>
    </div>
    <h2>All Servers</h2>
    <div v-for="server in allservers" v-bind:key="server">
      <p> Name:{{server["serverName"]}}
        <br/>HTTP port:{{ server["serverHTTPPort"] }}
        <br/>IP address:{{ server['serverip'] }}
      </p>
      <div>
      <p class="myterm" >Term:{{server['term']}}<br/>Role:{{server['role']}}</p>
      <button v-on:click="sendReq(server)">get</button>
      </div>
    </div>
  </div>
</template>

<script>
let workers = require('../../../../config/config.json');
let role,term;
export default {
  name: 'Servers',
  props: ['server'],
  data:
    function(){
    return {
      workers: workers,
      allservers:workers['AllServers'],
      //Term:term,
      //Role:role
    }
  },
  methods: {
    sendReq: function (server) {
      this.$forceUpdate();
      const http = require('http');
      let options = {
        "method": "GET",
        "hostname": server['serverip'],
        "port": server["serverHTTPPort"].toString(),
      };
      let sendData;
      var req = http.request(options, function (res) {
        var chunks = [];
        res.on("data", function (chunk) {
          chunks.push(chunk);
        });
        res.on('error',function(error){
          return;
        });
        res.on("end", function () {
          sendData = Buffer.concat(chunks);
          let ret = JSON.parse(sendData);
          //console.log(ret);
          //console.log(`Term:${ret['term']}`);
          //console.log(`Role:${ret['role']}`);
          term = ret['term'];
          role = ret['role'];
          server['role'] = ret['role'];
          server['term'] = ret['term'];
        });
      });
      this.$forceUpdate();
      //console.log(server);     
      //this.Role = role;
      //this.Term = term;
      req.end();
      return server;
  }
}
}

</script>

<style scoped>
p{
  font-size:10px;
}
</style>
