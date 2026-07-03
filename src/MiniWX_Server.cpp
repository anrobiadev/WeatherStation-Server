#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <vector>

#include <NTPtimeESP.h>
#include <stdlib.h>
#include "FS.h"


const char PAGE_Main_EN[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta lang='en' http-equiv='Content-Type' content='text/html' charset='UTF-8'>\
<title>MiniWX Weather Server</title>\
<style>\
  body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #FFB000; font-size: 32px;}\
  table { display: table; border: 1px solid #FFB000; border-collapse: separate; border-spacing: 1px; border-color: #FFB000;}\
  th, td {border: 1px solid #FFB000; border-collapse: separate; text-align: center; font-size: 28px;}\
  th {border-left: 30px solid #FFB000; border-right: 30px solid #FFB000; background: #FFB000; Color: black; font-size: 32px;}\
  fieldset { border:1px solid #999; border-radius:8px; box-shadow:0 0 10px #999;}\
  legend  { background:#000;}\
  .divTable { width: 100%; display:block; padding-top:10px; padding-bottom:10px; padding-right:10px; padding-left:10px;}\
  .divRow { width: 99%; display:block; padding-bottom:5px;}\
  .divColumn { float: left; width: 24%; display:block; padding-left:8px; }\
  .tabheader { border-style: solid; border-radius:8px; border-color: grey; box-shadow:0 0 8px #999; padding-left:10px;}\
  .notabheader { border-style: solid; border-radius:8px; border-color: black; box-shadow:0 0 8px #999; padding-left:10px; }\
  button { padding: 1px 6px 1px 6px;}\
  button > img,button > span { vertical-align: middle;}\
  button > text,button > span { vertical-align: middle;}\
</style>\
<style>\
.butn {\
  -webkit-border-radius: 10;\
  -moz-border-radius: 10;\
  border-radius: 10px;\
  text-shadow: 1px 1px 1px #666666;\
  -webkit-box-shadow: 0px 1px 3px #666666;\
  -moz-box-shadow: 0px 1px 3px #666666;\
  box-shadow: 0px 1px 1px #666666;\
  font-family: Arial;\
  color: #000;\
  font-size: 16px;\
  background: lightgray;\
  padding: 1px 6px 1px 6px;\
  text-decoration: none;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
}\
\
.butn:hover {\
  background: #FFB000;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
  text-decoration: none;\
}\
.butn:disabled {\
  background: gray;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
  text-decoration: none;\
}\
</style>\
<style>\
.grid-container {\
  display: grid;\
  grid-template-columns: auto;\
  grid-template-rows: auto auto;\
  grid-column-gap: 1px;\
  grid-row-gap: 6px;\
}\
.item1 {\
  grid-row: 1;\
}\
.item2 {\
  grid-row: 2;\
}\
</style>\
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>\
<script>\
setInterval(function() { getData();}, 1000);\
function getData() {\
  var xhttp = new XMLHttpRequest();\
  var reply = new Array;\
  xhttp.onreadystatechange = function() {\
     if (this.readyState == 4 && this.status == 200){\
      reply = this.responseText.split(',');\
      document.getElementById('clock').innerHTML = reply[0];\
      document.getElementById('temp').innerHTML = reply[1];\
      document.getElementById('pres').innerHTML = reply[2];\
      document.getElementById('rhum').innerHTML = reply[3];\
      document.getElementById('dewp').innerHTML = reply[4];\
      document.getElementById('hind').innerHTML = reply[5];\
      document.getElementById('nexttx').innerHTML = reply[6];\
      document.getElementById('rssi').innerHTML = reply[7];\
      document.getElementById('uptime').innerHTML = reply[8];\
    }\
  };\
  xhttp.open('GET', 'jquery', true);\
  xhttp.send();\
}\
</script>\
</head>\
  <body>\
    <hr>\
    <h3 style='text-shadow: 2px 1px grey;'>{{callsign}} Weather Server - {{SOFT_VER}} - <a id='clock' class='tabheader' style='padding-right:10px'>{{time}}</a></h3>\
    <h4>\
    <fieldset style='width:{{fieldsize0}}'>\
    <legend style='text-shadow: 2px 1px grey;'> {{ChipModel}} </legend>\
    <div class='divTable'>\
      <div class='divRow'>\
        <div class='divColumn' style='width:{{fieldsize1}}'>\
          <div class='tabheader'>Parameter</div>\
          <div class='notabheader'>Temperature</div>\
          <div class='notabheader'>Pressure</div>\
          <div class='notabheader'>Relative Humidity</div>\
          <div class='notabheader'>Dew point</div>\
          <div class='notabheader'>Heat Index</div>\
        </div>\
        <div class='divColumn' style='width:{{fieldsize2}}'>\
          <div class='tabheader'>Value</div>\
          <div id='temp' class='notabheader' style='color: white;'>{{degC}}</div>\
          <div id='pres' class='notabheader' style='color: white;'>{{mbar}}</div>\
          <div id='rhum' class='notabheader' style='color: white;'>{{rHum}}</div>\
          <div id='dewp' class='notabheader' style='color: white;'>{{DPdegC}}</div>\
          <div id='hind' class='notabheader' style='color: white;'>{{HIdegC}}</div>\
        </div>\
        <div class='divColumn' style='width:{{fieldsize3}}'>\
          <div class='tabheader'>Unit</div>\
          <div class='notabheader'>ºC</div>\
          <div class='notabheader'>mBar</div>\
          <div class='notabheader'>%</div>\
          <div class='notabheader'>ºC</div>\
          <div class='notabheader'>ºC</div>\
        </div>\
      </div>\
    </div>\
    <div></br></br></br></br></br></br></br></div>\
    <form action='/graphs' method='POST'>\
      <button class='butn' name='Trend' type='graphs' value='true'><img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAWCAYAAAA1vze2AAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAIGNIUk0AAIcbAACL/wAA/bEAAINJAAB9LgAA7W0AADg+AAAkPqJQ9KgAAATYSURBVHjapJRbTFN3HMd/yV63bA+LwS173esefZoxM0RgWEScF5QCpVwKp1DL6ek55/9v6Y0iK5cKoqAbGGzP6QWdIk4XXZxu04wtm0C5WUAW58NWjZm6LcuWffdQM2XOmOHD5+mffz753b60bt0rVF36dkGnbP2xRWy8631umu56pea7HVK9HhDKKCSWEYll+eSqyPPz9g+gxuahRidXBYtOgkWnoEYmIbs7EKwzIGDZ0dPlrHyR5N355DUVulxdx8ASi+D6zKpg+gxYYhFSexSxQC5uXd2Bk+EihBVLC3XLFmJ7cl2urmGwxAK4Pr0qmJYCSyxB9oTxlVYA3DHh2qkieGt3xenXBw+ow7a7RX1/8CmS1EOeIdGnocbmwUUV/XYDxvq3oNdReP+bS+cL6Jd7P1PYXu55UpIC02fAEwvgiTR4bAZcSz2liimwxAIcYR3DzXZ85GyHrWTD/S/Ojeb98ReIDkh15DXmuVe2KwUemwWLTEJsG4bcroNHp7LS/6xiBvKxb+G31uBq4AhCdSZ4ardlziT1V2e+zxB5jZup1WxYIWH6NFR9Fl6mYDS8Ccf35UJ1tYIll8C0J2fBE4twtOzDmOzDcckNh68bAcWekcw7135y5jRRwLSVWqs2P5JoKfDkIsT9Z9Gv5AHLRvw2txs9Uj7EkAaWXFopiV+H3H8OfU31+NJ7EK6mBsiRCfglIeO1GnN4vZHIV7GFAqbCh5J09tPgFbgs1egRijHzsQHjiQL0W0vRJ1ggBfvA4uns6mrTUGJzYA47Lnt6cLhJgLNvFKo2C78kZJilNMdtqybqEWuoU9jp4R1HoY4sQz06DndjDS57D+BDQYBqrYBTFNFuqcJ46xAie/dCVjnU6BTYiVtwdkVxzO7AeR6C0xUAH7kBNTIBr2jJBOzmNQvz00ShBiPtqytp4d1R8Ngc3E0WnOchnHJ6wFUXVH0eauIGnAdOoc1ajUueXpxWfFBte2H39SNYb8Z46yDarTWQh66Axa9DjUyACZWZq5+eeRPAy+SrKiG/uUh2hQbhURjOqkGMyT64FSV7xbHZbO+Ti2CDn8PXbINuV3BYMuDCkVwMKlvRW1sBFhoCTy5kN234O7QpTfd++uGGCcB6GvCJL7UJ5R85jEUYVfy44OpES7MNXEuBxeYeDVlLgcXTUCITsDdY8HXSANw2YeKkAVKDGYo+m51TbBZqZAJuW/XtsdjgGwBeoF7FPHqiuxhaMB+H6irhE0WwyLWVgsdFyUUonUnEg7m4+VkREsGNUNqGsrmnpf6R+KWGjNO8bc3pZIRoQN70J5aN+D1dhk57Hhz9l8AS6WdGCQsOgNsboPgPQtWyt8X16cckQqbFaszhQgVRr2N7ZvHiDqROFiAg1kKNPCMMtRSYPg2WSEONpcHj6ZXv/5JUvldIdMhjL3TvWr/MrCYoAxfB43OrTuLHJe3Mdrt+T/FrREQUspYTK9ko87AONrL81BD83xLVlqko3rSWiIhaTUUUNBs8vFuDevwmWGLh+UguQdHn4Jesd3yNla8X571DJJVuIJ/5XWdroA2sZwSsO/Zc8P1xqKEhBO1Vd3yNFWvaxDqi6sK3SKnanGPZnu8N1BQFu4QSpXOVdAkliqdqu8dfV9ITVmqLPUI5uev30N8DAPKfisupFjUwAAAAAElFTkSuQmCC' /> Graphics Trends</button>\
    </form>\
    </fieldset>\
    </h4>\
    <h6>\
    <fieldset style='width:{{fieldsize4}}'>\
    <legend style='text-shadow: 2px 1px grey;'> System Infos </legend>\
      <div class='divTable'>\
          <div class='divRow'>\
            <div class='divColumn' style='width:{{fieldsize5}}'>\
              <div class='tabheader'>Uptime</div>\
              <div class='tabheader'>Next TX</div>\
              <div class='tabheader'>SSID</div>\
              <div class='tabheader'>RSSI</div>\
              <div class='tabheader'>BSSID</div>\
              <div class='tabheader'>IP addr</div>\
              <div class='tabheader'>Position</div>\
            </div>\
            <div class='divColumn' style='width:{{fieldsize6}}'>\
              <div id='uptime' class='notabheader' style='color: silver;'>{{uptime}}</div>\
              <div id='nexttx' class='notabheader' style='color: silver;'>{{nexttx}}</div>\
              <div class='notabheader' style='color: silver;'>{{SSID}}</div>\
              <div id='rssi' class='notabheader' style='color: silver;'>{{RSSI}}</div>\
              <div class='notabheader' style='color: silver;'>{{BSSID}}</div>\
              <div class='notabheader' style='color: silver;'>{{myip}}</div>\
              <div class='notabheader' style='color: silver;'>Lat: {{lat}} Long: {{long}} Alt: {{alt}}m asl</div>\
            </div>\
          </div>\
      </div>\
    </fieldset>\
    <br>\
    <form action='/submit' method='POST'>\
      <fieldset style='width:{{fieldsize7}}'>\
      <legend style='text-shadow: 2px 1px grey;'> Control Panel </legend>\
      <div class='grid-container'>\
        <div class='item1'>\
            <button class='butn' name='MiniWXSettings' type='settings' value='true' formaction='/settings' method='POST'><img src='data:image/png;base64,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'/> Settings</button>\
            <button class='butn' name='SendAPRS' type='submit' value='true'><img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAZCAYAAADE6YVjAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAIGNIUk0AAIcbAACL/wAA/bEAAINJAAB9LgAA7W0AADg+AAAkPqJQ9KgAAAPCSURBVHjarJZfSNRZFMc/jjON2Tr92a0UzXHtnzSpmCVhbFmS9iAUbG4vPkhET01EYpqp9bBPFfTQ+hA0DyH7sI/9wZamf4wRKFiRC9Mqs4iagk1MKGkyOd99uLk/p3XcjbxwmN/53nvO99x7zj13kiSxwCh4+ZKfQyEq+vtxhkIQDkNODmRlwebNTHg8NLnd/LKQk6REJPX1/BSJ8JvfD4ODiR3k50NlJUQi5F+/zh/zLpIUJ6CU5mYpJUWC/y9ut9TSoo7P/clMW0p1tb6vqvoy559Lfb00M5OABJS6f//XEcxKQ4M0L8mpU4tDMCvNzWqfS+JuaJB3MQlAysqSKiuVIQn71BTeYJC6oiJTll1dMDYGDgeUl4PdDoEAjI/DkiVQVgYfP8LAABQWmpLu7DRFZLPBgQPG9sED2LmTv4Bieno04XRKly9LkuT1mkg8Hv0zDh402I4dRu/qkoqLpXfvjF5aauZraoweCkkrVxof4bB+tXV38830NDx+/On2FZjfwkKIxcz39u0WBnDnDvT0QH290RsbweWCs2dhZgaOH4dIBEIhCAb5kaNHTRRr10pjY9KLF0a/ckUaHJSGhqSODikpSWprM5Hu22fWOBzSvXvSzIx0966Zu3YtPjft7RLl5RZw65YUjUrr1hkjv1/y+aS+PoM9eyaNjEhpaZbNtm1SOGwIenulNWviSS5ckGwjI9bt7+gwia6ogOxseP4cHj2C9HQoKjJHGQjAxIRlEwzC6Kj5fv3aFM3cEY2CLSPDAgIB+PABjhyB3Fxz7j09kJYGhw9DcrIJZO44cQK2boWpKRNcbe08vaumxtrasmVSd7f0/r0Ui0nr10tOp/TmjTQ+bvBNm6z1eXlSJCKNjkrV1dLbt9LwsJSZaa25cUOitVV/zj3Dq1fN+Q4PS0uXGuz+fYM9eWJIQUpNNTmTpDNnDDZ7DXw+UyhOp9TZqUnb5CTnXa74vAwMwM2b5ugAHj6EoSG4fRumpw22ezds2QJ+P/h8Brt0yeSxrAzy8iAzE9xu2ohGxd691k4cDtO2XS4LW75cys21djZb8hs2SCtWxFfTqlXSxo1mp+fOSZK+QxJNTbq42L0rPV2qrVVeXBc+dmxxSVpa9Pu/Wn1VlTJ37Vocgrq6BO/Jp90U7NnzdQSnT0ux2ALPryRKSvTtyZNScvKXOV+9WmptVe9/vvFzpbFRdYcOGeOFnOfkmOfB69WeRL6SFvjf5QBKnz7l/KtX/NDXh72/30zY7eaOeDxMZmfTVlLCRSCcyNHfAwBCmavUZ7FsDAAAAABJRU5ErkJggg=='/> Send APRS Packet</button>\
            <button class='butn' name='NTPSync' type='submit' value='true'><img src='data:image/png;base64,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'/> NTP Sync</button>\
            <button class='butn' name='WiFiScan' type='submit' value='true'><img src='data:image/png;base64,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'/> WiFi Scan</button>\
        </div>\
        <div class='item2'>\
            <button class='butn' name='Reboot' type='submit' value='true'><img src='data:image/png;base64,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'/> Reboot</button>\
        </div>\
      </div>\
      </fieldset>\
    </form>\
    <p>MiniWX Server&#8482; ({{SOFT_VER}}), inspired from the MiniWX project <a href='https://github.com/IU5HKU/MiniWXStation'>here</a>. Developed by YO7ZRO.</p>\
    </h6>\
  </body>\
</html>";

//***********************************************************
//* MINIWX STATION  - SETTINGS PAGE
//* set main values through webpage too
//***********************************************************
const char PAGE_MiniWXSettings_EN[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta  lang='en' http-equiv='Content-Type' content='text/html; charset=UTF-8'>\
<title>MiniWXSettings</title>\
</head>\
<style>\
  body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #FFB000; font-size: 16px; }\
  .divHeaderTable { width: 100%; padding-bottom:5px;  display:block; }\
  .divHeaderRow { width: 100%; display:block; height:105px; }\
  .divHeaderColumn { float: left; width: 33%; display:block; }\
  .divTable { width: 100%; display:block; padding-top:10px; padding-bottom:10px; padding-right:10px; padding-left:10px; }\
  .divRow { width: 99%; display:block; padding-bottom:5px; }\
  .divColumn { float: left; width: 49%; display:block; padding-left:8px; }\
  input[type=text] { padding-left:2px; border: 2px solid grey; border-radius: 4px; background-color: black; color: white; box-shadow:0 0 8px #999; font-size: 14px;}\
  fieldset {  border:1px solid #999;  border-radius:8px;  box-shadow:0 0 8px #999; }\
  legend { background:#000; text-shadow: 2px 1px grey; }\
  button { padding: 1px 6px 1px 6px;}\
  button > img,button > span { vertical-align: middle;}\
  button > text,button > span { vertical-align: middle;}\
</style>\
<style>\
.butn {\
  -webkit-border-radius: 10;\
  -moz-border-radius: 10;\
  border-radius: 10px;\
  text-shadow: 1px 1px 1px #666666;\
  -webkit-box-shadow: 0px 1px 3px #666666;\
  -moz-box-shadow: 0px 1px 3px #666666;\
  box-shadow: 0px 1px 1px #666666;\
  font-family: Arial;\
  color: #000;\
  font-size: 16px;\
  background: lightgray;\
  padding: 5px 10px 5px 40px;\
  text-decoration: none;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
}\
.butn:hover {\
  background: #FFB000;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
  text-decoration: none;\
}\
.butn:disabled {\
  background: gray;\
  background-repeat: no-repeat;\
  background-position: 10px center;\
  text-decoration: none;\
}\
</style>\
<body>\
  <div id='form_container'>\
  <hr>\
    <h1><a style='text-shadow: 2px 1px grey;'>MiniWX Station&#8482; Settings - {{SOFT_VER}}</a></h1>\
    <form style='width:58%' action='/submit' method='POST'>\
        <div class='divTable'>\
          <div class='divRow'>\
            <div class='divColumn' style='width:99%'>\
              <fieldset>\
              <legend><h2> Station Callsign and Position </h2></legend>\
                <label>Callsign</label>\
                <div><input id='callsign' name='callsign' type='text' maxlength='9' size='9' value='{{callsign}}'/></div>\
                <label>Longitude (example 01023.16E)</label>\
                <input id='longitude' name='longitude' type='text' maxlength='9' size='9' value='{{lon}}'/>\
                <label> Latitude (example 4303.01N)</label>\
                <input id='latitude' name='latitude' type='text' maxlength='8' size='8' value='{{lat}}'/>\
                <p style='margin:4px 0;border-top:1px solid #333;'></p>\
                <label style='color:#FFB000;font-weight:bold;'>DD Coordinates (Decimal Degrees &#8594; APRS)</label>\
                <label>DD Latitude</label>\
                <input id='dd_lat' type='text' maxlength='12' size='12' placeholder='44.482900'/>\
                <label> DD Longitude</label>\
                <input id='dd_lon' type='text' maxlength='12' size='12' placeholder='26.034100'/>\
                <div style='margin-top:6px;'>\
                <button type='button' onclick='convertDDtoAPRS();' style='padding:7px 14px;background:#333;color:#FFB000;border:0;font-size:14px;cursor:pointer;border-radius:4px;'>Convert DD &#8594; APRS</button>\
                <span style='color:#888;font-size:13px;margin-left:8px;'>updates the Longitude / Latitude fields above</span>\
                </div>\
<script>\
function ddToAprsLat(dd){var d=Math.floor(Math.abs(dd));var m=(Math.abs(dd)-d)*60;var ms=m.toFixed(2);while(ms.split('.')[0].length<2)ms='0'+ms;return(d<10?'0':'')+d+ms+(dd>=0?'N':'S');}\
function ddToAprsLon(dd){var d=Math.floor(Math.abs(dd));var m=(Math.abs(dd)-d)*60;var ms=m.toFixed(2);while(ms.split('.')[0].length<2)ms='0'+ms;var ds=('00'+d).slice(-3);return ds+ms+(dd>=0?'E':'W');}\
function aprsToDdLat(s){if(!s||s.length<5)return'';var d=parseFloat(s.substring(0,2));var m=parseFloat(s.substring(2,s.length-1));return((s.slice(-1)==='S'?-1:1)*(d+m/60)).toFixed(6);}\
function aprsToDdLon(s){if(!s||s.length<6)return'';var d=parseFloat(s.substring(0,3));var m=parseFloat(s.substring(3,s.length-1));return((s.slice(-1)==='W'?-1:1)*(d+m/60)).toFixed(6);}\
function convertDDtoAPRS(){var dlat=parseFloat(document.getElementById('dd_lat').value);var dlon=parseFloat(document.getElementById('dd_lon').value);if(!isNaN(dlat))document.getElementById('latitude').value=ddToAprsLat(dlat);if(!isNaN(dlon))document.getElementById('longitude').value=ddToAprsLon(dlon);}\
$(document).ready(function(){var lat=document.getElementById('latitude').value;var lon=document.getElementById('longitude').value;var dl=aprsToDdLat(lat);if(dl)document.getElementById('dd_lat').value=dl;var dlo=aprsToDdLon(lon);if(dlo)document.getElementById('dd_lon').value=dlo;});\
</script>\
                <label> Altitude (meters)</label>\
                <input id='altitude' name='altitude' type='text' maxlength='4' size='4' value='{{alt}}'/>\
              </fieldset>\
            </div>\
          </div>\
        </div>\
        <div class='divTable'>\
          <div class='divRow'>\
            <div class='divColumn'>\
              <fieldset>\
              <legend><h2>APRS Settings</h2></legend>\
                <label>Server Address</label>\
                <div><input id='aprsserveraddr' name='aprsserveraddr' type='text' maxlength='255' size='30' value='{{aprsserveraddr}}'/></div>\
                <label>Password</label>\
                <div><input id='aprspassw' name='aprspassw' type='text' maxlength='5' size='5' value='{{aprspassw}}'/></div>\
                <label>Server Port</label>\
                <div><input id='aprsserverport' name='aprsserverport' type='text' maxlength='5' size='5' value='{{aprsserverport}}'/></div>\
                <div style='margin-top:8px;'><button type='button' onclick='document.getElementById(\"aprsserveraddr\").value=\"rotate.aprs.net\";document.getElementById(\"aprsserverport\").value=\"14580\";' style='padding:7px 14px;background:#333;color:#FFB000;border:0;font-size:14px;cursor:pointer;border-radius:4px;'>Reset APRS to defaults</button></div>\
                <label>Transmission Delay (min)</label>\
                <div><input id='txdelay' name='txdelay' type='text' maxlength='2' size='2' value='{{txdelay}}'/></div>\
                <label>Sensor Read Interval (s)</label>\
                <div><input id='measinterval' name='measinterval' type='text' maxlength='5' size='5' value='{{measinterval}}'/></div>\
                <label>Temp. Offset BME280 (deg C)</label>\
                <div><input id='tempoffset' name='tempoffset' type='text' maxlength='6' size='6' value='{{tempoffset}}'/></div>\
                <label>Station Packets Comment</label>\
                <div><input id='aprscmnt1' name='aprscmnt1' type='text' maxlength='43' size='43' value='{{aprscmnt1}}'/></div>\
                <label>Telemetry Packets Comment</label>\
                <div><input id='aprscmnt2' name='aprscmnt2' type='text' maxlength='23' size='23' value='{{aprscmnt2}}'/></div>\
              </fieldset>\
            </div>\
            <div class='divColumn'>\
              <fieldset>\
              <legend><h2>STATIC IP Definitions</h2></legend>\
                <span>\
                  <input id='usestatic' name='usestatic' type='hidden' value='false'/>\
                  <input id='usestatic' name='usestatic' type='checkbox' value='true' {{usestaticip}} />\
                  <label>USE STATIC IP (reboot needed)</label>\
                </span><p></p>\
                <label>IP</label>\
                <div><input id='staticip' name='staticip' type='text' maxlength='15' size='15' value='{{staticip}}'/></div>\
                <label>Gateway</label>\
                <div><input id='staticgateway' name='staticgateway' type='text' maxlength='15' size='15' value='{{staticgateway}}'/></div>\
                <label>Subnet Mask</label>\
                <div><input id='staticmask' name='staticmask' type='text' maxlength='15' size='15' value='{{staticmask}}'/></div>\
                <label>DNS1</label>\
                <div><input id='staticdns1' name='staticdns1' type='text' maxlength='15' size='15' value='{{staticdns1}}'/></div>\
                <label>DNS2</label>\
                <div><input id='staticdns2' name='staticdns2' type='text' maxlength='15' size='15'  value='{{staticdns2}}'/></div>\
              </fieldset>\
            </div>\
          </div>\
          <div class='divRow'><div class='divColumn'></br></div></div>\
          <div class='divRow'>\
            <div class='divColumn'>\
              <fieldset>\
              <legend><h2>NTP Settings</h2></legend>\
                <label>Server</label>\
                <div><input id='ntpserver' name='ntpserver'  type='text' maxlength='255' value='{{ntpserver}}'/></div>\
                <label>NTPSync Delay (hours)</label>\
                <div><input id='ntpsyncdelay' name='ntpsyncdelay'  type='text' maxlength='2' size='2' value='{{ntpsyncdelay}}'/></div>\
                <label>UTC Offset (hours)</label>\
                <div><input id='timezone' name='timezone' type='text' maxlength='5' size='5' value='{{timezone}}' placeholder='e.g. 3 or -5'/></div>\
              </fieldset>\
            </div>\
          </div>\
          <div class='divRow'><div class='divColumn'></br></br></br></div></div>\
          <div class='divRow'>\
            <div class='divColumn' style='width:99%;padding-bottom:10px'>\
              <div>\
                <a><button class='butn' style='padding: 1px 6px 1px 6px;' name='save' type='submit' value='true'><img src='data:png;base64,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'/> Save & Exit </button></a>\
                <a><input class='butn' type='button' onclick='javascript:window.history.go(-1);' style='background-image: url(data:image/png;base64,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);' value='Exit' ></a>\
              </div>\
            </div>\
          </div>\
        </div>\
        <div id='footer' style='padding-left:18px'><p>MiniWX Server&#8482; ({{SOFT_VER}}), inspired from the MiniWX project <a href='https://github.com/IU5HKU/MiniWXStation'>here</a>. Developed by YO7ZRO.</p></div>\
    </form>\
  </div>\
</body>\
</html>";

#include "SystemWebpages.h"

#include <SparkFunBME280.h>
// Global sensor object
BME280 mySensor;

// webserver for parameters & main pages
ESP8266WebServer server(80);
DNSServer dnsServer;                       // captive portal for AP configuration mode
#define WIFI_CONNECT_TIMEOUT_MS 25000      // if it doesn't connect within 25s -> AP portal

//**** Auto-recovery watchdogs (for remote / off-grid operation) ****
#define WIFI_LOST_RESTART_MS     (10UL*60UL*1000UL)   // WiFi disconnected 10 min -> restart
#define INTERNET_LOST_RESTART_MS (10UL*60UL*1000UL)   // WiFi up but no internet 10 min -> restart
#define NET_PROBE_INTERVAL_MS    (60UL*1000UL)        // test internet reachability every 60 s
#define AP_PORTAL_TIMEOUT_MS     (10UL*60UL*1000UL)   // stuck in hotspot 10 min -> reboot & retry saved network

//**************************************
//* INTERNAL USE & DEBUG               *
//**************************************
const char SOFT_VER[] = "v1.7.2";
//#define DEBUG_READSETTINGSFILE
//#define DEBUG_FORM_REPLIES
#define DISPLAY_RW_OUTPUT
//#define DEBUG_SERIAL_PLOTTER

//sample data every 150000ms (2'30") for successive graphs
#define SAMPLING_GRAPHS_DATA  150000

//*** Temperature correction offset (deg C). 0.0 = no correction.
//*** Use as a fine adjustment AFTER switching to forced mode, if any bias remains.
#define TEMP_OFFSET_DEF 0.0f   // default BME280 temp offset; now configurable from /settings (sets.tempOffset)

//*** Default sensor read interval (seconds), adjustable from the /settings web page
#define MEAS_SECONDS_DEF 120

//**************************************
//* ADDED FEATURES  - SWITCHES         *
//**************************************


//**** CHOOSE SERIAL MONITOR BAUD RATE
//#define SER_MON_BAUDRATE 115200
#define SER_MON_BAUDRATE 74880

//**** How the station is named in your NET
const char* WiFi_hostname = "MiniWX";

//**** APRS PASSWORD (use -1 if you are using a CWOP callsign)
const char* AprsPassw = "YouAPRSnumericalPASS";

//**** APRS COMMENT, you can set this string as you want (max 43 chars)
const char* APRS_CMNT = "MiniWX Station YourHomeTown";

//**** APRS_PRJ, Telemetry Project Title (max 23 chars)
const char* APRS_PRJ = "MiniWX Project";

//**** Comment this for ESP.getVcc() value in telemetry
//**** getVcc function (system_get_vdd33) is only available when A0 pin17 is suspended (floating),
//**** this function measure the power voltage of VDD3P3 pin 3 and 4 (in the ESP8266 chip)
//#define HAVE_BATTERY

//**** uncomment this for weatherunderground upload,remember to set ID and PASSWORD of your account
//#define USE_WUNDER
//* change ID and PASSWORD with yours
const char ID [] = "YourWunderID";                      
const char PASSWORD [] = "YourWunderpasswd";

//**** show BME280 registers in Serial Output;
//#define DISPLAY_BME_REGS
//**** show BME280 values in Serial Output;
#define DISPLAY_BME_VAL

//**** blinking led to show that into the 10 minutes the system is still alive WILL BE ELIMINATED IN BATTERY POWERED VERSION (1" blink)
//**** NOTE: WEMOS D1 Mini doens't have this led, NodeMCU V0.9 & V1.0 have it.
//#define BLINK_RED_LED

//**** blinking led to show that ESP8266 is transmitting WILL BE ELIMINATED IN BATTERY POWERED VERSION (0.5" blink)
//#define BLINK_BLUE_LED

//**** show (annoying) animated clock in the serial output
//#define SHOW_TICKS

//**** Sync the soft clock every 12 hours
#define NTPSYNC_DELAY  12

//**** NTP Server to use
const char* NTP_Server = "ntp1.inrim.it"; // ntp (alternative: pool.ntp.org)

//**** Your time zone UTC related (floating point number)
#define TIME_ZONE 3.0f

//**** Set credential for OTA firmware upgrade <<--->>
//*uncomment the #define if you wanna use this handy feature
#define USE_OTA_UPGRADE
#ifdef USE_OTA_UPGRADE
const char* OTA_hostname = "MINIWX";
//PLEASE CHANGE THIS PASSWORD WITH YOUR OWN FOR SECURITY REASON
const char* OTA_passw = "esp8266";
#endif

//**** use static ip instead of dns one
//**** DISABLED: at startup it uses DHCP (IP from the router).
//**** If you want a static IP, enable it from the /settings web page (checkbox + address),
//**** or uncomment the line below.
//#define USE_STATIC_IP
//* change to reflect your net configuration
String static_ip =      "192.168.88.191";   // STATIC IP (= ce interogheaza SmallTV)
String static_gateway = "192.168.88.1";     // GATEWAY (routerul tau)
String static_mask =    "255.255.255.0";    // SUBNET MASK
String static_dns1 =    "8.8.8.8";          // DNS1
String static_dns2 =    "4.4.2.2";          // DNS2
IPAddress ip, gateway, mask, dns1, dns2;

//**************************************************************************************************
//* time related structures and vars
//* time server for ntp function
NTPtime NTPch(NTP_Server);
strDateTime dateTime;
byte nextMinTx;
byte nextHour;
byte nextSecTx;

// flags & Time counters
Ticker TkAlarm;
bool bSendFlag;
Ticker TkSeconds;
bool bSecsFlag;
Ticker TkBlueLed;
bool bNtpSyncFlag;
Ticker TkNtpSync;
bool bDataAcquisitionFlag;
Ticker TkDataAcquisition;

unsigned char cnt=-1; // the progressive counter for telemetry packet ID

//for the CheckSensor()
#define MOD_BMP280  0x58
#define MOD_BME280  0x60

typedef struct {
  //Sensor_type (BME280 or BMP280 autodetected)
  uint8_t ChipModel;
  //*APRS
  char AprsPassw[6];
  char APRS_CMNT[44];
  char APRS_PRJ[24];
  //*WUNDER
  bool usewunder;
  char wunderid[20];
  char wunderpassw[20];
  //*STATIC IP
  bool usestaticip;
  char static_ip[16];
  char static_gateway[16];
  char static_mask[16];
  char static_dns1[16];
  char static_dns2[16];
  //*NTP
  char NTP_Server[20];
  unsigned char NTP_SYNC_DELAY;
  int measSeconds;          // sensor read interval (seconds) - forced, on-demand
  float tempOffset;         // BME280 temperature correction (deg C) - web-configurable
  float timeZone;           // UTC offset (hours, e.g. 3.0 for UTC+3) - web-configurable
} Settings, *SettingsPtr;
Settings sets;
SettingsPtr sets_ptr = &sets;

typedef struct {
  int8_t rssi;
  unsigned int vbat;
} Telemetry, *TelemetryPtr;
Telemetry tl; // declare telemetry structure
TelemetryPtr tl_ptr = &tl;

typedef struct {
  float temp;
  float pres;
  float rhum;
  int8_t rssi;
} SampledData, *SampledDataPtr;
SampledData sd;
SampledDataPtr sd_ptr = &sd;
//Our circular buffer, for graphs and simple forecast...
//data are sampled every 2'30" the buffer holds the lasts 4 hours
//as reccomended here:
//http://people.duke.edu/~rnau/411trend.htm
//and here:
//https://stormtrack.org/community/threads/calculating-pressure-trend.5732/
std::vector<SampledData> sd_vec;
std::vector<SampledData>::iterator sd_it;
#define VECTOR_SIZE 97

typedef struct  {
  float temperatureC;
  float temperatureF;
  float pressure ;
  float humidity;
  float heatindex;
  double fdewptf;
} WeatherStruct, *WeatherStructPtr;
WeatherStruct wx;    //declare la structure
WeatherStructPtr wx_ptr = &wx;

// HEAT INDEX ROUTINE - (C) 2016 Tyler Glenn
// Thanks, the whole library is a great work indeed,
// maybe i'll use it in a next release.
/*
  Copyright (C) 2016  Tyler Glenn

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Written: Dec 30 2015.
  Last Updated: Dec 23 2017.

  This header must be included in any derived code or copies of the code.
*/

//*** Heat Index coefficients from the above library

#define hi_coeff1 -42.379
#define hi_coeff2   2.04901523
#define hi_coeff3  10.14333127
#define hi_coeff4  -0.22475541
#define hi_coeff5  -0.00683783
#define hi_coeff6  -0.05481717
#define hi_coeff7   0.00122874
#define hi_coeff8   0.00085282
#define hi_coeff9  -0.00000199

typedef struct  {
  char ssid[50];
  char password[50];
} configStruct, *configStructPtr;
configStruct internet;    //declare la structure
configStructPtr internet_ptr = &internet;

//for connections to clients (APRS servers and WUNDER server)
//better keep this declaration global (as seen in forums)
WiFiClient client;

typedef struct  {
  char callsign[10];
  char tlm_callsign[10];
  char longitude[10];
  char latitude[10];
  char clientAddress[20];
  int clientPort;
  long transmitDelay;
  byte logger;
  int altitude;
} positionStruct, *positionStructPtr;
positionStruct station;    //declare la structure
positionStructPtr station_ptr = &station;

char car;

//**************************************
//* WUNDERGROUND
//**************************************
const char WGserver[] PROGMEM = "weatherstation.wunderground.com";
const char WEBPAGE[] PROGMEM = "GET /weatherstation/updateweatherstation.php?";
// ===== Function prototypes (generated during the .ino -> .cpp conversion) =====
void calcDewPoint();
void Send2Wunder();
void SetOTA();
void SetSendFlag(void);
void SetSecsFlag(void);
void SetNtpSyncFlag(void);
void SetDataAcquisitionFlag(void);
void BlinkBlueLed();
void setup(void);
void SystemUpTime();
float CalcHeatIndex(float temperature, float humidity);
void AdjustFieldsets(String* page);
void handleRoot();
void handleSubmit();
void handleJQuery();
void handleSettings();
void handleNotFound();
void startConfigPortal();
void handleWifiPortal();
void handleWifiSave();
void handleGraphs();
void loop();
void updateTime();
void updateServer();
void calcNextTX();
void ntp();
unsigned int ReadVBAT ();
void Send2APRS();
void initBme();
void getBmeValues();
void printBme();
byte detectMenu();
void configMenu();
void configAcessPoint();
void configWeather();
int readCharArray(char *buffer);
char readCarMenu();
void wifiScan();
void writeSsidFile();
void readSsidFile();
void writeStationFile();
void readStationFile();
void writeSettingsFile();
void readSettingsFile();
void createEraselogger();
void showlogger();
void ssidConnect();
// ===== end of prototypes =====



void calcDewPoint() {
  // Calculate dew Point
  double A0 = 373.15 / (273.15 + wx.temperatureF);
  double SUM = -7.90298 * (A0 - 1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1) ;
  SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM - 3) * wx.humidity;
  double T = log(VP / 0.61078);
  wx.fdewptf = (241.88 * T) / (17.558 - T);
}

void Send2Wunder() {
  // Calculate dew Point
  calcDewPoint();
  // connect to wunderground
  if (!client.connect(FPSTR(WGserver), 80)) {
    Serial.println(F("Send2Wunder Fail"));
    return;
  }
  Serial.print(F("WeatherUnderground page updating...."));

  client.print(FPSTR(WEBPAGE));
  client.print(F("ID="));
  client.print(sets.wunderid);
  client.print(F("&PASSWORD="));
  client.print(sets.wunderpassw);
  client.print(F("&dateutc="));
  client.print("now");
  client.print(F("&tempf="));
  client.print(wx.temperatureF);
  client.print(F("&dewptf="));
  client.print(wx.fdewptf);
  client.print(F("&humidity="));
  client.print(wx.humidity);
  client.print(F("&baromin="));
  client.print((wx.pressure / 100) * 0.02953f); // 1 mbar = 0.02953 inHg
  //more compliant WU output by EA1CDV Antonio
  client.print(F("&softwaretype=MiniWX%20Station%20"));
  client.print(SOFT_VER);
  client.print(F("&action=updateraw"));
  client.println();
  delay(2500);
  //Serial.println("done!");

  //print server reply
  Serial.print(F("server reply:"));
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  client.stop();
}

//******************************************
//* ARDUINO OTA SET HANDLERS
//* for flashing the firmware through TCP/IP
//******************************************
#ifdef USE_OTA_UPGRADE
void SetOTA() {

  ArduinoOTA.setHostname(OTA_hostname);
  ArduinoOTA.setPassword(OTA_passw);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });

  ArduinoOTA.begin();
  Serial.println(F("OTA ready"));
  Serial.println();
}
#endif
//************************************************************************
//* TICKERS INTERRUPT ROUTINES
//* keep as short as posssible!
//* WARNING: the ESP can only use 32 bits to measure time, as times are calculated in uS this gives a total of 71 mins.
//* 32 bits = 4,294,967,295 / 1,000,000 = 4,294 Seconds / 60 = 71 Minutes.
//************************************************************************
void SetSendFlag(void) {
  bSendFlag = true;
}

//Soft Clock, quite precise indeed...
void SetSecsFlag(void) {
  bSecsFlag = true;
  dateTime.second = (dateTime.second + 1) % 60;

  if (dateTime.second == 0) {
    dateTime.minute = (dateTime.minute + 1) % 60;
    if (dateTime.minute == 0) {
      dateTime.hour = (dateTime.hour + 1) % 24;
    }
  }
}

//NTP Sync every NTPSYNC_DELAY hours
char ntpdelaycnt = 0x00;
void SetNtpSyncFlag(void) {
  if (ntpdelaycnt++ == NTPSYNC_DELAY) {
    bNtpSyncFlag = true;
    ntpdelaycnt = 0x00;
  }
}

// for DataGraphs
void SetDataAcquisitionFlag(void) {
  bDataAcquisitionFlag = true;
}

#ifdef BLINK_BLUE_LED
void BlinkBlueLed() {
  digitalWrite(D4, digitalRead(D4) ^ 1); // turn the ESP-12 LED off and on (HIGH is the voltage level and meaning OFF)
}
#endif

//**************************************
//* MAIN SETUP ROUTINE
//**************************************
void setup(void)
{
  strcpy(station.clientAddress, "rotate.aprs.net");  // default, reccomended over rotate.aprs.net
  station.clientPort = 14580;
  station.transmitDelay = 10; // expressed in minutes
  station.logger = 0;
  FSInfo fs_info;

  Serial.begin(SER_MON_BAUDRATE);  // to read the esp8266 core and startup messages
  Serial.println();
  delay(10);

  Wire.begin();

  if (!SPIFFS.begin()) {                 // mount SPIFFS (auto-format la prima folosire)
    Serial.println(F("SPIFFS mount failed -> formatting..."));
    SPIFFS.format();
    SPIFFS.begin();
  }

  if (SPIFFS.info(fs_info)) {
    Serial.println("SPIFFS successfully mounted");
    Serial.print("Total avail : "); Serial.print(fs_info.totalBytes); Serial.println(" bytes");
    Serial.print("Used        : "); Serial.print(fs_info.usedBytes); Serial.println(" bytes");
  }

  if (SPIFFS.exists("/ssid.txt") == 0) {
    Serial.println("^:WARNING:^ no ssid config -> starting the WiFi AP configuration portal");
    startConfigPortal();   // raises the hotspot, saves the SSID from the browser and reboots (does not return)
  }
  else {
    readSsidFile();
  }

  if (SPIFFS.exists("/station.txt") == 0) {
    Serial.println("^:WARNING:^ no station config -> writing defaults (configurable from the /settings browser page)");
    strcpy(station.callsign, "NOCALL");
    strcpy(station.tlm_callsign, "NOCALL");
    strcpy(station.latitude,  "0.0");
    strcpy(station.longitude, "0.0");
    station.altitude = 0;
    writeStationFile();
  }
  else {
    readStationFile();
  }

  //*DEFAULT VALUES for the SETTINGS STRUCTURE
  // APRS
  strcpy(sets.AprsPassw, AprsPassw);
  strcpy(sets.APRS_CMNT, APRS_CMNT);
  strcpy(sets.APRS_PRJ, APRS_PRJ);
  // WUNDER
#ifdef USE_WUNDER
  sets.usewunder = true;
#else
  sets.usewunder = false;
#endif
  strcpy(sets.wunderid, ID);
  strcpy(sets.wunderpassw, PASSWORD);
  // STATIC IP
#ifdef USE_STATIC_IP
  sets.usestaticip = true;
#else
  sets.usestaticip = false;
#endif
  static_ip.toCharArray(sets.static_ip, 16);
  static_gateway.toCharArray(sets.static_gateway, 16);
  static_mask.toCharArray(sets.static_mask, 16);
  static_dns1.toCharArray(sets.static_dns1, 16);
  static_dns2.toCharArray(sets.static_dns2, 16);
  // NTP
  strcpy(sets.NTP_Server, NTP_Server);
  sets.NTP_SYNC_DELAY = NTPSYNC_DELAY;
  sets.measSeconds = MEAS_SECONDS_DEF;
  sets.tempOffset  = TEMP_OFFSET_DEF;
  sets.timeZone    = TIME_ZONE;

  if (SPIFFS.exists("/settings.txt") == 0) {
    // absolutely 'first time use', write the defaults values
    Serial.println("^:WARNING:^ no settings config found, write defaults");
    writeSettingsFile();
  }
  else {
    //read the saved settings
    readSettingsFile();
  }
  sets.usewunder = false;   // Wunderground disabled (removed from MiniWX Station)

  // Configuration is done from the browser. The serial menu remains optional:
  // uncomment the next line if you want the serial menu at startup.
  // if (detectMenu() == 1) configMenu();

  ssidConnect();
  initBme();
  // this for save ChipID in defaults values, just in case you change from BMP to BME
  writeSettingsFile();
  printBme();

#ifdef USE_OTA_UPGRADE
  //** Init the required handlers for OTA firmware flashing
  SetOTA();
#endif

  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.on("/settings", handleSettings);
  server.on("/jquery", handleJQuery);
  server.on("/graphs", handleGraphs);
  server.on("/wifi", handleWifiPortal);      // WiFi reconfiguration from the browser
  server.on("/wifisave", handleWifiSave);
  server.onNotFound(handleNotFound);

  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "X-Forwarded-For"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);

  server.begin();
  Serial.println("Server started.");

  // First and most important sync with NTP Server, next sync will be done after 'xx' hours,
  // default 12 hours, but you can change it in 'settings' webpage.
  ntp();

  //set bSecsFlag for tickin'the clock
  bSecsFlag = false;
  TkSeconds.attach( 1, SetSecsFlag);

  //initialize random seed
  randomSeed(analogRead(A0));

  //NodeMCU V0.9 pinout for onboard LED
#ifdef BLINK_BLUE_LED
  pinMode(D4, OUTPUT);     // Initialize GPIO2 pin as an output
  digitalWrite(D4, HIGH);  // Led OFF
#endif
#ifdef BLINK_RED_LED
  pinMode(D0, OUTPUT);     // Initialize GPIO16 pin as an output WARNING: DO NOT USE in deepsleep() mode!!!
  digitalWrite(D0, HIGH);  // Led OFF
#endif
#ifdef HAVE_BATTERY
  pinMode(A0, INPUT);   // read from A0
#endif

  //set bSendFlag to true every transmitDelay seconds
  bSendFlag = false;
  TkAlarm.attach( station.transmitDelay * 60, SetSendFlag);

  //set bNtpSyncFlag for NTPsync() every 3600 seconds
  bNtpSyncFlag = false;
  TkNtpSync.attach( 3600, SetNtpSyncFlag);

  bDataAcquisitionFlag = false;
  TkDataAcquisition.attach_ms( SAMPLING_GRAPHS_DATA, SetDataAcquisitionFlag);

  //adviced to avoid memory fragmentation
  sd_vec.reserve(VECTOR_SIZE);
}

int sysUpTimeSec;
int sysUpTimeMn;
int sysUpTimeHr;
int sysUpTimeDy;

void SystemUpTime() {
  long millisecs = millis();
  sysUpTimeSec = int((millisecs / (1000)) % 60);
  sysUpTimeMn = int((millisecs / (1000 * 60)) % 60);
  sysUpTimeHr = int((millisecs / (1000 * 60 * 60)) % 24);
  sysUpTimeDy = int((millisecs / (1000 * 60 * 60 * 24)) % 365);
}

/****************************************************************/
//FYI: https://ehp.niehs.nih.gov/1206273/ in detail this flow graph: https://ehp.niehs.nih.gov/wp-content/uploads/2013/10/ehp.1206273.g003.png
float CalcHeatIndex( float temperature, float humidity)
{
  float heatIndex(NAN);

  if ( isnan(temperature) || isnan(humidity) )
  {
    return heatIndex;
  }

  temperature = (temperature * (9.0 / 5.0) + 32.0); /*conversion to [°F]*/

  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  if (temperature <= 40)
  {
    heatIndex = temperature;  //first red block
  }
  else
  {
    heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (humidity * 0.094)); //calculate A -- from the official site, not the flow graph

    if (heatIndex >= 79)
    {
      /*
        calculate B
        the following calculation is optimized. Simply spoken, reduzed cpu-operations to minimize used ram and runtime.
        Check the correctness with the following link:
        http://www.wolframalpha.com/input/?source=nav&i=b%3D+x1+%2B+x2*T+%2B+x3*H+%2B+x4*T*H+%2B+x5*T*T+%2B+x6*H*H+%2B+x7*T*T*H+%2B+x8*T*H*H+%2B+x9*T*T*H*H
      */
      heatIndex = hi_coeff1
                  + (hi_coeff2 + hi_coeff4 * humidity + temperature * (hi_coeff5 + hi_coeff7 * humidity)) * temperature
                  + (hi_coeff3 + humidity * (hi_coeff6 + temperature * (hi_coeff8 + hi_coeff9 * temperature))) * humidity;
      //third red block
      if ((humidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      {
        heatIndex -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);
      } //fourth red block
      else if ((humidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      {
        heatIndex += (0.02 * (humidity - 85.0) * (87.0 - temperature));
      }
    }
  }

  return (heatIndex - 32.0) * (5.0 / 9.0); /*conversion back to [°C]*/
}

//***********************************************************
//* CHECK BROWSER TYPE & ADJUST FIELDSETS SIZE
//***********************************************************
void AdjustFieldsets( String* page) {
  // Checking if the client is mobile or not, quite naive, but functional
  if (server.hasHeader("User-Agent")) {
    //here you can adjust the <fieldset style='width: '> to match your needs
    const char* mobile_fieldsizes[] = {"97%", "44%", "32%", "18%", "98%", "18%", "78%", "98%", "*"};
    const char* pc_fieldsizes[] = {"49%", "50%", "22%", "22%", "50%", "28%", "68%", "50%", "*"};
    String repl("{{fieldsize");
    int i = 0;

    if (server.header("User-Agent").indexOf("Android") > 0) {
      //ANDROID
      while (mobile_fieldsizes[i] != "*") {
        repl += String(i) + "}}";
        page->replace(repl, mobile_fieldsizes[i++]);
        repl = "{{fieldsize";
      }
    }
    else {
      //ALL THE OTHERS (PC for the great majority)
      while (mobile_fieldsizes[i] != "*") {
        repl += String(i) + "}}";
        page->replace(repl, pc_fieldsizes[i++]);
        repl = "{{fieldsize";
      }
    }
  }
}

//***********************************************************
//* MINIWX STATION  - handle root page request
//***********************************************************
void handleRoot() {

  String page = FPSTR(PAGE_Main_EN);

  char buffer[20];
  float dpdegc;

  readSettingsFile();

  AdjustFieldsets(&page);

  page.replace(F("{{callsign}}"), station.callsign);
  page.replace(F("{{lat}}"), station.latitude);
  page.replace(F("{{long}}"), station.longitude);
  page.replace(F("{{alt}}"), String(station.altitude));

  SystemUpTime();
  String sysUpTime(String(sysUpTimeDy) + "d : " + String(sysUpTimeHr) + "h : " + String(sysUpTimeMn) + "m : " + String(sysUpTimeSec) + "s" );
  page.replace(F("{{uptime}}"), sysUpTime);

  getBmeValues();

  // Zero-ing values that can't be display cause lack of rH% in bmp280
  switch (sets.ChipModel) {
    case MOD_BMP280:  //temp,press
      page.replace(F("{{ChipModel}}"), String("BMP280"));
      wx.humidity = 0.0f;
      wx.heatindex = 0.0f;
      dpdegc = 0.0f;
      break;
    case MOD_BME280:  //temp,press,rhum
      page.replace(F("{{ChipModel}}"), String("BME280"));
      calcDewPoint();
      dpdegc = (wx.fdewptf - 32.0f) * 0.55f; // Fahrenheit to Celsius
      break;
    case 0x00:  // NO SENSOR AT ALL
      page.replace(F("{{ChipModel}}"), String("NO SENSOR"));
      wx.temperatureC = 0.0f;
      wx.temperatureF = 0.0f;
      wx.pressure = 0.0f;
      wx.humidity = 0.0f;
      wx.heatindex = 0.0f;
      dpdegc = 0.0f;
      break;
  }

  page.replace(F("{{degC}}"), String((wx.temperatureC), 2));
  page.replace(F("{{mbar}}"), String((wx.pressure / 100), 2));
  page.replace(F("{{rHum}}"), String(wx.humidity, 2));
  page.replace(F("{{DPdegC}}"), String(dpdegc, 2));
  page.replace(F("{{HIdegC}}"), String(wx.heatindex, 2));

  sprintf(buffer, "%02d:%02d:%02d", nextHour, nextMinTx, nextSecTx);
  page.replace(F("{{nexttx}}"), buffer);

  sprintf(buffer, "%02d:%02d:%02d", dateTime.hour, dateTime.minute, dateTime.second);
  page.replace(F("{{time}}"), buffer);

  page.replace(F("{{SSID}}"), internet.ssid);
  page.replace(F("{{RSSI}}"), String(WiFi.RSSI()));
  page.replace(F("{{BSSID}}"), String(WiFi.BSSIDstr()));
  page.replace(F("{{myip}}"), WiFi.localIP().toString());

  if (sets.usewunder)
    page.replace(F("{{wunderstate}}"), F(" "));
  else
    page.replace(F("{{wunderstate}}"), F("disabled"));

  page.replace(F("{{SOFT_VER}}"), SOFT_VER);

  // Serve page actually
  server.sendHeader(F("Content-Length"), String(page.length()));
  server.send ( 200, F("text/html"), page );
}

//***********************************************************
//* MINIWX STATION  - handle the main page menu buttons
//***********************************************************
void handleSubmit() {
  String message;

  if (server.args() > 0 ) {
    //** common elements of the various pages
    message += FPSTR(HTTP_HEAD_HTML);
    message += FPSTR(HTTP_STYLE);
    message.replace(F("{{callsign}}"), station.callsign);

    //*Reboot button *********************************************
    if (server.argName(0) == "Reboot" && server.arg(0) == "true") {
      message += FPSTR(HTTP_REBOOT_SCRIPT);
      message += FPSTR(HTTP_BODY);
      // Serve page actually
      message += F("<fieldset style='width:49%'><legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; system message </legend>");
      message += F("<form><div class='divTable'><div class='divRow'><div class='divColumn' style='width:98%'>");
      message += F("<div class='notabheader'>MiniWX&#8482; is rebooting, please wait 20 secs...</div>");
      message += FPSTR(HTTP_FOOT);

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );
      delay(1000);

      ESP.restart();
      wdt_reset();
    }

    //*SendAPRS button *********************************************
    if (server.argName(0) == "SendAPRS" && server.arg(0) == "true") {
      message += FPSTR(HTTP_SCRIPT);
      message += FPSTR(HTTP_BODY);

      message.replace(F("{{language}}"), "en");
      //Display sysmsg in a new page and come back
      message += F("<fieldset style='width:49%'><legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; system message </legend>");

      message += F("<form><div class='divTable'><div class='divRow'><div class='divColumn' style='width:98%'><div class='notabheader'>Sending packets to APRS server...</div>");
      message += F("</div></div></div></form></fieldset>");
      message += FPSTR(HTTP_FOOT);

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );

      getBmeValues();
      Send2APRS();
    }

    //*SendWUNDER button *********************************************
    if (server.argName(0) == "SendWUNDER" && server.arg(0) == "true") {
      message += FPSTR(HTTP_SCRIPT);
      message += FPSTR(HTTP_BODY);

      //Display sysmsg in a new page and come backe

      message += F("<fieldset style='width:49%'><legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; system message </legend>");

      message += F("<form><div class='divTable'><div class='divRow'><div class='divColumn' style='width:98%'><div class='notabheader'>Sending packets to WUNDER server...</div>");
      message += F("</div></div></div></form></fieldset>");
      message += FPSTR(HTTP_FOOT);

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );

      getBmeValues();
      if (sets.usewunder) Send2Wunder();
    }

    //*NTPSYNC button *********************************************
    if (server.argName(0) == "NTPSync" && server.arg(0) == "true") {
      message += FPSTR(HTTP_SCRIPT);
      message += FPSTR(HTTP_BODY);

      //Display sysmsg in a new page and come backe

      message += F("<fieldset style='width:49%'><legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; system message </legend>");

      message += F("<form><div class='divTable'><div class='divRow'><div class='divColumn' style='width:98%'><div class='notabheader'>Sending NTP SYNC request to server...</div>");

      message += F("</div></div></div></form></fieldset>");

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );

      ntp();
    }

    // WiFi Scan and put results in a table with CSS Style
    if (server.argName(0) == "WiFiScan" && server.arg(0) == "true") {
      message += FPSTR(HTTP_NO_SCRIPT);
      message += FPSTR(HTTP_BUTN_STYLE);
      message += FPSTR(HTTP_BODY);
      int i = 0;
      int AvailNets = WiFi.scanNetworks();//Scan for total networks available
      // and prepare the table to show in webpage
      String stations(F("<div><table style='font-size: 20px'><thead><tr><th>SSID</th><th>RSSI (dbm)</th><th>CHANNEL</th><th>BSSID</th><th>ENCRYPTION TYPE</th></tr></thead><tbody>"));

      for ( i; i < AvailNets; ++i)
      {
        // Print SSID and RSSI for each network found
        stations += "<tr>";
        stations += "<td <td style='text-align:left'>";
        stations += WiFi.SSID(i);
        stations += "</td><td>";
        stations += WiFi.RSSI(i);
        stations += "</td><td>";
        stations += WiFi.channel(i);
        stations += "</td><td>";
        stations += WiFi.BSSIDstr(i);
        stations += "</td><td style='text-align:left'>";
        switch (WiFi.encryptionType(i))
        {
          case ENC_TYPE_WEP:
            stations += String(lock) + F(" WEP ");
            break;
          case ENC_TYPE_TKIP:
            stations += String(lock) + F(" WPA/PSK ");
            break;
          case ENC_TYPE_CCMP:
            stations += String(lock) + F(" WPA2/PSK ");
            break;
          case ENC_TYPE_NONE:
            stations += String(unlock) + F(" OPEN ");
            break;
          case ENC_TYPE_AUTO:
            stations += String(lock) + F(" WPA/WPA2/PSK ");
            break;
        }
        stations += "</td>";
      }
      stations += "</tr>";

      if (server.hasHeader("User-Agent")) {
        if (server.header("User-Agent").indexOf("Android") > 0) {
          //ANDROID
          message += F("<fieldset style='width:88%'>");
        }
        else {
          //ALL THE OTHERS (PC for the great majority)
          message += F("<fieldset style='width:52%'>");
        }
      }

      message += F("<legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; scanning found ");
      message += String(i);
      message += F(" networks </legend>");
      message += stations;
      message += F("</tbody></table></div></fieldset>");
      //message += F("<form>"); // Already in HTTP_EXIT_BUTN
      message += FPSTR(HTTP_EXIT_BUTN);

      message.replace(F("{{exit_btn}}"), "Exit");
      
      //message += F("</form>"); // Already in HTTP_EXIT_BUTN
      message += FPSTR(HTTP_FOOT);

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );
    }

    //***************************
    // HANDLE SETTINGS PAGE FORM
    //***************************
    if (server.args() > 2) {
      for (uint8_t i = 0; i < server.args(); i++)
      {
        //station
        if (server.argName(i) == F("callsign"))  server.arg(i).toCharArray(station.callsign, 10);
        if (server.argName(i) == F("latitude")) server.arg(i).toCharArray(station.latitude, 10);
        if (server.argName(i) == F("longitude")) server.arg(i).toCharArray(station.longitude, 10);
        if (server.argName(i) == F("altitude")) station.altitude = server.arg(i).toInt();

        //aprs
        if (server.argName(i) == F("aprsserveraddr"))  server.arg(i).toCharArray(station.clientAddress, 20);
        if (server.argName(i) == F("aprspassw")) server.arg(i).toCharArray(sets.AprsPassw, 6);
        if (server.argName(i) == F("aprsserverport")) station.clientPort = server.arg(i).toInt();
        if (server.argName(i) == F("txdelay")) station.transmitDelay = server.arg(i).toInt();
        if (server.argName(i) == F("measinterval")) sets.measSeconds = server.arg(i).toInt();
        if (server.argName(i) == F("tempoffset"))   sets.tempOffset  = server.arg(i).toFloat();
        if (server.argName(i) == F("aprscmnt1")) server.arg(i).toCharArray(sets.APRS_CMNT, 44);
        if (server.argName(i) == F("aprscmnt2")) server.arg(i).toCharArray(sets.APRS_PRJ, 24);

        //wunder
        if (server.argName(i) == F("usewunder") && server.arg(i) == "true") sets.usewunder = true;
        if (server.argName(i) == F("usewunder") && server.arg(i) == "false") sets.usewunder = false;
        if (server.argName(i) == F("wunderid")) server.arg(i).toCharArray(sets.wunderid, 20);
        if (server.argName(i) == F("wunderpassw")) server.arg(i).toCharArray(sets.wunderpassw, 20);

        //ntp
        if (server.argName(i) == F("ntpserver")) server.arg(i).toCharArray(sets.NTP_Server, 20);
        if (server.argName(i) == F("ntpsyncdelay")) sets.NTP_SYNC_DELAY = server.arg(i).toInt();
        if (server.argName(i) == F("timezone"))     sets.timeZone  = server.arg(i).toFloat();

        //static ip
        if (server.argName(i) == F("usestatic") && server.arg(i) == "true") sets.usestaticip = true;
        if (server.argName(i) == F("usestatic") && server.arg(i) == "false") sets.usestaticip = false;
        if (server.argName(i) == F("staticip")) server.arg(i).toCharArray(sets.static_ip, 16);
        if (server.argName(i) == F("staticgateway")) server.arg(i).toCharArray(sets.static_gateway, 16);
        if (server.argName(i) == F("staticmask")) server.arg(i).toCharArray(sets.static_mask, 16);
        if (server.argName(i) == F("staticdns1")) server.arg(i).toCharArray(sets.static_dns1, 16);
        if (server.argName(i) == F("staticdns2")) server.arg(i).toCharArray(sets.static_dns2, 16);

#ifdef DEBUG_FORM_REPLIES
        Serial.print(i);
        Serial.print(" ");
        Serial.print("argName: "); Serial.print(server.argName(i));
        Serial.print(" arg: "); Serial.println(server.arg(i));
#endif
      }
      //save new values in flash
      writeStationFile();
      writeSettingsFile();

      message += FPSTR(HTTP_SETS_SCRIPT);
      message += FPSTR(HTTP_BODY);
      message += F("<fieldset style='width:49%'><legend style='text-shadow: 2px 1px grey; font-size: 18px;'>MiniWX&#8482; system message </legend>");
      message += F("<form><div class='divTable'><div class='divRow'><div class='divColumn' style='width:98%'><div class='notabheader'>Saving new settings in Flash memory...</div>");
      message += F("</div></div></div></form></fieldset>");
      message += FPSTR(HTTP_FOOT);

      server.sendHeader(F("Content-Length"), String(message.length()));
      server.send ( 200, "text/html", message );
    }
  }
}


//***********************************************************
//* MINIWX STATION - JQUERY REPLIES
//***********************************************************
void handleJQuery() {
  char espclock[20];
  char nexttx[20];
  char uptime[40];
  float dpdegc;

  getBmeValues();
  SystemUpTime();
  // Zero-ing values that can't be display cause lack of rH% in bmp280
  switch (sets.ChipModel) {
    case MOD_BMP280:  //temp,press
      wx.humidity = 0.0f;
      wx.heatindex = 0.0f;
      dpdegc = 0.0f;
      break;
    case MOD_BME280:  //temp,press,rhum
      calcDewPoint();
      dpdegc = (wx.fdewptf - 32.0f) * 0.55f; // Fahrenheit to Celsius
      break;
    case 0x00:  // NO SENSOR AT ALL
      wx.temperatureC = 0.0f;
      wx.temperatureF = 0.0f;
      wx.pressure = 0.0f;
      wx.humidity = 0.0f;
      wx.heatindex = 0.0f;
      dpdegc = 0.0f;
      break;
  }
  sprintf(espclock, "%02d:%02d:%02d", dateTime.hour, dateTime.minute, dateTime.second);
  sprintf(nexttx, "%02d:%02d:%02d", nextHour, nextMinTx, nextSecTx);
  sprintf(uptime, "%02dd : %02dh : %02dm : %02ds", sysUpTimeDy, sysUpTimeHr, sysUpTimeMn, sysUpTimeSec);

  // sends multiple data in array-form
  server.send ( 200, "text/plain", String(espclock) + "," +
                String(wx.temperatureC, 2) + "," +
                String(wx.pressure / 100, 2) + "," +
                String(wx.humidity, 2) + "," +
                String(dpdegc, 2) + "," +
                String(wx.heatindex, 2) + "," +
                String(nexttx) + "," +
                String(WiFi.RSSI()) + "," +
                String(uptime)
              );

}

//***********************************************************
//* MINIWX STATION  - handle Settings webpage
//***********************************************************
void handleSettings() {

  String page = FPSTR(PAGE_MiniWXSettings_EN);

  //Load fields with previous values
  readSettingsFile();

  //Station placeholders
  page.replace(F("{{callsign}}"), station.callsign);
  page.replace(F("{{lat}}"), station.latitude);
  page.replace(F("{{lon}}"), station.longitude);
  page.replace(F("{{alt}}"), String(station.altitude));

  //APRS placeholders
  page.replace(F("{{aprsserveraddr}}"), station.clientAddress);
  page.replace(F("{{aprspassw}}"), sets.AprsPassw);
  page.replace(F("{{aprsserverport}}"), String(station.clientPort));
  page.replace(F("{{txdelay}}"), String(station.transmitDelay));
  page.replace(F("{{measinterval}}"), String(sets.measSeconds));
  page.replace(F("{{tempoffset}}"), String(sets.tempOffset, 1));
  page.replace(F("{{aprscmnt1}}"), sets.APRS_CMNT);
  page.replace(F("{{aprscmnt2}}"), sets.APRS_PRJ);

  //WUNDER placeholders
  if (sets.usewunder) {
    page.replace(F("{{usewunder}}"), F("checked"));
    page.replace(F("{{wunderid}}"), sets.wunderid);
    page.replace(F("{{wunderpassw}}"), sets.wunderpassw);
  }
  else {
    page.replace(F("{{usewunder}}"), "");
    page.replace("{{wunderid}}", "");
    page.replace("{{wunderpassw}}", "");
  }

  //NTP placeholders
  page.replace(F("{{ntpserver}}"), sets.NTP_Server);
  page.replace(F("{{ntpsyncdelay}}"), String(sets.NTP_SYNC_DELAY));
  page.replace(F("{{timezone}}"), String(sets.timeZone, 1));

  //STATICIP placeholders
  if (sets.usestaticip) {
    page.replace(F("{{usestaticip}}"), F("checked"));
  }
  page.replace(F("{{staticip}}"), sets.static_ip);
  page.replace(F("{{staticgateway}}"), sets.static_gateway );
  page.replace(F("{{staticmask}}"), sets.static_mask);
  page.replace(F("{{staticdns1}}"), sets.static_dns1);
  page.replace(F("{{staticdns2}}"), sets.static_dns2);

  page.replace(F("{{SOFT_VER}}"), SOFT_VER);

  server.sendHeader(F("Content-Length"), String(page.length()));
  server.send ( 200, "text/html", page );
}

//*************************************************
//* PAGE NOT FOUND
//*************************************************
void handleNotFound() {
  String message;

  message += FPSTR(HTTP_HEAD_HTML);
  message.replace(F("{{language}}"), "en");
  message += FPSTR(HTTP_404_STYLE);
  message += F("</head>");
  message += FPSTR(HTTP_BODY);
  message += FPSTR(HTTP_404_BODY);

  message += F("<div class='divColumn' style='width:72%'><div>");
  message +=  String(ESP.getFreeHeap()) + " bytes</div><div>";
  message +=  "0x" + String(ESP.getChipId(), HEX) + "</div><div>";
  message +=  "0x" + String(ESP.getFlashChipId(), HEX) + "</div><div>";
  message +=  String(ESP.getFlashChipSize()) + " bytes</div><div>";
  message +=  String(ESP.getCycleCount()) + " Cycles</div><div>";
  message +=  String(millis()) + " msec</div><div>";
  message += server.uri() + "</div><div>";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("</div><div>");
  message += "args count = " + String(server.args()) + "</div><div>";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += "<div>arg" + String(i) + ": name: " + server.argName(i) + " value: " + server.arg(i) + "</div>";
  }
  message += "</div>";
  message += F("</fieldset></div></div>");
  message += FPSTR(HTTP_FOOT);

  server.sendHeader(F("Content-Length"), String(message.length()));
  server.send(404, "text/html", message);
}

//*************************************************
//* DRAW GRAPHICS & TRENDS
//*************************************************
void handleGraphs() {
  String message;

  message += FPSTR(HTTP_SVG_HEAD);

  message.replace(F("{{language}}"), "en");
  message += FPSTR(HTTP_STYLE);
  message += FPSTR(HTTP_DIV_STYLE);
  message += FPSTR(HTTP_BUTN_STYLE);
  message += FPSTR(HTTP_SVG_BODY);
  message.replace(F("{{svg_temp}}"), "Temperature (°C)");
  message.replace(F("{{svg_pres}}"), "Pressure (hPa)");
  message.replace(F("{{svg_rhum}}"), "relative Humidity (%)");
  message.replace(F("{{svg_rssi}}"), "rssi (dbm)");
  message.replace(F("{{svg_grid}}"), "");   // self-contained drawChart() draws its own background/grid/axes

  message += FPSTR(HTTP_EXIT_BUTN);
  message.replace(F("{{exit_btn}}"), "Exit");
  // makes the data arrays for the svg graphs script
  message += F("<script> let graphData = [");
  for (sd_it = sd_vec.begin(); sd_it < sd_vec.end(); sd_it++) {
    message += "{ temp:" + String(sd_it->temp, 2) + ",";
    message += "  pres:" + String(sd_it->pres, 2) + ",";
    message += "  rhum:" + String(sd_it->rhum, 2) + ",";
    message += "  rssi:" + String(sd_it->rssi) + "},";
  }
  message += F("]</script>");

  //this actually draws the graphs
  message += FPSTR(HTTP_SVG_D3JS_LIBRARY);
  message += FPSTR(HTTP_SVG_INTERACTIVE_GRAPHS_STYLE);
  message += FPSTR(HTTP_SVG_DRAW_INTERACTIVE_GRAPHS);
  //message += FPSTR(HTTP_SVG_SAVESAMPLEDDATA);
  message += FPSTR(HTTP_SVG_FOOT);

  server.sendHeader(F("Content-Length"), String(message.length()));
  server.send ( 200, "text/html", message);
}

//*************************************************
//* MAIN PROGRAM LOOP
//*************************************************
// ===== Auto-recovery: network watchdogs =====
unsigned long wifiDownSince  = 0;
unsigned long lastInternetOK = 0;
unsigned long lastNetProbe   = 0;

// Lightweight, non-abusive internet reachability test: TCP-connect to a public DNS resolver.
bool internetReachable() {
  WiFiClient probe;
  probe.setTimeout(2000);
  IPAddress d1(1, 1, 1, 1), d2(8, 8, 8, 8);
  if (probe.connect(d1, 53)) { probe.stop(); return true; }
  if (probe.connect(d2, 53)) { probe.stop(); return true; }
  return false;
}

// Called every loop(): reboots the board if WiFi or internet stay down too long,
// so a remote/off-grid unit recovers on its own (it never gets stuck).
void netWatchdog() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {              // ---- WiFi is down ----
    if (wifiDownSince == 0) wifiDownSince = now;
    if (now - wifiDownSince >= WIFI_LOST_RESTART_MS) {
      Serial.println(F("[WATCHDOG] WiFi lost > 10 min -> restart"));
      delay(50);
      ESP.restart();
    }
    return;                                         // skip internet checks while WiFi is down
  }

  // ---- WiFi is connected ----
  if (wifiDownSince != 0) {                         // just reconnected: fresh internet window
    wifiDownSince  = 0;
    lastInternetOK = now;
  }
  if (lastInternetOK == 0) lastInternetOK = now;    // first initialization

  if (now - lastNetProbe >= NET_PROBE_INTERVAL_MS) {
    lastNetProbe = now;
    if (internetReachable()) lastInternetOK = now;
  }
  if (now - lastInternetOK >= INTERNET_LOST_RESTART_MS) {
    Serial.println(F("[WATCHDOG] internet lost > 10 min -> restart"));
    delay(50);
    ESP.restart();
  }
}

void loop()
{
#ifdef USE_OTA_UPGRADE
  //** this for upgrade the firmware OTA
  ArduinoOTA.handle();
#endif

  server.handleClient();

  netWatchdog();   // auto-recovery for remote/off-grid operation

  updateTime();

  if (bSendFlag) {

#ifdef BLINK_BLUE_LED
    TkBlueLed.attach(0.25, BlinkBlueLed);
#endif

    updateServer();

#ifdef BLINK_BLUE_LED
    TkBlueLed.detach();
    digitalWrite(D4, HIGH);  // Led OFF
#endif
    bSendFlag = false;
  }

  //*******************************************************
  if (bDataAcquisitionFlag) {
    // push sampled data in circular buffer
    //static int cnt;

    getBmeValues();

    sd.temp = wx.temperatureC;
    sd.pres = wx.pressure;
    sd.rhum = wx.humidity;
    sd.rssi = abs(WiFi.RSSI());

    //Circular Buffer with std::vector<> it holds 96 values
    if (sd_vec.size() <= (VECTOR_SIZE - 1))
      sd_vec.push_back(sd);         // first fill the buffer
    else {                          // then
      sd_vec.erase(sd_vec.begin()); // erase the first element
      sd_vec.push_back(sd);         // and add element in back position
    }

#ifdef DEBUG_SERIAL_PLOTTER
    char currentTime[10];
    sprintf(currentTime, "%02d:%02d:%02d", dateTime.hour, dateTime.minute, dateTime.second);
    Serial.print(currentTime);
    Serial.println(" - Graphs Data Sampled");
#endif

    bDataAcquisitionFlag = false;

#ifdef DEBUG_SERIAL_PLOTTER
    for (sd_it = sd_vec.begin(); sd_it < sd_vec.end(); sd_it++) {
      Serial.print(sd_it->temp); Serial.print(" ");
      Serial.print(sd_it->pres); Serial.print(" ");
      Serial.print(sd_it->rhum); Serial.print(" ");
      Serial.print(sd_it->rssi); Serial.println(" ");
    }
#endif
  }
  //*******************************************************

  if (Serial.available() > 0) {
    car = Serial.read();
    if (car == 'm') {
      while (Serial.read() != '\n') {};
      configMenu();
      ntp();
    }
    // immediately sends data
    if (car == 'f') {
      while (Serial.read() != '\n') {};
#ifdef BLINK_BLUE_LED
      TkBlueLed.attach(0.25, BlinkBlueLed);
#endif
      updateServer();
#ifdef BLINK_BLUE_LED
      TkBlueLed.detach();
      digitalWrite(D4, HIGH);  // Led OFF
#endif

    }
  }
}

//*************************************************************************
//*** Support function to keep lightweight the Soft Clock interrupt routine
//*************************************************************************
void updateTime()
{
  char currentTime[10];

  if (bSecsFlag) {

#ifdef SHOW_TICKS
    sprintf(currentTime, "%02d:%02d:%02d", dateTime.hour, dateTime.minute, dateTime.second);
    Serial.println(currentTime);
#endif

#ifdef BLINK_RED_LED
    digitalWrite(D0, digitalRead(D0) ^ 1); // turn the ESP-12 LED off and on (HIGH is the voltage level and meaning OFF)
#endif

    bSecsFlag = false;
  }
}

//********************************************************************
//*** Sends the packets to desired servers and to log file, if enabled
//********************************************************************
void updateServer()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    printBme();
    calcNextTX();
    if (bNtpSyncFlag) {
      Serial.println(F("Syncing softclock with NTP time"));
      ntp();   // NTP Sync every 12 hours is more than enough
      bNtpSyncFlag = false;
    }

    Send2APRS();
    //#ifdef USE_WUNDER
    if (sets.usewunder) Send2Wunder();
    //#endif
  }

  if (station.logger == 1) {
    if (SPIFFS.exists("/logger.txt") == 1) {
      String s;
      //long sizefile;
      File f = SPIFFS.open("/logger.txt", "a");
      if (!f) {
        Serial.println(F("file open failed"));
      }
      Serial.println(F("====== add data logger ========="));
      char buffer[50];
      sprintf(buffer, "%02d/%02d/%04d;", dateTime.day, dateTime.month, dateTime.year);
      f.print(buffer);
      sprintf(buffer, "%02d:%02d:%02d;", dateTime.hour, dateTime.minute, dateTime.second);
      f.print(buffer);
      sprintf(buffer, "%03d;%02d;%05d\n", (int)(wx.temperatureC), (int)(wx.humidity), (int)(wx.pressure));
      f.print(buffer);
      f.close();
    }
  }
}

//****************************************************
//* SYNCRONIZE SYSTEM TIME with NTP SERVERS
//* need to be modified, redundant library in use...
//****************************************************
#define SEND_INTV     10
#define RECV_TIMEOUT  10

void calcNextTX() {
  nextMinTx = (dateTime.minute + station.transmitDelay) % 60;
  nextSecTx = dateTime.second;
  if (nextMinTx < dateTime.minute) (nextHour = dateTime.hour + 1) % 24;
  else nextHour = dateTime.hour;
}

void ntp()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    // first parameter: UTC offset (sets.timeZone, web-configurable); second parameter: 0 = no auto DST (set the offset manually for summer/winter)

    //**** BIG ISSUE: in case of poor connection, we risk to remain in this loop forever
    NTPch.setSendInterval(SEND_INTV);
    NTPch.setRecvTimeout(RECV_TIMEOUT);
    do
    {
      dateTime = NTPch.getNTPtime(sets.timeZone, 0);   // DST=0: timeZone is the full UTC offset (set it manually for summer/winter)
      delay(1); //don't block the entire system pls...
    }
    while (!dateTime.valid);

    NTPch.printDateTime(dateTime);
    //dateTime.second = (dateTime.second + RECV_TIMEOUT) % 60;  //adjust the setRecvTimeout delay;
    calcNextTX();
    Serial.print(F(" ->>>>> next tx at : " ));
    char buffer[20];
    sprintf(buffer, "%02d:%02d:%02d", nextHour, nextMinTx, nextSecTx);
    Serial.println(buffer);
  }
}

//****************************************************
//* READ BATTERY VOLTAGE
//* 0.004V resolution for 4.2V (R100K)
//* 0.003V resolution for 3.3V (R10K)
//* If you are using NodeMCU v0.9 then you need
//* a resistor if you wanna measure voltage greater than 3.3V
//* https://forum.arduino.cc/index.php?topic=445538.0
//*
//****************************************************
unsigned int raw = 0;
float volt = 0.0;

#ifndef HAVE_BATTERY
ADC_MODE(ADC_VCC);      //needed for ESP.getVcc()
#endif

unsigned int ReadVBAT () {
  raw = analogRead(A0);
  volt = raw / 1023.0;
  //volt=volt*4.2f; //100Kohm resistor
  volt = volt * 3.3f; //10kohm resistor
  return ((unsigned int)(volt * 100)); // two decimals after the comma
}
//****************************************************

void Send2APRS()
{
  char login[60];
  char sentence[150];
  unsigned int len;          // for padding callsign

  //MANDATORY: CWOP doesn't need password, but need to register to the CWOP program, and obtain a valid callsign
  //sprintf(login, "user %s pass -1 vers VERSION ESP8266", station.callsign);

  sprintf(login, "user %s pass %s vers VERSION ESP8266", station.callsign, sets.AprsPassw); // user must be "yourcallsign-13" if you are an hamradio operator, otherwise request and use a CWOP callsign....

  //retrieve telemetry infos
  tl.rssi = abs(WiFi.RSSI());   // strenght of WiFi AP signal

#ifdef HAVE_BATTERY
  tl.vbat = ReadVBAT();         // Read 0-4.2Volts from A0 (if you use 100kohm res)
#else
  tl.vbat = (unsigned int) (ESP.getVcc() * 0.1f); // Read the power voltage of VDD3P3 pin 3 and 4 (in the ESP8266 chip)
#endif

  Serial.print(F("Connecting to APRS server..."));
  int retr = 20;
  while (!client.connect(station.clientAddress, station.clientPort) && (retr > 0)) {
    delay(50);
    --retr;
  }

  if (!client.connected()) {
    Serial.println(F("connection failed"));
    client.stop();
    return;
  }
  else
  {
    Serial.println(F("done"));
    lastInternetOK = millis();   // successful APRS contact = internet OK

    client.println(login);
    Serial.println(login);

    delay(3000); //as reccomended, 3" between login and sends packet
  }

  //print server reply
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // Send Position Lat/Long/Alt Report message
  // WORKAROUND: i'm using an hack, WX Station can not have the 43 chars wide comment field
  // and the ALTITUDE field, so i first send a "red dot icon" position report and after that
  // i change the report icon in WX again....dirty, but fully functional :-)

  if (cnt++==256) cnt=0; // packets are sent using a progressive counter instead of the previous random one
                         // the non-written standard de facto says to reset the counter after 256 packets
  
  if ( cnt == 0 || cnt == 64 || cnt == 128 || cnt == 192 ) // Sending one position packet only after 64 telemetry packets sent
  {
    Serial.println(F("** POSITION PACKET **"));
    sprintf(sentence, "%s>APRS,TCPIP*:=%s/%s&%s (%s)", station.callsign,
            station.latitude,
            station.longitude,
            sets.APRS_CMNT, SOFT_VER);
    client.println(sentence);
    Serial.println(sentence);
  }
  
  Serial.println(F("** WX PACKET **"));

  switch (sets.ChipModel) {
    case MOD_BMP280:  //temp,press and no rHum
      sprintf(sentence, "%s>APRS,TCPIP*:=%s/%s_.../...g...t%03dr...p...P...h..b%05d", station.callsign,
              station.latitude,
              station.longitude,
              (int)(wx.temperatureF),
              (int)(wx.pressure / 10));

      break;
    case MOD_BME280:  //temp,press,rHum
      sprintf(sentence, "%s>APRS,TCPIP*:=%s/%s_.../...g...t%03dr...p...P...h%02db%05d", station.callsign,
              station.latitude,
              station.longitude,
              (int)(wx.temperatureF),
              (int)(wx.humidity),
              (int)(wx.pressure / 10));
      break;
    case 0x00:  //no values at all
      sprintf(sentence, "%s>APRS,TCPIP*:=%s/%s_.../...g...t...r...p...P...h..b.....", station.callsign,
              station.latitude,
              station.longitude);
      break;
  }

  client.println(sentence);
  Serial.println(sentence);

  //print server reply, but the server here doesn't reply at all......
  //delay(500);
  //while (client.available()) {
  //  String line = client.readStringUntil('\r');
  //  Serial.print(line);
  //}

  // adjust callsign to 9 char as requested by tlm packet (see APRS101.pdf)
  strcpy(station.tlm_callsign, station.callsign);

  if ((len = strlen(station.callsign)) < 9)
  {
    do { // pad with spaces
      station.tlm_callsign[len++] = 0x20;
    }
    while (len < 9);
  }

  // Send telemetry sentences, refer to APRS101.pdf
  Serial.println(F("** TELEMETRY PACKETS **"));
  sprintf(sentence, "%s>APRS,TCPIP*:T#%03d,%03d,%03d,000,000,000,00000000", station.callsign,
          cnt,
          tl.rssi,
          tl.vbat);
  client.println(sentence);
  Serial.println(sentence);
  
  if ( cnt == 0)   // Send telemetry parameters only every 256 packets (it's enough)
    {
      //Define telemetry parameters (labels)
      sprintf(sentence, "%s>APRS,TCPIP*::%s:PARM.RSSI,VBAT",  station.callsign,
              station.tlm_callsign);
      client.println(sentence);
      Serial.println(sentence);
    
      //Define telemetry units
      sprintf(sentence, "%s>APRS,TCPIP*::%s:UNIT.dbm,V",  station.callsign,
              station.tlm_callsign);
      client.println(sentence);
      Serial.println(sentence);
    
      //Add telemetry coefficient so the APRS protocol can convert your raw values into real value.
      sprintf(sentence, "%s>APRS,TCPIP*::%s:EQNS.0,-1,0,0,0.01,0,0,0,0,0,0,0,0,0,0", station.callsign,
              station.tlm_callsign);
      client.println(sentence);
      Serial.println(sentence);
    
      //Send bits and project comment
      sprintf(sentence, "%s>APRS,TCPIP*::%s:BITS.00000000,%s",  station.callsign,
              station.tlm_callsign,
              sets.APRS_PRJ);
      client.println(sentence);
      Serial.println(sentence);
    
      Serial.println();
      Serial.print(F("closing connection..."));
      client.stop();
      Serial.println(F("closed!"));
    }

}

//******************************************************
//* BME related routines, init, get values, sense type
//*****************************************************

void initBme()
{
  //***Driver settings********************************//
  //commInterface can be I2C_MODE or SPI_MODE
  //specify chipSelectPin using arduino pin names
  //specify I2C address.  Can be 0x77(default) or 0x76

  //For I2C, enable the following and disable the SPI section
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;

  //For SPI enable the following and dissable the I2C section
  //mySensor.settings.commInterface = SPI_MODE;
  //mySensor.settings.chipSelectPin = 10;


  //***Operation settings*****************************//

  //renMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  mySensor.settings.runMode = 1; //Forced mode (reduces sensor self-heating)

  //tStandby can be:
  //  0, 0.5ms
  //  1, 62.5ms
  //  2, 125ms
  //  3, 250ms
  //  4, 500ms
  //  5, 1000ms
  //  6, 10ms
  //  7, 20ms
  mySensor.settings.tStandby = 4;

  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  mySensor.settings.filter = 0;

  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.tempOverSample = 1;

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.pressOverSample = 1;

  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.humidOverSample = 1;

  Serial.println(F("Program Started"));
  Serial.print(F("Starting BMP/BME280... result of .begin(): 0x"));

  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(mySensor.begin(), HEX);

  delay(10);
  sets.ChipModel = mySensor.readRegister(BME280_CHIP_ID_REG);
  switch (sets.ChipModel) {
    case MOD_BMP280:
      Serial.println(F("BMP280 Detected"));
      break;
    case MOD_BME280:
      Serial.println(F("BME280 Detected"));
      break;
    case 0x00:
      Serial.println(F("Unknown Sensor type"));
      break;
  }

#ifdef DISPLAY_BME_REGS
  Serial.println(F("Displaying ID, reset and ctrl regs"));

  Serial.print(F("ID(0xD0): 0x"));
  Serial.println(mySensor.readRegister(BME280_CHIP_ID_REG), HEX);
  Serial.print(F("Reset register(0xE0): 0x"));
  Serial.println(mySensor.readRegister(BME280_RST_REG), HEX);
  Serial.print(F("ctrl_meas(0xF4): 0x"));
  Serial.println(mySensor.readRegister(BME280_CTRL_MEAS_REG), HEX);
  Serial.print(F("ctrl_hum(0xF2): 0x"));
  Serial.println(mySensor.readRegister(BME280_CTRL_HUMIDITY_REG), HEX);

  Serial.print(F("\n\n"));


  Serial.print(F("Displaying all regs\n"));
  uint8_t memCounter = 0x80;
  uint8_t tempReadData;
  for (int rowi = 8; rowi < 16; rowi++ )
  {
    Serial.print(F("0x"));
    Serial.print(rowi, HEX);
    Serial.print(F("0:"));
    for (int coli = 0; coli < 16; coli++ )
    {
      tempReadData = mySensor.readRegister(memCounter);
      Serial.print((tempReadData >> 4) & 0x0F, HEX);//Print first hex nibble
      Serial.print(tempReadData & 0x0F, HEX);//Print second hex nibble
      Serial.print(" ");
      memCounter++;
    }
    Serial.print("\n");
  }

  Serial.print("\n\n");

  Serial.print(F("Displaying concatenated calibration words\n"));
  Serial.print(F("dig_T1, uint16: "));
  Serial.println(mySensor.calibration.dig_T1);
  Serial.print(F("dig_T2, int16: "));
  Serial.println(mySensor.calibration.dig_T2);
  Serial.print(F("dig_T3, int16: "));
  Serial.println(mySensor.calibration.dig_T3);

  Serial.print(F("dig_P1, uint16: "));
  Serial.println(mySensor.calibration.dig_P1);
  Serial.print(F("dig_P2, int16: "));
  Serial.println(mySensor.calibration.dig_P2);
  Serial.print(F("dig_P3, int16: "));
  Serial.println(mySensor.calibration.dig_P3);
  Serial.print(F("dig_P4, int16: "));
  Serial.println(mySensor.calibration.dig_P4);
  Serial.print(F("dig_P5, int16: "));
  Serial.println(mySensor.calibration.dig_P5);
  Serial.print(F("dig_P6, int16: "));
  Serial.println(mySensor.calibration.dig_P6);
  Serial.print(F("dig_P7, int16: "));
  Serial.println(mySensor.calibration.dig_P7);
  Serial.print(F("dig_P8, int16: "));
  Serial.println(mySensor.calibration.dig_P8);
  Serial.print(F("dig_P9, int16: "));
  Serial.println(mySensor.calibration.dig_P9);

  Serial.print(F("dig_H1, uint8: "));
  Serial.println(mySensor.calibration.dig_H1);
  Serial.print(F("dig_H2, int16: "));
  Serial.println(mySensor.calibration.dig_H2);
  Serial.print(F("dig_H3, uint8: "));
  Serial.println(mySensor.calibration.dig_H3);
  Serial.print(F("dig_H4, int16: "));
  Serial.println(mySensor.calibration.dig_H4);
  Serial.print(F("dig_H5, int16: "));
  Serial.println(mySensor.calibration.dig_H5);
  Serial.print(F("dig_H6, uint8: "));
  Serial.println(mySensor.calibration.dig_H6);

  Serial.println();
#endif
}

//*** Retrieve values from BME280 and fill in the structure
unsigned long lastBmeMeasMs = 0;   // timestamp of the last real measurement (forced, on-demand)

void getBmeValues() {

  float pres;

  //*** Measures for REAL only at the read interval set in /settings (measSeconds).
  //*** Between measurements the cached values (wx) are returned, to avoid reading on every request.
  {
    unsigned long _iv = (unsigned long)sets.measSeconds * 1000UL;          // seconds -> ms
    if (_iv < 5000UL) _iv = 5000UL;                                        // plafon minim de siguranta
    if (lastBmeMeasMs != 0 && (millis() - lastBmeMeasMs) < _iv) return;    // use cache
    lastBmeMeasMs = millis();
  }

  //*** Forced mode: a single on-demand measurement, then the sensor returns to sleep.
  //*** Elimina auto-incalzirea (si reduce consumul la cativa uA).
  mySensor.setMode(MODE_FORCED);
  { unsigned long _t0 = millis();
    do { delay(1); } while (mySensor.isMeasuring() && (millis() - _t0 < 120)); }

  wx.temperatureC = mySensor.readTempC() + sets.tempOffset;

  //*** calc standardized barometric pressure
  if ((pres = mySensor.readFloatPressure()) > 0.1f)
    wx.pressure = pres * ( pow(1.0 - (0.0065 * (float) station.altitude * -1 / (273.15 + wx.temperatureC)), 5.255));
  else
    wx.pressure = 0.0f;

  wx.temperatureF = wx.temperatureC * 1.8f + 32.0f;   // from C (with offset)
  wx.humidity =  mySensor.readFloatHumidity();
  wx.heatindex = CalcHeatIndex(wx.temperatureC, wx.humidity);

}

//*** Print BME280 values in Serial Monitor
void printBme()
{
  getBmeValues();

  float pres = mySensor.readFloatPressure(); // read pressure where we are (altitude related)

#ifdef DISPLAY_BME_VAL
  Serial.print(F("Pressure at home level: "));
  Serial.print((pres / 100), 2); //(Pa->mBar)
  Serial.println(F(" Pa"));

  Serial.print(F("Pressure at sea level: "));
  Serial.print((wx.pressure / 100), 2); //(Pa->mBar)
  Serial.println(F(" Pa"));

  Serial.print(F("Temperature: "));
  Serial.print(mySensor.readTempC(), 2);
  Serial.println(F(" degrees C"));

  Serial.print(F("Temperature: "));
  Serial.print(mySensor.readTempF(), 2);
  Serial.println(F(" degrees F"));

  Serial.print(F("%RH: "));
  Serial.print(mySensor.readFloatHumidity(), 2);
  Serial.println(F(" %"));

  Serial.println();
#endif
}

//****************************************************************************
//* WHOLE SYSTEM MENU
//****************************************************************************

byte detectMenu()
{
  long previousMillisSerial = 0;
  long currentMillisSerial;
  long EcratMillisSerial;
  int countDown = 0;
  Serial.println(F("m for boot menu"));
  previousMillisSerial = millis();
  do {
    currentMillisSerial = millis();
    EcratMillisSerial = currentMillisSerial - previousMillisSerial;
    if (Serial.available() > 0) {
      if (Serial.read() == 'm') {
        while (Serial.read() != '\n') {};
        return 1;
      }
    }
    if ((EcratMillisSerial / 1000) != countDown) {
      countDown++;
      Serial.write(countDown + 0x30);
    }
  }
  while (EcratMillisSerial < 10000);
  Serial.println();
  return 0;
}

void configMenu()
{
  char carMenu;
  do {
    carMenu = 0;
    Serial.println(F("-----------"));
    Serial.println(F("Config menu"));
    Serial.println(F("0 Quit menu"));
    Serial.println(F("1 format file system"));
    Serial.println(F("2 config wifi access point"));
    Serial.println(F("3 config weather station"));
    Serial.println(F("4 test ntp"));
    Serial.println(F("5 test bmp/bme 280"));
    Serial.println(F("6 test server upload"));
    Serial.println(F("7 print weather data logger (historic)"));
    Serial.println(F("8 create and erase weather data logger"));
    Serial.println(F("-----------"));
    carMenu = readCarMenu();
    switch (carMenu) {
      case '1' :
        Serial.println(F("Please wait 30 secs for SPIFFS to be formatted..."));
        Serial.println(SPIFFS.format() ? "format ok" : "format fail");
        break;
      case '2' :  configAcessPoint();
        break;
      case '3' :  configWeather();
        break;
      case '4' :  ssidConnect();
        ntp(); //prévoir un test de connexion
        break;
      case '5' :  initBme();
        printBme();
        break;
      case '6' :  initBme();
        printBme();
        ssidConnect();
        ntp;
        Send2APRS();
        if (sets.usewunder) Send2Wunder();
        break;
      case '7' :  showlogger();
        break;
      case '8' :  createEraselogger();
        break;
      case '0' :
        break;
      default : Serial.println(F("error"));
    }
  } while (carMenu != '0');
}

void configAcessPoint()
{
  if (SPIFFS.exists("/ssid.txt") == 1) {
    readSsidFile();
  }
  else
  {
    Serial.println(F("no ssid config file"));
  }
  char carMenu;
  do {
    carMenu = 0;
    Serial.println(F("-----------"));
    Serial.println(F("Config wifi access point menu"));
    Serial.println(F("0 Save and exit AP menu"));
    Serial.println(F("1 ssid list"));
    Serial.println(F("2 set ssid"));
    Serial.println(F("3 set ssid password"));
    Serial.println(F("4 show ssid config"));
    Serial.println(F("5 test ssid"));
    Serial.println(F("6 reset to DNS"));
    Serial.println(F("-----------"));
    carMenu = readCarMenu();
    switch (carMenu) {
      case '0' :
        break;
      case '1' :
        wifiScan();
        break;
      case '2' :
        Serial.println(F("type your ssid"));
        readCharArray(internet.ssid);
        break;
      case '3' :
        Serial.println(F("type your password"));
        readCharArray(internet.password);
        break;
      case '4' :
        Serial.println(F("your wifi ssid config is"));
        Serial.println(internet.ssid);
        Serial.println(internet.password);
        break;
      case '5' :
        Serial.println(F("test ssid internet access"));
        ssidConnect();
        break;
      case '6' :
        Serial.println(F("Reset System to DNS use..."));
        sets.usestaticip = false;
        writeSettingsFile();
        Serial.println(F("...done, now reboot please"));
        break;
      default :
        Serial.println(F("error"));
        break;
    }
  } while (carMenu != '0');
  writeSsidFile();
}

void configWeather()
{
  if (SPIFFS.exists("/station.txt") == 1) {
    readStationFile();
  }
  else
  {
    Serial.println(F("no station config file"));
  }
  char carMenu;
  char buffer[10];
  char len;
  do {
    carMenu = 0;
    Serial.println(F("-----------"));
    Serial.println(F("Config weather station"));
    Serial.println(F("0 Save and exit weather station menu"));
    Serial.println(F("1 set callsign station"));
    Serial.println(F("2 set longitude"));
    Serial.println(F("3 set latitude"));
    Serial.println(F("4 set altitude"));
    Serial.println(F("5 set server address"));
    Serial.println(F("6 set server port"));
    Serial.println(F("7 set transmit delay"));
    Serial.println(F("8 logger enable"));
    Serial.println(F("9 show weather config"));
    Serial.println(F("-----------"));
    carMenu = readCarMenu();
    switch (carMenu) {
      case '1' :
        Serial.println(F("type your callsign station ex: FWxxxx"));
        len = readCharArray(station.callsign);
        break;
      case '2' :
        Serial.println(F("type your longitude ex: 00012.21E"));
        readCharArray(station.longitude);
        break;
      case '3' :
        Serial.println(F("type your latitude ex: 4759.75N"));
        readCharArray(station.latitude);
        break;
      case '4' :
        Serial.println(F("type your altitude (meters) ex: 78"));
        readCharArray(buffer);
        station.altitude = atoi(buffer);
        break;
      case '5' :
        Serial.println(F("type your server address, default : cwop.aprs.net"));
        readCharArray(station.clientAddress);
        break;
      case '6' :
        Serial.println(F("type your server port, default : 14580"));
        readCharArray(buffer);
        station.clientPort = atoi(buffer);
        break;
      case '7' :
        Serial.println(F("type transmit delay, default 10 minutes"));
        readCharArray(buffer);
        station.transmitDelay = atoi(buffer);
        break;
      case '8' :
        Serial.println(F("logger enable 0/1, defaut 0"));
        readCharArray(buffer);
        station.logger = atoi(buffer);
        break;
      case '9' :
        Serial.print(F("callsign : "));
        Serial.println(station.callsign);
        Serial.print(F("longitude : "));
        Serial.println(station.longitude);
        Serial.print(F("latitude : "));
        Serial.println(station.latitude);
        Serial.print(F("altitude : "));
        Serial.println(station.altitude);
        Serial.print(F("server address : "));
        Serial.println(station.clientAddress);
        Serial.print(F("server port : "));
        Serial.println(station.clientPort);
        Serial.print(F("tx delay : "));
        Serial.println(station.transmitDelay);
        Serial.print(F("logger enable : "));
        Serial.println(station.logger);
        break;
      case '0' :
        break;
      default : Serial.println(F("error"));
        break;
    }
  } while (carMenu != '0');
  writeStationFile();
}

//********************************
//* READ CHARS FROM PC SERIAL COMM
//********************************
int readCharArray(char *buffer)
{
  char car;
  int ptr = 0;

  do
  {
    if (Serial.available() > 0) {
      car = Serial.read();
      if (car != '\n') {
        buffer[ptr++] = car;
      }
    }
  }
  while (car != '\n');

  buffer[ptr] = 0;
  // return the number of char read
  return ptr;
}
//********************************

char readCarMenu()
{
  char car = 0;
  char ret = 0;
  while (car != '\n')
  {
    if (Serial.available() > 0) {
      car = Serial.read();
      if ((car >= '0') && (car <= '9')) {
        ret = car;
      }
    }
  }
  return ret;
}

//******************************************
//* WiFi Scan (every parameter i can obtain)
//******************************************
void wifiScan()
{
  Serial.println(F("scan start"));
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (n == 0)
    Serial.println(F("no networks found"));
  else
  {
    Serial.print(n);
    Serial.println(F(" networks found"));
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(F(": "));
      Serial.print(WiFi.SSID(i));
      Serial.print(F(" ("));
      Serial.print(WiFi.RSSI(i));
      Serial.print("dbm");
      Serial.print(F(")"));
      Serial.print(F(" CH:"));
      Serial.print(WiFi.channel(i));
      Serial.print(F(" BSSID:"));
      Serial.print(WiFi.BSSIDstr(i));
      switch (WiFi.encryptionType(i))
      {
        case ENC_TYPE_WEP:
          Serial.println(F(" WEP "));
          break;
        case ENC_TYPE_TKIP:
          Serial.println(F(" WPA/PSK "));
          break;
        case ENC_TYPE_CCMP:
          Serial.println(F(" WPA2/PSK "));
          break;
        case ENC_TYPE_NONE:
          Serial.println(F(" OPEN "));
          break;
        case ENC_TYPE_AUTO:
          Serial.println(F(" WPA/WPA2/PSK "));
          break;
      }
      delay(10);
    }
  }
  Serial.println("");
}

//***********************
//* MANAGE SPIFFS FILES *
//***********************

//**** WRITE/READ SSID ***************************************************************
void writeSsidFile()
{
  File ssidFile = SPIFFS.open("/ssid.txt", "w+");
  if (!ssidFile) {
    Serial.println(F("ssid.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Writing to ssid.txt file     =========>"));
#endif
  size_t bytes = ssidFile.write((unsigned char*)(internet_ptr), sizeof(configStruct));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  ssidFile.close();
  return;
}

void readSsidFile()
{
  File ssidFile = SPIFFS.open("/ssid.txt", "r+");
  if (!ssidFile) {
    Serial.println(F("ssid.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Reading ssid.txt file        =========>"));
#endif
  size_t bytes = ssidFile.read((unsigned char*)(internet_ptr), sizeof(configStruct));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  ssidFile.close();
  return;
}

//**** WRITE/READ STATION **************************************************************
void writeStationFile()
{
  File stationFile = SPIFFS.open("/station.txt", "w+");
  if (!stationFile) {
    Serial.println(F("station.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Writing to station.txt file  =========>"));
#endif
  size_t bytes = stationFile.write((unsigned char*)(station_ptr), sizeof(positionStruct));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  stationFile.close();
  return;
}

void readStationFile()
{
  File stationFile = SPIFFS.open("/station.txt", "r+");
  if (!stationFile) {
    Serial.println(F("station.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Reading station.txt file     =========>"));
#endif
  size_t bytes = stationFile.read((unsigned char*)(station_ptr), sizeof(positionStruct));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  stationFile.close();
  return;
}

//**** WRITE/READ SETTINGS *************************************************************
void writeSettingsFile()
{
  File setsFile = SPIFFS.open("/settings.txt", "w+");
  if (!setsFile) {
    Serial.println(F("settings.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Writing to settings.txt file =========>"));
#endif
  size_t bytes = setsFile.write((unsigned char*)(sets_ptr), sizeof(Settings));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  setsFile.close();
  return;
}

void readSettingsFile()
{
  File setsFile = SPIFFS.open("/settings.txt", "r+");
  if (!setsFile) {
    Serial.println(F("settings.txt file open failed"));
    return;
  }
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(F("====== Reading settings.txt file    =========>"));
#endif
  size_t bytes = setsFile.read((unsigned char*)(sets_ptr), sizeof(Settings));
#ifdef DISPLAY_RW_OUTPUT
  Serial.print(bytes); Serial.println(F(" bytes<"));
#endif
  setsFile.close();

#ifdef DEBUG_READSETTINGSFILE
  //Sensor_type (BME280 or BMP280 autodetected)
  Serial.println(); Serial.print(sets.ChipModel, HEX ); Serial.print(",");
  //*APRS
  Serial.print(sets.AprsPassw); Serial.print(",");
  Serial.print(sets.APRS_CMNT); Serial.print(",");
  Serial.print(sets.APRS_PRJ); Serial.print(",");
  //*WUNDER
  Serial.print(sets.usewunder); Serial.print(",");
  Serial.print(sets.wunderid); Serial.print(",");
  Serial.print(sets.wunderpassw); Serial.print(",");
  //*STATIC IP
  Serial.print(sets.usestaticip); Serial.print(",");
  Serial.print(sets.static_ip); Serial.print(",");
  Serial.print(sets.static_gateway); Serial.print(",");
  Serial.print(sets.static_mask); Serial.print(",");
  Serial.print(sets.static_dns1); Serial.print(",");
  Serial.print(sets.static_dns2); Serial.print(",");
  //*NTP
  Serial.print(sets.NTP_Server); Serial.print(",");
  Serial.print(sets.NTP_SYNC_DELAY); Serial.println();
#endif
  return;
}

//**************LOGGER RELATED ROUTINES***********************
void createEraselogger()
{
  File f = SPIFFS.open("/logger.txt", "w");
  if (!f) {
    Serial.println(F("file open failed"));
  }
  Serial.println(F("====== new logger file ========="));
  f.println("date;time;temperature;humidity;pressure");
  f.close();
}

void showlogger()
{
  if (SPIFFS.exists("/logger.txt") == 1) {
    String s;
    //long sizefile;
    File f = SPIFFS.open("/logger.txt", "r");
    if (!f) {
      Serial.println(F("file open failed"));
    }
    //sizefile=f.size()-42;
    Serial.println(F("====== read logger file ========="));
    do {
      s = f.readStringUntil('\n');
      Serial.println(s);
    }
    while (s.length() > 0);
    f.close();
  }
}
//***********************************************************

//***********************************************************
//* AP CONFIG PORTAL - WiFi provisioning from the browser
//* (replaces the SSID configuration from the serial menu)
//***********************************************************
void handleWifiPortal() {
  String p;
  p  = F("<!DOCTYPE html><html><head><meta charset='utf-8'>"
         "<meta name='viewport' content='width=device-width,initial-scale=1'>"
         "<title>MiniWX WiFi</title><style>"
         "body{font-family:sans-serif;margin:18px auto;max-width:480px;padding:0 14px}"
         "h2{margin:0 0 12px}label{font-size:14px;color:#444}"
         "input,select{width:100%;padding:9px;margin:5px 0 12px;box-sizing:border-box;"
         "border:1px solid #bbb;border-radius:6px;font-size:15px}"
         "button{width:100%;padding:11px;border:0;border-radius:6px;background:#1565c0;color:#fff;font-size:16px}"
         ".muted{color:#888;font-size:13px}</style></head><body>");
  p += F("<h2>MiniWX &mdash; WiFi setup</h2>");
  p += F("<form method='POST' action='/wifisave'>");
  p += F("<label>Networks found</label>"
         "<select onchange=\"document.getElementById('ssid').value=this.value\">");
  p += F("<option value=''>-- select network --</option>");
  int nets = WiFi.scanNetworks();
  for (int i = 0; i < nets; i++) {
    String enc = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? String(F(" (open)")) : String("");
    p += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) +
         " (" + WiFi.RSSI(i) + " dBm)" + enc + "</option>";
  }
  p += F("</select>");
  p += F("<label>SSID</label><input id='ssid' name='ssid' value=''>");
  p += F("<label>Password</label><input name='pass' type='password' value=''>");
  p += F("<button type='submit'>Save and reboot</button></form>");
  p += F("<p class='muted'>After saving, the board reboots and connects to the selected network. "
         "If it fails, it automatically returns to this setup mode.</p>");
  p += F("</body></html>");
  server.send(200, "text/html", p);
}

void handleWifiSave() {
  if (server.hasArg("ssid") && server.arg("ssid").length() > 0) {
    memset(internet.ssid, 0, sizeof(internet.ssid));
    memset(internet.password, 0, sizeof(internet.password));
    server.arg("ssid").toCharArray(internet.ssid, sizeof(internet.ssid));
    server.arg("pass").toCharArray(internet.password, sizeof(internet.password));
    writeSsidFile();
    Serial.print(F("[WiFi] SSID saved: ")); Serial.println(internet.ssid);
    server.send(200, "text/html",
      F("<!DOCTYPE html><html><head><meta charset='utf-8'>"
        "<meta http-equiv='refresh' content='12;url=/'></head>"
        "<body style='font-family:sans-serif'>"
        "<h3>Saved. Rebooting and connecting...</h3>"
        "<p>If the network is correct, the board appears on the network within a few seconds.</p></body></html>"));
    delay(900);
    ESP.restart();
  } else {
    server.send(200, "text/html",
      F("<html><body style='font-family:sans-serif'>Missing SSID. "
        "<a href='/'>back</a></body></html>"));
  }
}

void startConfigPortal() {
  String ap = "MiniWX-Setup-" + String(ESP.getChipId() & 0xFFFF, HEX);

  // clean radio start (prevents an in-progress STA attempt from blocking the AP)
  WiFi.persistent(false);
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP_STA);            // AP (config) + STA (network scan)
  delay(200);

  // fixed, predictable IP for the AP
  IPAddress apIP(192, 168, 4, 1), apGW(192, 168, 4, 1), apMask(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apGW, apMask);

  bool ok = WiFi.softAP(ap.c_str());          // open hotspot, channel 1
  for (uint8_t k = 0; k < 3 && !ok; k++) {    // retry if it failed
    delay(300);
    ok = WiFi.softAP(ap.c_str());
  }

  Serial.println();
  Serial.println(F("***************************************************"));
  Serial.println(F("* WiFi CONFIGURATION MODE                          *"));
  Serial.print  (F("* Hotspot: "));            Serial.println(ap);
  Serial.print  (F("* softAP started: "));     Serial.println(ok ? F("YES") : F("NO (!!)"));
  Serial.print  (F("* Open in browser: http://")); Serial.println(WiFi.softAPIP());
  Serial.println(F("***************************************************"));

  dnsServer.start(53, "*", WiFi.softAPIP());   // captive portal

  server.on("/", handleWifiPortal);
  server.on("/wifisave", handleWifiSave);
  server.onNotFound(handleWifiPortal);
  server.begin();

  unsigned long portalStart = millis();
  while (true) {                     // serve the portal until save OR until timeout
    dnsServer.processNextRequest();
    server.handleClient();
    yield();
    // Off-grid safety: don't stay in hotspot forever. After the timeout, reboot and
    // retry the saved network (which may have come back in the meantime).
    if (millis() - portalStart >= AP_PORTAL_TIMEOUT_MS) {
      Serial.println(F("[WATCHDOG] hotspot timeout -> restart & retry saved network"));
      delay(50);
      ESP.restart();
    }
  }
}

void ssidConnect()
{
  Serial.println(internet.ssid);
  Serial.println(internet.password);

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("Connecting to "));
    Serial.println(internet.ssid);

    WiFi.persistent(false);       // WiFi config isn't saved in flash
    WiFi.mode(WIFI_STA);          // use WIFI_AP_STA if you want an AP
    WiFi.setAutoReconnect(true);  // let the ESP retry the saved network on its own
    WiFi.hostname(WiFi_hostname); // must be called before wifi.begin()
    WiFi.begin(internet.ssid, internet.password);

    //****************************************************
    //* STATIC IP DEFINITION, comment these lines for DNS
    //****************************************************
    if (sets.usestaticip) {
      ip.fromString(static_ip);             // STATIC IP
      gateway.fromString(static_gateway);   // GATEWAY
      mask.fromString(static_mask);         // SUBNET MASK
      dns1.fromString(static_dns1);         // DNS1
      dns2.fromString(static_dns2);         // DNS2
      WiFi.config(ip, gateway, mask, dns1, dns2);
    }
    //****************************************************

    // Wait for connection with timeout; if it fails -> AP reconfiguration portal
    unsigned long _wt = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
      if (millis() - _wt > WIFI_CONNECT_TIMEOUT_MS) {
        Serial.println(F("\n[WiFi] timeout -> starting the AP configuration portal"));
        startConfigPortal();   // does not return (saves from the browser and reboots)
      }
    }
  }

  Serial.println();
  Serial.print(F("Connected to "));
  Serial.println(internet.ssid);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}