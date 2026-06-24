# Arduino-mkr1000 LED strip control

A web-based RGB LED strip controller built for the Arduino MKR1000. The board creates its own WiFi Access Point and hosts a browser UI that lets you pick any color for the strip in real time — no router or internet connection required.

--------------------------------------------------------------------------------
FEATURES
--------------------------------------------------------------------------------

  - WiFi Access Point mode  : MKR1000 acts as its own hotspot (no router needed)
  - Web UI                  : Responsive color-picker page served directly from
                              the board
  - RGB sliders             : Independent Red, Green, and Blue sliders (0–255)
  - Live color preview       : Circle on the page updates as you move sliders
  - Preset color buttons    : One-tap presets — Red, Green, Blue, Yellow,
                              Magenta, Cyan, White, and Off
  - Auto-apply presets      : Preset buttons apply color immediately without
                              needing to press "Apply"
  - State persistence       : Last color is restored when a client reconnects
  - Serial debug output     : Connection status, MAC addresses, and color values
                              printed to the Serial Monitor

--------------------------------------------------------------------------------
HARDWARE REQUIREMENTS
--------------------------------------------------------------------------------

  - Arduino MKR1000 (WiFi 101 chip required)
  - WS2812B (or compatible NeoPixel) LED strip
  - 5 V power supply rated for your strip length
    (Rule of thumb: 30 LEDs x 60 mA = ~1.8 A at full white)
  - 300–500 Ω resistor on the data line (recommended)
  - 1000 µF capacitor across the strip power rails (recommended)

Wiring
  MKR1000 Pin 6  -->  [300Ω resistor]  -->  LED strip DATA IN
  5V supply      -->  LED strip VCC
  GND (shared)   -->  LED strip GND + MKR1000 GND

<img width="989" height="425" alt="connection diagram" src="https://github.com/user-attachments/assets/22c483b0-49ee-48eb-af6a-fca436bc8977" />


<img width="3024" height="4032" alt="MKR 1000 WiFi _1" src="https://github.com/user-attachments/assets/25a79fd6-30b0-4832-87ba-559bf7f08a67" />

<img width="768" height="1024" alt="MKR 1000 WiFi with LED" src="https://github.com/user-attachments/assets/8198526e-771c-4d11-952f-a0a4096b0e92" />

--------------------------------------------------------------------------------
SOFTWARE / LIBRARY REQUIREMENTS
--------------------------------------------------------------------------------

  Arduino IDE 1.8+ (or Arduino IDE 2.x)

  Board Package:
    Arduino SAMD Boards (MKR series)
    Install via: Tools > Board > Boards Manager > search "Arduino SAMD"

  Libraries (install via Sketch > Include Library > Manage Libraries):
    - WiFi101           by Arduino          (v0.16.1 or later)
    - Adafruit NeoPixel by Adafruit         (v1.10.0 or later)
    - SPI               (built-in, no install needed)

--------------------------------------------------------------------------------
CONFIGURATION
--------------------------------------------------------------------------------

  Open MKR1000_led_strip.ino and edit the defines near the top:

    #define SECRET_SSID  "LED_Strip"   // WiFi network name broadcast by MKR1000
    #define SECRET_PASS  ""            // Leave empty for an open network
                                       // or set a WPA2 password

    #define LED_PIN      6             // Digital pin connected to strip data line
    #define NUM_LEDS     30            // Number of LEDs on your strip
    #define BRIGHTNESS   50            // Global brightness (0 = off, 255 = max)

  Adjust NUM_LEDS and BRIGHTNESS to match your hardware before uploading.

--------------------------------------------------------------------------------
HOW TO USE
--------------------------------------------------------------------------------

  1. Upload the sketch to your MKR1000 via Arduino IDE.

  2. Open the Serial Monitor (9600 baud) and wait for:
       "LED Strip Controller Ready!"
     The board prints its IP address at this point.

  3. On your phone or laptop, connect to the WiFi network named "LED_Strip"
     (open network, no password by default).

  4. Open a browser and navigate to:
       http://192.168.1.1
     (Check the Serial Monitor for the exact IP if it differs.)

  5. Use the web UI to control the strip:
       - Drag the Red / Green / Blue sliders to mix a color.
       - The color preview circle updates live.
       - Tap a preset button to jump to a common color instantly.
       - Press "Apply Color" to send a custom slider color to the strip.
<img width="1344" height="680" alt="webpage" src="https://github.com/user-attachments/assets/76332b02-40a0-40ea-890f-511a1fd02580" />

--------------------------------------------------------------------------------
PROJECT STRUCTURE
--------------------------------------------------------------------------------

  MKR1000_led_strip.ino   Main Arduino sketch (single-file project)

  Key sections inside the sketch:
    setup()               Initializes Serial, LED strip, and WiFi AP; starts
                          the HTTP server on port 80.
    loop()                Monitors AP client connections and handles incoming
                          HTTP requests.
    setLEDColor(r, g, b)  Sets all LEDs on the strip to the given RGB color.
    printWiFiStatus()     Prints SSID, IP, and RSSI to the Serial Monitor.
    printMacAddress()     Prints the MAC address of a connected device.

  HTTP endpoints:
    GET /                           Serves the full HTML color-picker page.
    GET /setcolor?r=&g=&b=    Sets strip color; responds with plain "OK".

--------------------------------------------------------------------------------
TROUBLESHOOTING
--------------------------------------------------------------------------------

  Problem                        Likely Cause / Fix
  ------------------------------ -----------------------------------------------
  "WiFi shield not present"      WiFi101 firmware may be outdated. Update using
                                 the "WiFi101 Firmware Updater" sketch.

  LEDs don't light up            Check wiring on Pin 6, verify shared GND, and
                                 confirm NUM_LEDS matches your strip length.

  Browser can't reach the board  Confirm you are connected to "LED_Strip" WiFi,
                                 not your regular router. Check Serial Monitor
                                 for the correct IP address.

  Colors look wrong              Some strips use GRB byte order. The sketch uses
                                 NEO_GRB — change to NEO_RGB in the
                                 Adafruit_NeoPixel constructor if needed.

  Strip flickers                 Add a 1000 µF capacitor across VCC/GND on the
                                 strip, and a 300 Ω resistor on the data line.
                                 
--------------------------------------------------------------------------------


video and images link 
https://drive.google.com/drive/folders/1Ttq-nWlZMDxtRtFysJdTIaGA4gzmSMWI?usp=sharing
