# MiniWX Server

<img width="1801" height="1869" alt="Git" src="https://github.com/user-attachments/assets/533a47f4-2993-4db5-9997-5f287162e472" />
<img width="647" height="540" alt="image" src="https://github.com/user-attachments/assets/ea53e389-1e34-41be-83f3-b85b0e7a6980" />


An ESP8266-based amateur-radio **weather server** with a self-contained web interface, live APRS-IS reporting, NTP-synchronised clock and built-in history graphs — no external libraries loaded from the internet, no cloud account, everything served straight from the device.

> **Version v1.7.2** — Originally **inspired by** the [MiniWX project by IU5HKU](https://github.com/IU5HKU/MiniWXStation). The firmware has since been substantially rewritten and is developed and maintained by **YO7ZRO**.

---

## Table of contents

1. [What it does](#what-it-does)
2. [Hardware](#hardware)
3. [Flashing the pre-built firmware (.bin)](#flashing-the-pre-built-firmware-bin)
4. [Building and flashing (PlatformIO)](#building-and-flashing-platformio)
5. [First start — WiFi setup](#first-start--wifi-setup)
6. [The web interface](#the-web-interface)
7. [Settings page](#settings-page)
8. [History graphs — how they work](#history-graphs--how-they-work)
9. [APRS reporting](#aprs-reporting)
10. [Tips & troubleshooting](#tips--troubleshooting)
11. [Credits & licence](#credits--licence)

---

## What it does

- Reads temperature, pressure and humidity from a **BME280** sensor.
- Calculates **dew point** and **heat index**.
- Serves a complete **web dashboard** (live values, system info, control panel).
- Plots **history graphs** (temperature, pressure, humidity, WiFi RSSI) rendered entirely on-device with no external JavaScript libraries.
- Sends **APRS-IS** weather packets so the station appears on aprs.fi and feeds APRS weather displays/receivers.
- Keeps the clock in sync over **NTP**, with a user-configurable UTC offset.
- Is configured **entirely from the browser** — no recompilation needed for day-to-day settings.

The interface language is **English only**, in an amber-on-black theme.

---

## Hardware

| Part | Notes |
|------|-------|
| ESP8266 board | ESP-12E / ESP-12F (e.g. Wemos D1 mini, NodeMCU). The build targets `esp12e`. |
| BME280 sensor | I2C, for temperature / pressure / humidity. |
| Power | Stable supply. Mount the sensor away from the board's own heat. |

The BME280 is read in **forced mode** and only sampled on demand (see *Sensor Read Interval*), which avoids the self-heating that continuous-mode sensors suffer from.

---

## Flashing the pre-built firmware (.bin)

If you just want to run the station without compiling anything, download the ready-made firmware from the project's **Releases** page on GitHub and flash it to a blank ESP8266. The `.bin` already contains everything — you do **not** need PlatformIO or the source code for this.

### What you need

- An **ESP8266** board (ESP-12E / ESP-12F, NodeMCU, Wemos D1 mini …).
- A **USB cable** — and, for bare ESP-12 modules, a USB-to-serial (TTL) adapter.
- A flashing tool: the command-line **esptool**, or a GUI such as **NodeMCU PyFlasher** (Windows/macOS/Linux) or the **Espressif Flash Download Tool** (Windows).
- The **`.bin`** file downloaded from the Releases page.

### Find the serial port

- **Windows:** `COM3`, `COM4`, … (Device Manager → Ports).
- **Linux:** `/dev/ttyUSB0` (you may need to be in the `dialout` group).
- **macOS:** `/dev/cu.usbserial-XXXX` or `/dev/cu.wchusbserial-XXXX`.

> Bare ESP-12 modules must be put into **flash mode**: hold **GPIO0 to GND** while powering up or pressing reset. Dev boards (NodeMCU, D1 mini) do this automatically.

### Option A — esptool (command line, recommended)

Install once:

```bash
pip install esptool
```

Erase the chip (recommended for a clean install on a new device), then write the firmware at flash address `0x0`:

```bash
esptool.py --chip esp8266 --port <PORT> erase_flash
esptool.py --chip esp8266 --port <PORT> --baud 460800 write_flash --flash_size detect 0x0 <firmware>.bin
```

Replace `<PORT>` with your serial port (e.g. `COM3`, `/dev/ttyUSB0`) and `<firmware>.bin` with the downloaded file. If `460800` baud is unstable, drop to `115200`.

### Option B — NodeMCU PyFlasher (GUI)

1. Open NodeMCU PyFlasher.
2. Select the **serial port** and the **`.bin`** file.
3. Baud rate `115200`, Flash mode **DIO**, **Erase flash: yes** (for a clean new device).
4. Click **Flash NodeMCU** and wait until it finishes.

### After flashing

Power-cycle the board. A freshly flashed device boots into the **WiFi setup portal** (`MiniWX-Setup-XXXX`) — follow [First start — WiFi setup](#first-start--wifi-setup) to join it to your network, then open the dashboard and configure it from the [Settings page](#settings-page).

> **Updating an existing station** is even easier: you don't need to re-flash over USB. Build a firmware `.bin` (or use the Release file) and upload it through the station's built-in **OTA** update, over WiFi.

---

## Building and flashing (PlatformIO)

The project is a standard **PlatformIO** project.

```
MiniWX_PIO/
├── platformio.ini
└── src/
    ├── MiniWX_v1.1g.cpp      # main firmware
    ├── EN_Locale.h           # all web-page templates (English)
    └── SystemWebpages.h      # shared web fragments + graph engine
```

Dependencies (declared in `platformio.ini`, fetched automatically):

- `SparkFun BME280` library
- `NTPtimeESP` (NTP client)
- `DNSServer` (part of the ESP8266 core — used for the captive setup portal)

Build & upload:

```bash
pio run -t upload          # compile + flash
pio device monitor         # serial monitor (74880 baud at boot)
```

The filesystem is **SPIFFS**; on first boot the firmware will auto-format it if needed.

---

## First start — WiFi setup

The station has **no hard-coded WiFi credentials**. On first boot (or whenever it cannot join the saved network):

1. The device starts an open access point named **`MiniWX-Setup-XXXX`** (where `XXXX` is part of the chip ID, in hex).
2. Connect your phone/laptop to that AP. A **captive portal** opens automatically (if not, browse to `http://192.168.4.1`).
3. Enter your **WiFi SSID and password** and save.
4. The station reboots and joins your network using **DHCP** by default.

If the station cannot connect within **25 seconds**, it falls back to the setup AP again so you are never locked out.

> Once connected, find the station's IP address from your router, or from the **IP addr** field shown on the dashboard. Open `http://<station-ip>/` in a browser.

---

## The web interface

### Main page

The header shows your **callsign** and a **live clock**:

```
YO7ZRO-13 Weather Server  -  23:00:29
```

Below it, three blocks:

**1. Live measurements**

| Parameter | Unit |
|-----------|------|
| Temperature | °C |
| Pressure | mBar |
| Relative Humidity | % |
| Dew point | °C |
| Heat Index | °C |

**2. System Infos**

- **Uptime** — how long the station has been running
- **Next TX** — countdown to the next APRS transmission
- **SSID / RSSI / BSSID** — WiFi connection details
- **IP addr** — the station's address on your network
- **Position** — current latitude / longitude / altitude

The page footer shows the firmware version (currently **v1.7.2**). The version is defined once in the firmware as a single `SOFT_VER` constant and inserted into the page dynamically (the footer reads `MiniWX Server™ (v1.7.2) … Developed by YO7ZRO`), so a single edit updates it everywhere it appears — the main page, the settings page and the APRS packet comment.

**3. Control Panel**

| Button | Action |
|--------|--------|
| **Graphics Trends** | Opens the history graphs page (`/graphs`). |
| **Settings** | Opens the configuration page (`/settings`). |
| **Send APRS Packet** | Forces an immediate APRS-IS weather transmission. |
| **NTP Sync** | Forces an immediate clock resync over NTP. |
| **WiFi Scan** | Lists nearby networks. |
| **Reboot** | Restarts the station. |

The values refresh automatically; the page does not need to be reloaded to see new readings.

---

## Settings page

Open it with the **Settings** button. After changing any field, submit the form to save — values are stored in flash and survive reboots. Some changes (static IP) require a reboot, as noted on the page.

### Station Callsign and Position

| Field | Description |
|-------|-------------|
| **Callsign** | Your station callsign (e.g. `YO7ZRO-13`). |
| **Longitude** | In **APRS format**, e.g. `01023.16E` (DDDMM.mmE/W). |
| **Latitude** | In **APRS format**, e.g. `4303.01N` (DDMM.mmN/S). |
| **DD Latitude / DD Longitude** | Optional helper fields in **decimal degrees**. |
| **Altitude (meters)** | Station altitude above sea level. |

**Decimal-degree converter.** If you know your position only in decimal degrees, type it into **DD Latitude** / **DD Longitude** and press **Convert DD → APRS**. The Longitude / Latitude fields above are filled automatically in the correct APRS format. When the page opens, these DD fields are also pre-filled by converting the current APRS values back to decimal degrees, so you can always see both.

> Example: `44.4882`, `26.0343` → `4429.29N`, `02602.06E`.

### APRS & Sensor settings

| Field | Description |
|-------|-------------|
| **Server Address** | APRS-IS server. Default **`rotate.aprs.net`**. |
| **Password** | Your APRS-IS passcode (derived from your callsign). |
| **Server Port** | APRS-IS port. Default **`14580`**. |
| **Transmission Delay (min)** | How often the station transmits its weather packet. |
| **Sensor Read Interval (s)** | How often the BME280 is actually measured (forced mode). Cached values are served between reads. |
| **Temp. Offset BME280 (deg C)** | Correction added to the raw temperature reading — use e.g. `-2.0` if the sensor reads high. |
| **Station Packets Comment** | Free-text comment attached to position packets. |
| **Telemetry Packets Comment** | Free-text comment attached to telemetry packets. |

A **Reset APRS to defaults** button restores the server address (`rotate.aprs.net`) and port (`14580`) without touching your callsign or passcode. (It only fills the fields — press save afterwards.)

### Static IP definitions

By default the station uses **DHCP**. If you prefer a fixed address:

| Field | Description |
|-------|-------------|
| **USE STATIC IP (reboot needed)** | Enable static addressing. |
| **IP / Gateway / Subnet Mask** | Standard network parameters. |
| **DNS1 / DNS2** | DNS servers. |

After enabling static IP, **reboot** for it to take effect.

### NTP settings

| Field | Description |
|-------|-------------|
| **Server** | NTP server hostname (e.g. `ntp1.inrim.it`, `pool.ntp.org`). |
| **NTPSync Delay (hours)** | How often the clock resynchronises. |
| **UTC Offset (hours)** | Your timezone offset from UTC. Set it to the value that gives correct local time. |

> **About the UTC offset:** there is **no automatic daylight-saving adjustment** — you set the full offset yourself. For Romania, use `3` in summer (EEST) and `2` in winter (EET). Fractional offsets such as `5.5` are supported.

---

## History graphs — how they work

Open with **Graphics Trends**. The page header explains the behaviour, and four charts are drawn:

| Chart | Colour | Unit |
|-------|--------|------|
| Temperature | amber | °C |
| Pressure | cyan | hPa |
| Relative Humidity | green | % |
| WiFi RSSI | red | dBm |

**Sampling.** The station records one sample of every value **every 2.5 minutes**, keeping a rolling buffer of the most recent **96 samples** — about **4 hours** of history.

**Dynamic window.** The time axis is not fixed: it spans exactly from the oldest stored sample to *now*, growing as data accumulates and capped at ~4 hours. With only two samples (about five minutes of uptime) you already see an active two-point graph with correct labels.

**Refresh.** The graphs are regenerated from the latest data **every time you open the page** — just reload `/graphs` to see the newest readings.

**Axes & labels.**

- **Y-axis (vertical):** auto-scales to the current data range, with numeric tick labels and the unit shown on the top tick.
- **X-axis (horizontal):** elapsed-time labels (`now`, `15m ago`, `1h 35m ago`, …). The number of labels adapts to how many samples exist, so they never overlap or repeat.
- The most recent reading is marked with a dot and its value, at the right edge.

**No internet required.** The graphs are rendered entirely on-device using plain SVG — there are no external chart libraries to download, so they work even on a LAN with no internet access.

---

## APRS reporting

When configured with a valid callsign, position and passcode, the station logs in to APRS-IS and periodically transmits a standard **weather packet** (temperature, humidity, pressure). This is what makes the station visible on services like **aprs.fi** and lets it feed APRS weather **receivers/displays**.

- Transmission frequency is set by **Transmission Delay (min)**.
- Press **Send APRS Packet** on the main page to transmit immediately.
- The **Next TX** field on the dashboard shows the countdown to the next automatic transmission.
- The firmware version (`v1.7.2`) is appended to the position-packet comment, e.g. `… your comment (v1.7.2)`.

---

## Tips & troubleshooting

- **Temperature reads a bit high.** Sensors close to electronics pick up residual heat. Set **Temp. Offset BME280** to the measured difference (e.g. `-2.0`) against a reference thermometer at thermal equilibrium.
- **Clock is one hour off.** Adjust the **UTC Offset** (there is no automatic DST). See the NTP section above.
- **Graphs are empty right after boot.** The first sample is taken 2.5 minutes after start; the graph becomes active from the second sample (~5 minutes).
- **Can't reach the station.** Check the IP on your router. If WiFi failed, the `MiniWX-Setup-XXXX` AP will be available for reconfiguration.
- **APRS not showing on aprs.fi.** Verify callsign, passcode, server address/port and that your position is in valid APRS format.

---

## Credits & licence

- This project was **inspired by** the MiniWX Station project by IU5HKU (<https://github.com/IU5HKU/MiniWXStation>). The current firmware has been substantially rewritten and reworked — including a PlatformIO port, English-only UI, web-configurable temperature offset / read interval / UTC offset, a decimal-degree coordinate converter, self-contained on-device graphs, an APRS defaults reset, a dynamic version string and an amber theme.
- Developed and maintained by **YO7ZRO**.

The project that inspired this work is published under the GNU GPL; if you redistribute this firmware, please keep the licence and attribution accordingly.

73 de **YO7ZRO**
