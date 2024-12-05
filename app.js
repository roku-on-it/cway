/* This file is created to test the connection to the server. */

const net = require("net");

const client = net.createConnection({ port: 80, host: 'google.com'}, async () => {
  client.on("data", (data) => {
    console.log(data.toString());
  });

  console.log("connected to server!");
  client.write("GET / ");
});
