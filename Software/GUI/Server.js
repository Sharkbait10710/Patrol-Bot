//Note: First, npm init -y to initialize package.json
//      Second, npm install express ws to acquire ws dependencies
//dependencies
const express = require('express');
const WebSocket = require('ws');
const SocketServer = require('ws').Server;
const path = require('path');

const PORT = process.env.PORT || 80;

const server = express()
    .listen(PORT, () => console.log(`Listening on ${ PORT }`));

const wss = new SocketServer({server});

DEBUG = false;
//array of clients
var lookup = {};
var names = {};

//Data
var MPU   = [0, 0, 0, 0, 0, 0];
var ADS   = [0, 0, 20];
var Sonar = [0, 0, 0];
var temperature = 20;

wss.on('connection', (ws) => {
    ws.id = Math.random().toString(36).substr(2, 9);
    lookup[ws.id]=ws;
    console.log('[Server] A Client ' + ws.id + ' was connected.');

    ws.on('close', () => { 
        console.log('[Server] Client' + ws.id +  'disconnected');
        delete lookup[ws.id];
    })

    ws.on('message', (message) => {
        try {
            data = JSON.parse(message);
            console.log("[CLIENT] Message:" + message);
            data = JSON.parse(message);
            data_name = data['Name'];
            switch (data["type"]) {
                case "master-device": {
                    ws.name=data_name;
                    names[ws.name] = ws.id; 
                    if (DEBUG) {
                        console.log("data_name: " + data_name);
                        console.log("ws.name: " + ws.name);
                        console.log("names[ws.name]: " + names[ws.name]);
                    }
                    if (data_name == 'GUI') console.log('GUI detected');
                    break;
                }

                case "redirect": {
                    try {
                        if (DEBUG) {
                            console.log(data['message']);
                            console.log("? "+data["recipient"]);
                            console.log("?? "+names[data["recipient"]]);
                        }
                        lookup[names[data["recipient"]]].send(JSON.stringify(data['message']));
                    } catch (e) {console.log(e); console.log("Recipient does not exist");}
                    break;
                }

                case "sensor": {
                    
                    switch (data_name) {
                        case "MPU-6050": {
                        MPU[0] = data["delta_S.x"];
                        MPU[1] = data["delta_S.y"];
                        MPU[2] = data["delta_S.z"];
                        
                        MPU[3] = data["delta_R.x"];
                        MPU[4] = data["delta_R.y"];
                        MPU[5] = data["delta_R.z"];

                        temperature = data["temp"];
                        break;}

                        case "Sonar"   : {
                        Sonar[0] = data["Sonar_1"];
                        Sonar[1] = data["Sonar_2"];
                        Sonar[2] = data["Sonar_3"];
                        break;}

                        case "ADS"     : {
                        ADS[0] = data["Lumosity"];
                        ADS[1] = data["Bat_Volt"];
                        ADS[2] = data["Audio"];
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
                            "client_name": "GUI",
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
                                "Name"     :      "ADS",
                                "client_name": "GUI",
                                "Lumosity":      ADS[0],
                                "Bat_Volt":      ADS[1],
                                "Audio"   :      ADS[2]
                            })); 
                        break;}

                        case "Sonar": {
                            ws.send(JSON.stringify({
                                "From"     :   "Server",
                                "Type"     :   "Sensor",
                                "Name"     :    "Sonar",
                                "client_name": "GUI",
                                "Sonar_1":      Sonar[0],
                                "Sonar_2":      Sonar[1],
                                "Sonar_3":      Sonar[2]
                            })); 
                        break;}

                        case "Temperature": {
                            ws.send(JSON.stringify({
                                "From"     :   "Server",
                                "Type"     :   "Sensor",
                                "Name"     :    "Temperature",
                                "client_name": "GUI",
                                "Temp":      temperature
                            })); 
                        break;}

                        default: break;
                    }
                }

                default: break;
            }
        }
        catch (e) {
            console.log("Caught a non-JSON format");
            wss.clients.forEach(function each(client) {
                if (names[client.id] == 'GUI') {
                    client.send(message);
                    console.log('SENT');
                }
            });
        }
        
    })
})