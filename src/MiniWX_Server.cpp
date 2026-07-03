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


//===== EN_Locale content (merged from EN_Locale.h) =====
//***********************************************************
//* MINIWX STATION  - MAIN INFO PAGE
//* improved readability on portable devices by EA1CDV Antonio
//* WebPages in FLASH and smart dynamics pages from:
//* http://blog.tldnr.org/2017/10/25/how-to-deliver-larger-web-pages-with-an-esp8266/
//***********************************************************
const char PAGE_Main_EN[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta charset='UTF-8'>\
<meta name='viewport' content='width=device-width,initial-scale=1'>\
<title>MiniWX Weather Server</title>\
<style>\
:root{--bg:#0d0f12;--card:#181b20;--card2:#1f232a;--acc:#FFB000;--txt:#e8e8e8;--mut:#8a9099;--line:#2a2f37;--cy:#4FC3F7;--gr:#81C784;}\
*{box-sizing:border-box;}\
body{margin:0;background:var(--bg);color:var(--txt);font-family:'Segoe UI',Arial,Helvetica,sans-serif;}\
a{color:var(--acc);text-decoration:none;}\
.wrap{max-width:1400px;margin:0 auto;padding:16px;}\
.top{display:flex;justify-content:space-between;align-items:flex-start;flex-wrap:wrap;gap:10px;padding:6px 4px 16px;}\
.brand{font-size:28px;font-weight:700;color:var(--acc);letter-spacing:1px;}\
.brand small{display:block;font-size:13px;color:var(--mut);font-weight:400;margin-top:5px;letter-spacing:0;}\
.clock{text-align:right;}\
.clock .t{font-size:28px;font-weight:600;}\
.clock .d{font-size:13px;color:var(--mut);margin-top:5px;}\
.sec{background:var(--card);border-radius:10px;padding:14px 16px;margin-bottom:16px;}\
.sec-h{display:flex;justify-content:space-between;color:var(--mut);font-size:14px;border-bottom:1px solid var(--line);padding-bottom:9px;margin-bottom:14px;}\
.row2{display:grid;gap:14px;grid-template-columns:1fr 320px;}\
@media(max-width:860px){.row2{grid-template-columns:1fr;}}\
.grid{display:grid;gap:12px;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));}\
.card{background:var(--card2);border-radius:8px;padding:14px 16px;}\
.card .lab{color:var(--mut);font-size:14px;margin-bottom:14px;}\
.card .val{font-size:36px;font-weight:300;line-height:1;}\
.card .val u{font-size:15px;color:var(--mut);text-decoration:none;margin-left:5px;}\
.gauge{display:flex;align-items:center;justify-content:center;}\
.charts{display:grid;gap:12px;grid-template-columns:1fr 1fr;}\
@media(max-width:860px){.charts{grid-template-columns:1fr;}}\
.chart-card{background:var(--card2);border-radius:8px;padding:10px 12px;}\
.chart-card .ct{color:var(--mut);font-size:13px;margin-bottom:4px;}\
.chart-card svg{width:100%;height:auto;display:block;}\
.info{display:grid;gap:10px;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));}\
.info .i{background:var(--card2);border-radius:8px;padding:10px 14px;}\
.info .i b{display:block;color:var(--mut);font-size:12px;font-weight:600;margin-bottom:4px;}\
.info .i span{color:var(--txt);font-size:15px;}\
.ctrl{display:flex;flex-wrap:wrap;gap:10px;}\
.ctrl button{background:var(--card2);color:var(--txt);border:1px solid var(--line);border-radius:8px;padding:10px 18px;font-size:14px;cursor:pointer;}\
.ctrl button:hover{background:var(--acc);color:#000;border-color:var(--acc);}\
.foot{color:var(--mut);font-size:12px;text-align:center;padding:6px 0 2px;}\
</style>\
<script>\
function polar(cx,cy,r,deg){var a=(deg-90)*Math.PI/180;return[cx+r*Math.cos(a),cy+r*Math.sin(a)];}\
function gArc(g,cx,cy,r,a0,a1,col,w){var NS='http://www.w3.org/2000/svg';var p0=polar(cx,cy,r,a0),p1=polar(cx,cy,r,a1);var large=(a1-a0)>180?1:0;var pa=document.createElementNS(NS,'path');pa.setAttribute('d','M'+p0[0].toFixed(1)+' '+p0[1].toFixed(1)+' A'+r+' '+r+' 0 '+large+' 1 '+p1[0].toFixed(1)+' '+p1[1].toFixed(1));pa.setAttribute('fill','none');pa.setAttribute('stroke',col);pa.setAttribute('stroke-width',w);pa.setAttribute('stroke-linecap','round');g.appendChild(pa);}\
function gTxt(g,x,y,s,col,sz,w){var NS='http://www.w3.org/2000/svg';var t=document.createElementNS(NS,'text');t.setAttribute('x',x);t.setAttribute('y',y);t.setAttribute('text-anchor','middle');t.setAttribute('fill',col);t.setAttribute('font-size',sz);if(w)t.setAttribute('font-weight',w);t.textContent=s;g.appendChild(t);}\
function drawGauge(t,p){\
  var NS='http://www.w3.org/2000/svg';var g=document.getElementById('gauge');if(!g)return;\
  while(g.firstChild)g.removeChild(g.firstChild);\
  var cx=130,cy=130,A0=225,SW=270,deg=String.fromCharCode(176);\
  for(var i=0;i<=72;i++){var ang=A0+(i*SW/72);if(ang>360)ang-=360;var lng=(i%9==0);var r1=lng?110:115,r2=120;var pa=polar(cx,cy,r1,ang),pb=polar(cx,cy,r2,ang);var ln=document.createElementNS(NS,'line');ln.setAttribute('x1',pa[0].toFixed(1));ln.setAttribute('y1',pa[1].toFixed(1));ln.setAttribute('x2',pb[0].toFixed(1));ln.setAttribute('y2',pb[1].toFixed(1));ln.setAttribute('stroke','#3a4048');ln.setAttribute('stroke-width',lng?2:1);g.appendChild(ln);}\
  if(isNaN(t))t=0;if(isNaN(p))p=1013;\
  var tmin=-10,tmax=45,tf=Math.max(0,Math.min(1,(t-tmin)/(tmax-tmin)));\
  gArc(g,cx,cy,98,A0,A0+SW,'#262c34',9);\
  if(tf>0)gArc(g,cx,cy,98,A0,A0+SW*tf,'#FFB000',9);\
  var pmin=980,pmax=1040,pf=Math.max(0,Math.min(1,(p-pmin)/(pmax-pmin)));\
  gArc(g,cx,cy,80,A0,A0+SW,'#262c34',7);\
  if(pf>0)gArc(g,cx,cy,80,A0,A0+SW*pf,'#4FC3F7',7);\
  gTxt(g,cx,cy-14,'TEMPERATURE','#8a9099',10,600);\
  gTxt(g,cx,cy+14,t.toFixed(2)+deg+'C','#FFB000',30,600);\
  gTxt(g,cx,cy+38,p.toFixed(1)+' hPa','#4FC3F7',15,600);\
}\
function upDate(){var el=document.getElementById('today');if(el)el.innerHTML=new Date().toLocaleDateString('en-US',{weekday:'long',year:'numeric',month:'long',day:'numeric'});}\
function getData(){var x=new XMLHttpRequest();x.onreadystatechange=function(){if(this.readyState==4&&this.status==200){var r=this.responseText.split(',');\
document.getElementById('clock').innerHTML=r[0];\
document.getElementById('temp').innerHTML=r[1];\
document.getElementById('pres').innerHTML=r[2];\
document.getElementById('rhum').innerHTML=r[3];\
document.getElementById('dewp').innerHTML=r[4];\
document.getElementById('hind').innerHTML=r[5];\
document.getElementById('nexttx').innerHTML=r[6];\
document.getElementById('rssi').innerHTML=r[7];\
document.getElementById('uptime').innerHTML=r[8];\
drawGauge(parseFloat(r[1]),parseFloat(r[2]));\
}};x.open('GET','jquery',true);x.send();}\
window.addEventListener('load',function(){upDate();getData();setInterval(getData,1000);});\
</script>\
</head>\
<body>\
<div class='wrap'>\
  <div class='top'>\
    <div class='brand'>{{callsign}} Weather Server<small>{{stationcomment}} &middot; Lat {{lat}} &middot; Long {{long}} &middot; Alt {{alt}} m &middot; {{SOFT_VER}} &middot; <a href='https://www.southernmanrobbie.com' target='_blank'>www.southernmanrobbie.com</a></small></div>\
    <div class='clock'><div class='t' id='clock'>{{time}}</div><div class='d' id='today'></div></div>\
  </div>\
\
  <div class='sec'>\
    <div class='sec-h'><span>Current weather conditions</span><span>{{ChipModel}}</span></div>\
    <div class='row2'>\
      <div class='grid'>\
        <div class='card'><div class='lab'>Temperature (&deg;C)</div><div class='val'><span id='temp'>{{degC}}</span></div></div>\
        <div class='card'><div class='lab'>Dew point (&deg;C)</div><div class='val'><span id='dewp'>{{DPdegC}}</span></div></div>\
        <div class='card'><div class='lab'>Humidity (%)</div><div class='val'><span id='rhum'>{{rHum}}</span></div></div>\
        <div class='card'><div class='lab'>Pressure (hPa)</div><div class='val'><span id='pres'>{{mbar}}</span></div></div>\
        <div class='card'><div class='lab'>Heat Index (&deg;C)</div><div class='val'><span id='hind'>{{HIdegC}}</span></div></div>\
      </div>\
      <div class='card gauge'><svg id='gauge' viewBox='0 0 260 260' style='width:100%;max-width:280px;height:auto'></svg></div>\
    </div>\
  </div>\
\
  <div class='sec'>\
    <div class='sec-h'><span>History &mdash; sampling every 2.5 min, up to ~4 h</span></div>\
    <div class='charts'>\
      <div class='chart-card'><div class='ct'>Temperature (&deg;C)</div><svg id='svgChart_temp' viewBox='0 0 760 240'></svg></div>\
      <div class='chart-card'><div class='ct'>Pressure (hPa)</div><svg id='svgChart_pres' viewBox='0 0 760 240'></svg></div>\
      <div class='chart-card'><div class='ct'>Relative Humidity (%)</div><svg id='svgChart_rhum' viewBox='0 0 760 240'></svg></div>\
      <div class='chart-card'><div class='ct'>WiFi RSSI (dBm)</div><svg id='svgChart_rssi' viewBox='0 0 760 240'></svg></div>\
    </div>\
  </div>\
\
  <div class='sec'>\
    <div class='sec-h'><span>System info</span></div>\
    <div class='info'>\
      <div class='i'><b>UPTIME</b><span id='uptime'>{{uptime}}</span></div>\
      <div class='i'><b>NEXT TX</b><span id='nexttx'>{{nexttx}}</span></div>\
      <div class='i'><b>SSID</b><span>{{SSID}}</span></div>\
      <div class='i'><b>RSSI</b><span id='rssi'>{{RSSI}}</span> dBm</div>\
      <div class='i'><b>BSSID</b><span>{{BSSID}}</span></div>\
      <div class='i'><b>IP ADDRESS</b><span>{{myip}}</span></div>\
      <div class='i'><b>POSITION</b><span>Lat {{lat}}<br>Long {{long}}<br>Alt {{alt}} m</span></div>\
    </div>\
  </div>\
\
  <div class='sec'>\
    <div class='sec-h'><span>Control panel</span></div>\
    <div class='ctrl'>\
      <form action='/settings' method='POST' style='margin:0'><button name='MiniWXSettings' value='true'>Settings</button></form>\
      <form action='/submit' method='POST' style='margin:0'><button name='SendAPRS' value='true'>Send APRS Packet</button></form>\
      <form action='/submit' method='POST' style='margin:0'><button name='NTPSync' value='true'>NTP Sync</button></form>\
      <form action='/submit' method='POST' style='margin:0'><button name='WiFiScan' value='true'>WiFi Scan</button></form>\
      <form action='/submit' method='POST' style='margin:0'><button name='Reboot' value='true'>Reboot</button></form>\
    </div>\
  </div>\
\
  <div class='foot'>MiniWX Server&#8482; ({{SOFT_VER}}) &middot; Developed by YO7ZRO</div>\
</div>\
{{graphscript}}\
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
:root{--bg:#0d0f12;--card:#181b20;--card2:#1f232a;--acc:#FFB000;--txt:#e8e8e8;--mut:#8a9099;--line:#2a2f37;}\
*{box-sizing:border-box;}\
body{margin:0 auto;background:var(--bg);color:var(--txt);font-family:'Segoe UI',Arial,sans-serif;font-size:15px;padding:16px;max-width:1000px;}\
h2,h3{color:var(--acc);}\
a{color:var(--acc);text-decoration:none;}\
.divHeaderTable{width:100%;padding-bottom:5px;display:block;}\
.divHeaderRow{width:100%;display:block;}\
.divHeaderColumn{float:left;width:33%;display:block;}\
.divTable{width:100%;display:block;padding:8px 0;}\
.divRow{width:100%;display:flex;flex-wrap:wrap;gap:14px;align-items:flex-start;padding-bottom:6px;}\
.divColumn{flex:1 1 300px;box-sizing:border-box;min-width:280px;}\
label{font-size:14px;color:var(--mut);}\
input[type=text],input[type=number],select,textarea{padding:8px;border:1px solid var(--line);border-radius:6px;background:var(--card2);color:var(--txt);font-size:14px;box-shadow:none;}\
fieldset{border:1px solid var(--line);border-radius:10px;box-shadow:none;background:var(--card);margin-bottom:14px;padding:14px 16px;}\
legend{background:transparent;color:var(--acc);text-shadow:none;padding:0 6px;}\
legend h2{font-size:16px;font-weight:600;margin:6px 0;}\
button,.butn{background:var(--card2);color:var(--txt);border:1px solid var(--line);border-radius:8px;padding:9px 18px;font-size:14px;cursor:pointer;box-shadow:none;text-shadow:none;}\
button:hover,.butn:hover{background:var(--acc);color:#000;border-color:var(--acc);}\
.butn:disabled{background:#333;color:#888;}\
</style>\
<body>\
  <div id='form_container'>\
  <hr>\
    <h1><a>MiniWX Station&#8482; Settings - {{SOFT_VER}}</a></h1>\
    <form action='/submit' method='POST'>\
        <div class='divTable'>\
          <div class='divRow'>\
            <div class='divColumn' style='width:99%'>\
              <fieldset>\
              <legend><h2> Station Callsign and Position </h2></legend>\
                <label>Callsign</label>\
                <div><input id='callsign' name='callsign' type='text' maxlength='9' size='9' value='{{callsign}}'/></div>\
                <div style='margin-top:10px;'></div>\
                <label style='display:inline-block;margin-top:10px;'>Latitude</label>\
                <input id='latitude' name='latitude' type='text' maxlength='8' size='8' value='{{lat}}'/>\
                <label> Longitude</label>\
                <input id='longitude' name='longitude' type='text' maxlength='9' size='9' value='{{lon}}'/>\
                <p style='margin:4px 0;border-top:1px solid #333;'></p>\
                <label style='color:#FFB000;font-weight:bold;'>DD Coordinates (Decimal Degrees &#8594; APRS)</label><br>\
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
                <label style='display:inline-block;margin-top:10px;'> Altitude (meters)</label>\
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
              <fieldset>\
              <legend><h2>NTP Settings</h2></legend>\
                <label>Server</label>\
                <div><input id='ntpserver' name='ntpserver'  type='text' maxlength='255' value='{{ntpserver}}'/></div>\
                <label>NTPSync Delay (hours)</label>\
                <div><input id='ntpsyncdelay' name='ntpsyncdelay'  type='text' maxlength='2' size='2' value='{{ntpsyncdelay}}'/></div>\
                <label>UTC Offset (hours)</label>\
                <div><input id='timezone' name='timezone' type='text' maxlength='5' size='5' value='{{timezone}}' placeholder='e.g. 3 or -5'/></div>\
              </fieldset>\
              <fieldset>\
              <legend><h2>GPIO Setup</h2></legend>\
                <label>BME280 SDA pin (GPIO)</label>\
                <div><input id='sdapin' name='sdapin' type='text' maxlength='2' size='3' value='{{sdapin}}'/></div>\
                <label>BME280 SCL pin (GPIO)</label>\
                <div><input id='sclpin' name='sclpin' type='text' maxlength='2' size='3' value='{{sclpin}}'/></div>\
                <p style='color:#8a9099;font-size:12px;line-height:1.5'>Enter the raw GPIO number. Board pin equivalents:<br>NodeMCU / Wemos D1: D0=16, D1=5, D2=4, D3=0, D4=2, D5=14, D6=12, D7=13, D8=15 (default: SDA=4, SCL=5).<br>ESP-01: GPIO0, GPIO2. Bare ESP-12: use any free GPIO.</p>\
              </fieldset>\
            </div>\
          </div>\
          <div class='divRow'><div class='divColumn'></br></br></br></div></div>\
          <div class='divRow'>\
            <div class='divColumn' style='width:99%;padding-bottom:10px'>\
              <div>\
                <button name='save' type='submit' value='true'>Save &amp; Exit</button>\
                <button type='button' onclick='window.history.go(-1);'>Exit</button>\
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
const char SOFT_VER[] = "v1.9.0 Mini WX Server";
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
#define SDA_PIN_DEF 4    // default BME280 I2C SDA (GPIO4 / D2)
#define SCL_PIN_DEF 5    // default BME280 I2C SCL (GPIO5 / D1)

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
  int sdaPin;               // BME280 I2C SDA GPIO - web-configurable
  int sclPin;               // BME280 I2C SCL GPIO - web-configurable
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
  sets.sdaPin      = SDA_PIN_DEF;
  sets.sclPin      = SCL_PIN_DEF;

  if (SPIFFS.exists("/settings.txt") == 0) {
    // absolutely 'first time use', write the defaults values
    Serial.println("^:WARNING:^ no settings config found, write defaults");
    writeSettingsFile();
  }
  else {
    //read the saved settings
    readSettingsFile();
  }
  Wire.begin(sets.sdaPin, sets.sclPin);   // re-init I2C on the configured BME280 pins

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
  page.replace(F("{{stationcomment}}"), sets.APRS_CMNT);
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
  // ---- inline history graphs on the main page (graphData + drawing script) ----
  String gd = F("<script> let graphData = [");
  for (sd_it = sd_vec.begin(); sd_it < sd_vec.end(); sd_it++) {
    gd += "{ temp:" + String(sd_it->temp, 2) + ",";
    gd += "  pres:" + String(sd_it->pres, 2) + ",";
    gd += "  rhum:" + String(sd_it->rhum, 2) + ",";
    gd += "  rssi:" + String(sd_it->rssi) + "},";
  }
  gd += F("]</script>");
  gd += FPSTR(HTTP_SVG_DRAW_INTERACTIVE_GRAPHS);
  page.replace(F("{{graphscript}}"), gd);

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
        stations += "<td style='text-align:left'><button type='button' onclick='pickSsid(this)' style='background:none;border:0;color:#FFB000;cursor:pointer;font-size:20px;text-decoration:underline;padding:0'>";
        stations += WiFi.SSID(i);
        stations += "</button></td><td>";
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

      String scanW = F("52%");
      if (server.hasHeader("User-Agent") && server.header("User-Agent").indexOf("Android") > 0) scanW = F("88%");
      message += "<fieldset style='width:" + scanW + "'>";

      message += F("<legend style='font-size: 18px;'>MiniWX&#8482; scanning found ");
      message += String(i);
      message += F(" networks </legend>");
      message += stations;
      message += F("</tbody></table></div></fieldset>");
      message += "<fieldset style='width:" + scanW + ";margin-top:14px'>";
      message += F("<legend style='font-size:18px;'>Connect to a network</legend>"
                   "<form action='/wifisave' method='POST'>"
                   "<p style='color:#8a9099'>Click an SSID above (or type it), enter the password, then Connect.</p>"
                   "<label>SSID</label><br><input id='selssid' name='ssid' type='text' maxlength='32' size='24' style='padding:8px;border:1px solid #2a2f37;border-radius:6px;background:#1f232a;color:#e8e8e8'/><br>"
                   "<label>Password</label><br><input name='pass' type='password' maxlength='63' size='24' style='padding:8px;border:1px solid #2a2f37;border-radius:6px;background:#1f232a;color:#e8e8e8'/><br><br>"
                   "<button type='submit'>Connect &amp; Reboot</button>"
                   "</form></fieldset>"
                   "<script>function pickSsid(b){document.getElementById('selssid').value=b.textContent;}</script>");
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
        if (server.argName(i) == F("sdapin"))       sets.sdaPin    = server.arg(i).toInt();
        if (server.argName(i) == F("sclpin"))       sets.sclPin    = server.arg(i).toInt();

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
  page.replace(F("{{sdapin}}"), String(sets.sdaPin));
  page.replace(F("{{sclpin}}"), String(sets.sclPin));

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
         "body{font-family:'Segoe UI',Arial,sans-serif;margin:18px auto;max-width:480px;padding:0 14px;background:#0d0f12;color:#e8e8e8}"
         "h2{margin:0 0 14px;color:#FFB000}label{font-size:14px;color:#8a9099}"
         "input,select{width:100%;padding:10px;margin:5px 0 14px;box-sizing:border-box;"
         "border:1px solid #2a2f37;border-radius:6px;font-size:15px;background:#1f232a;color:#e8e8e8}"
         "button{width:100%;padding:12px;border:0;border-radius:8px;background:#FFB000;color:#000;font-size:16px;font-weight:600;cursor:pointer}"
         ".muted{color:#8a9099;font-size:13px}a{color:#FFB000}</style></head><body>");
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
        "<body style='font-family:\"Segoe UI\",Arial,sans-serif;background:#0d0f12;color:#e8e8e8;margin:18px auto;max-width:480px;padding:0 14px'>"
        "<h3 style='color:#FFB000'>Saved. Rebooting and connecting...</h3>"
        "<p style='color:#8a9099'>If the network is correct, the board appears on the network within a few seconds.</p></body></html>"));
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