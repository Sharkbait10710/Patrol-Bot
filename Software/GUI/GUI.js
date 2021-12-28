const port = "3000";
const ws = new WebSocket("ws://localhost:"+port);

const initTime = (new Date()).getTime();

ws.addEventListener("open", () => {
    console.log("We are connected!");
    ws.send(JSON.stringify({ 
        "type": "master-device", 
        "name": "GUI",
        "message": "trying to connect"}));
});

ws.addEventListener("message", (message) => {
    //if (false) console.log('[Server] Recieved message: %s', message), document.getElementById('live');
    console.log("[SERVER] " + message.data);
    try {
        var data, data_name;
        data = JSON.parse(message.data);
        data_name = data['name'];
        console.log('data_name ' + data_name)
        switch (data_name) {
            case "MPU-6050": {
            document.getElementById("delta_S.x").innerHTML = data["delta_S.x"];
            document.getElementById("delta_S.y").innerHTML = data["delta_S.y"];
            document.getElementById("delta_S.z").innerHTML = data["delta_S.z"];

            document.getElementById("delta_R.x").innerHTML = data["delta_R.x"];
            document.getElementById("delta_R.y").innerHTML = data["delta_R.y"];
            document.getElementById("delta_R.z").innerHTML = data["delta_R.z"];
            break;}

            case "Sonar"   : {
            document.getElementById("Sonar_1").innerHTML = data["Sonar_1"];
            document.getElementById("Sonar_2").innerHTML = data["Sonar_2"];
            document.getElementById("Sonar_3").innerHTML = data["Sonar_3"];
            break;}

            case "ADS"     : {
            document.getElementById("Lumosity").innerHTML = data["Lumosity"];
            document.getElementById("Bat_Volt").innerHTML = data["Bat_Volt"];
            document.getElementById("Audio").innerHTML = data["Audio"];
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
    }
    catch (e) {
        try {
            var bytes = new Uint8Array(message);
            if (bytes) {
                var binary= '';
                var img = document.getElementById('live');
                var len = bytes.byteLength;
                for (var i = 0; i < len; i++)
                    binary += String.fromCharCode(bytes[i]);
                img.src = 'data:image/jpg;base64,'+window.btoa(binary);
            }
        }
        catch (e) {console.log(e);}
    }
    try {data_name = data["name"];}
    catch {data_name = "N/A";}
});

ws.addEventListener("close", () => {
    console.log("Disconnected from server");
})

var time_axis = [];
var audio_data = [];

const data = {
labels: time_axis,
datasets: [{
    label: 'Audio',
    backgroundColor: 'rgb(255, 99, 132)',
    borderColor: 'rgb(255, 99, 132)',
    data: audio_data,
    }]
};

const config = {
    type: 'line',
    data: data,
    options: {
        animation: false,
        scales: {
            x:{
                max: 10,
                min: 0,
                ticks: {
                    stepSize: 0.5
                },
                display: true
            },
            y: {
                max: 4096,
                min: 0,
                ticks: {
                    stepSize: 1024
                },
                display: true
            }
        }
    }
};


audioChart = new Chart(
document.getElementById('audioChart'),
config
);

