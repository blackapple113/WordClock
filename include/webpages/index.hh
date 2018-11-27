const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>WordClock</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-colorpicker/2.5.3/css/bootstrap-colorpicker.min.css">
    <script src="https://code.jquery.com/jquery-3.3.1.js"></script>
    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-colorpicker/2.5.3/js/bootstrap-colorpicker.min.js"></script>
    <script>
        function sendData() {
            let cp = document.getElementById("colorpicker");
            colorData(cp.value);
        }

        function colorData(color) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if(this.readyState == 4 && this.status == 200) {
                    console.log("connection successfull");
                }
            }
            let colors = /rgb\((\d{1,3}),(\d{1,3}),(\d{1,3})\)/;
            let match = colors.exec(color);
            xhttp.open("GET", "setLED?LEDstate=COLOR&colorR="+match[1]+"&colorG="+match[2]+"&colorB="+match[3], true);
            xhttp.send();
        }

        function animation() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if(this.readyState == 4 && this.status == 200) {
                    document.getElementById("LED_rainbow").innerHTML = this.responseText;
                }
            }
            xhttp.open("GET", "setLED?LEDstate=ani&animation=rainbow");
            xhttp.send();
        }
        function startTime() {
            var today = new Date();
            var h = today.getHours();
            var m = today.getMinutes();
            var s = today.getSeconds();
            m = checkTime(m);
            s = checkTime(s);
            document.getElementById('clock').innerHTML =
            h + ":" + m + ":" + s;
            var t = setTimeout(startTime, 100);
        }
        function checkTime(i) {
            if(i < 10) {i = "0" + i};
            return i;
        }
    </script>
</head>
<body onload="startTime()">
    <nav class="navbar navbar-expand-md navbar-dark bg-dark">
        <span class="navbar-brand">WordClock</span>
    </nav>
    <main class="container" role="main">
        <h1 id="clock" class="text-center"></h1>
        <div class="starter-template">
            <h1>LED-Steuerung</h1>
            <h3>Farbe: </h3>
            <input id="colorpicker" type="text" class="form-control" onblur="sendData()" value="rgb(255, 127, 0)" />
            <br>
            <input type="button" class="btn btn-secondary" id="LED_button" onclick="colorData('rgb(0,0,0)')" value="OFF" />
            <input type="button" class="btn btn-secondary" id="LED_rainbow" onclick="animation()" value="rainbow" />
        </div>
    </main>
    <script>
        $(function() {
            $('#colorpicker').colorpicker({
                format: 'rgb'
            });
        });
    </script>
</body>
</html>
)=====";