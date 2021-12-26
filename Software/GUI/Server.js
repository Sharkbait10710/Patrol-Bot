//Note: First, npm init -y to initialize package.json
//      Second, npm install express ws to acquire ws dependencies
//dependencies
const express = require('express');
const WebSocket = require('ws');
const SocketServer = require('ws').Server;

const server = express().listen(3000); //Listen on 80

const wss = new SocketServer({server});

//array of clients
var lookup = {};
const DEGUG = True;

wss.on('connection', (ws) => {
    console.log('[Server] A Client was connected.');

    ws.on('close', () => { console.log('[Server] Client disconnected')});

    ws.on('message', (message) => {
        if (DEBUG) console.log('[Server] Recieved message: %s', message);
        dataStr = "" + data;
        if (dataStr.search("->200<-") != -1) {
            ws.name = /Msg::.+::/
            ws.id = dataStr.substring(10, dataStr.length);
            lookup[ws.id] = ws;
            if (DEBUG) console.log("This is id: ");
            if (DEBUG) console.log(lookup[ws.id].id); 
        }
        //Sample usage of sending data to client
        // wss.clients.forEach(function each(client) {
        //     if (client !== ws && client.readyState === WebSocket.OPEN) {
        //         client.send(message);
        //     }
        // })
    })

    ws.on("close", () => {
        //remove id from client array
        delete lookup[ws.id];
        if (DEBUG) console.log("Client " + ws.id + " has been disconnected");
    });
})
