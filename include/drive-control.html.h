#ifndef MARS_WIFI_UI
#define MARS_WIFI_UI

#include <pgmspace.h>

// For begining and end of the string
// INCLUDED_LITERAL_BEGIN R"rawliteral(
// INCLUDED_LITERAL_END   )rawliteral"

const char HTTP_MARS_WIFI_UI[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Rover Control</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #f4f4f4;
            font-family: Arial, sans-serif;
            margin: 0;
        }
        .dpad {
            display: grid;
            grid-template-columns: repeat(3, 80px);
            gap: 10px;
            justify-content: center;
            align-items: center;
        }
        .btn {
            width: 80px;
            height: 80px;
            font-size: 24px;
            font-weight: bold;
            background: #007BFF;
            color: white;
            border: none;
            border-radius: 15px;
            display: flex;
            justify-content: center;
            align-items: center;
            box-shadow: 2px 2px 10px rgba(0, 0, 0, 0.2);
            transition: background 0.2s, transform 0.1s;
        }
        .btn:active {
            background: #0056b3;
            transform: scale(0.95);
        }
    </style>
</head>
<body>
    <div class="dpad">
        <div></div>
        <button class="btn" onclick="sendCommand('w')" ontouchstart="sendCommand('w')">↑</button>
        <div></div>
        <button class="btn" onclick="sendCommand('a')" ontouchstart="sendCommand('a')">←</button>
        <button class="btn" onclick="sendCommand('c')" ontouchstart="sendCommand('c')">■</button>
        <button class="btn" onclick="sendCommand('d')" ontouchstart="sendCommand('d')">→</button>
        <div></div>
        <button class="btn" onclick="sendCommand('s')" ontouchstart="sendCommand('s')">↓</button>
        <div></div>
    </div>

    <script>
        function sendCommand(direction) {
            fetch(`/update`, { method: 'POST', headers: { 'Content-Type': 'text/plain' }, body: `${direction}` })
                .catch(error => console.error('Error:', error));
        }
    </script>
</body>
</html>
)rawliteral";

#endif