/*=========================================================================BACKEND=========================================================================*/
/*==WS==*/

//Constants
const port = "80";
const ws = new WebSocket("ws://localhost:"+port);
const initTime = (new Date()).getTime();
var connected = false;
const DEBUG = true;

//Camera
ws.binaryType = "arraybuffer";

//Listeners
ws.addEventListener("open", () => {
    if (DEBUG) console.log("We are connected!");
    ws.send(JSON.stringify({ 
        "type": "master-device", 
        "name": "GUI",
        "message": "trying to connect"}));
    connected = true;
});

ws.addEventListener("close", () => {
    if (DEBUG) console.log("Disconnected from server");
})

ws.addEventListener("message", (message) => {
    try { //If message can be parse, it's not binary
        data = JSON.parse(message.data);
        data_name = "ADS"; //data['Name'];
        if (DEBUG) console.log('data_name ' + data_name);
        switch (data_name) {

            case "MPU-6050": {
            document.getElementById("delta_S.x").innerHTML = data["delta_S.x"];
            document.getElementById("delta_S.y").innerHTML = data["delta_S.y"];
            document.getElementById("delta_S.z").innerHTML = data["delta_S.z"];

            document.getElementById("delta_R.x").innerHTML = data["delta_R.x"];
            document.getElementById("delta_R.y").innerHTML = data["delta_R.y"];
            document.getElementById("delta_R.z").innerHTML = data["delta_R.z"];
            break;}

            case "Sonar": {
            document.getElementById("Sonar_1").innerHTML = data["Sonar_1"];
            document.getElementById("Sonar_2").innerHTML = data["Sonar_2"];
            document.getElementById("Sonar_3").innerHTML = data["Sonar_3"];
            break;}

            case "ADS": {
            document.getElementById("Lumosity").innerHTML = data["Lumosity"];
            document.getElementById("Bat_Volt").innerHTML = data["Bat_Volt"];
            time_axis.push(((new Date()).getTime() - initTime)/1000);
            audio_data.push(data["Audio"]);
            if (time_axis.length > 500) {
                time_axis.shift();
                audio_Chart.options.scales.x.min += audio_data.at(-1);
                audio_Chart.options.scales.x.max += audio_data.at(-1);
            }
            if (audio_data.length > 500) audio_data.shift();
            audioChart.update();
            break;}

            default: break;
        }
    } catch(e) { //message must be binary
        //let TYPED_ARRAY = new Uint8Array(message.data);
        //const STRING_CHAR = String.fromCharCode.apply(null, new Uint8Array(message.data));
        //var img = document.getElementById('live');
        let base64String = btoa(String.fromCharCode.apply(null, new Uint8Array(message.data)));
        if (DEBUG) {
            console.log("Expecting datatype: binary");
            console.log(message.data);
            console.log("data:image/jpg;base64," + base64String);
        }
        document.getElementById('live').src = "data:image/jpg;base64," + base64String;
    }
});

//Server Requests
setInterval(function () {
    // ws.send(JSON.stringify({
    //     "type": "request",
    //     "Name": "MPU-6050"
    //     }));
    // ws.send(JSON.stringify({
    //     "type": "request",
    //     "Name": "Sonar"
    //     }));
    ws.send(JSON.stringify({
        "type": "request",
        "Name": "ADS"
        }));
    console.log("Sent a request")}, 1000);









/*=========================================================================FRONTEND=========================================================================*/

/*==Audio Graph==*/
time_axis = []; audio_data = [];
const audioData = {
    labels: time_axis,
    datasets: [{
        label: 'Audio',
        backgroundColor: 'rgb(255, 99, 132)',
        borderColor: 'rgb(255, 99, 132)',
        data: audio_data,
    }]
};

const audioConfig = {
    type: 'line',
    data: audioData,
    options: {
        animation: false,
        scales: {
            x:{
                max: 10, min: 0,
                ticks: {stepSize: 0.5},
                display: true
            },
            y: {
                max: 4096, min: 0,
                ticks: {stepSize: 1024},
                display: true
            }
        }
    }
}; audioChart = new Chart(document.getElementById('audioChart'), audioConfig);

/*==Lumosity Graph==*/
lumosity = [11];
const lumosityData = {
    labels: [""],
    datasets: [{
        label: 'Lumosity',
        data: lumosity,
        backgroundColor: [
            'rgba(38, 246, 38, 0.8)'
        ]
    }]
}

const lumosityConfig = {
    type: 'bar',
    data: lumosityData,
    options: {
        scales: {
            y: {
                max: 12, min: 0,
                ticks: {stepSize: 2},
                beginAtZero: true
            }
        }
    },
  };
batteryChart = new Chart(document.getElementById('lumosityChart'), lumosityConfig);

/*==Lumosity Graph==*/
battery = [11];
const batteryData = {
    labels: [""],
    datasets: [{
        label: 'Battery',
        data: battery,
        backgroundColor: [
            'rgba(235, 25, 88, 0.8)'
        ]
    }]
}

const batteryConfig = {
    type: 'bar',
    data: batteryData,
    options: {
        scales: {
            y: {
                display: true,
                max: 12, min: 0,
                ticks: {stepSize: 2},
                beginAtZero: true
            }
        }
    },
  };
batteryChart = new Chart(document.getElementById('batteryChart'), batteryConfig);
