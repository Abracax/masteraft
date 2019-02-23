<template>
  <div id="app">
    <Servers/>
  </div>
</template>

<script>
import Servers from './components/index.vue'

export default {
  name: 'app',
  components: {
    Servers
  }
}
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: left;
  color: #2c3e50;
  margin-top: 10px;
}
</style>
