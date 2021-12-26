//Note: First, npm init -y to initialize package.json
//      Second, npm install express ws to acquire ws dependencies
//dependencies
const express = require('express');
const WebSocket = require('ws');
const SocketServer = require('ws').Server;
const path = require('path');

const PORT = process.env.PORT || 3000;
const INDEX = path.join(__dirname, 'index.html');

const server = express()
    .use((req, res) => res.sendFile(INDEX))
    .listen(PORT, () => console.log(`Listening on ${ PORT }`));

const wss = new SocketServer({server});

//array of clients
var lookup = {};
const DEBUG = true;

//Data
const MPU   = [0, 0, 0, 0, 0, 0];
const ADS   = [0, 0, 20        ];
const Sonar = [0, 0, 0         ];

wss.on('connection', (ws) => {
    ws.id=Date.now()%10000000000;
    lookup[ws.id]=ws;
    console.log('[Server] A Client ' + ws.id + ' was connected.');

    ws.on('close', () => { 
        console.log('[Server] Client' + ws.id +  'disconnected');
        delete lookup[ws.id];
    })

    ws.on('message', (message) => {
        if (DEBUG) console.log('[Server] Recieved message: %s', message);
        let data = JSON.parse(message);
        let data_name = data["name"];
        switch (data["type"]) {
            case "master-device": {ws.name=data_name; break;}
            case "sensor": {
                switch (data_name) {
                    case "MPU-6050": {
                    MPU[0] = data["delta_S.x"];
                    MPU[1] = data["delta_S.y"];
                    MPU[2] = data["delta_S.z"];
                    
                    MPU[3] = data["delta_R.x"];
                    MPU[4] = data["delta_R.y"];
                    MPU[5] = data["delta_R.z"];
                    break;}

                    case "Sonar"   : {
                    Sonar[0] = data["Sonar_1"];
                    Sonar[1] = data["Sonar_2"];
                    Sonar[2] = data["Sonar_3"];
                    break;}

                    case "ADS"     : {
                    ADS[0] = data["Lumosity"];
                    ADS[1] = data["Bat_Volt"];
                    ADS[2] = data["Audio"   ];
                    break;}

                    default: break;
                }
            }
            
            case "request": {
                switch (data_name) {
                    case "MPU-6050": {
                    ws.send(JSON.stringify({
                        "From"     :   "Server",
                        "Type"     :   "Sensor",
                        "Name"     : "MPU-6050",
                        "delta_S.x":     MPU[0],
                        "delta_S.y":     MPU[1],
                        "delta_S.z":     MPU[2],

                        "delta_R.x":     MPU[3],
                        "delta_R.y":     MPU[4],
                        "delta_R.z":     MPU[5]
                    }));
                    break;}

                    case "ADS": {
                        ws.send(JSON.stringify({
                            "From"     :   "Server",
                            "Type"     :   "Sensor",
                            "Name"     : "ADS",
                            "Lumosity":      ADS[0],
                            "Bat_volt":      ADS[1],
                            "Temperature":   ADS[2]
                        })); 
                    break;}

                    case "Sonar": {
                        ws.send(JSON.stringify({
                            "From"     :   "Server",
                            "Type"     :   "Sensor",
                            "Name"     :    "Sonar",
                            "Sonar-1":      Sonar[0],
                            "Sonar-2":      Sonar[1],
                            "Sonar-3":      Sonar[2]
                        })); 
                    break;}
                }
            }

            default: break;
        }
    })
})
