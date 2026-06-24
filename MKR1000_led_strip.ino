#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_NeoPixel.h>

#define SECRET_SSID "LED_Strip"
#define SECRET_PASS ""

// LED Strip Configuration
#define LED_PIN 6         
#define NUM_LEDS 30        
#define BRIGHTNESS 50      

char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;    
int keyIndex = 0;               

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// Initialize LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Variables for LED strip color control
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server - LED Strip Controller");

  // Initialize LED strip
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show(); // Initialize all pixels to 'off'

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }

  delay(10000);

  server.begin();
  printWiFiStatus();
  
  // Keep LEDs off after boot
  for(int i=0; i<NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

void loop() {
  if (status != WiFi.status()) {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];

      // a device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
      
      // Restore previous color
      setLEDColor(redValue, greenValue, blueValue);
    } else {
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("new client");           
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {          
        char c = client.read();            
        Serial.write(c);                  
        if (c == '\n') {                   
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // HTML page with color picker and color control
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta charset='UTF-8'>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
            client.println("<title>LED Strip Controller</title>");
            client.println("<style>");
            client.println("* { margin: 0; padding: 0; box-sizing: border-box; }");
            client.println("body { font-family: 'Segoe UI', Arial, sans-serif; text-align: center; min-height: 100vh; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); display: flex; justify-content: center; align-items: center; padding: 20px; }");
            client.println(".container { background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; max-width: 450px; width: 100%; box-shadow: 0 20px 60px rgba(0,0,0,0.3); backdrop-filter: blur(10px); }");
            client.println("h1 { color: #333; margin-bottom: 20px; font-size: 28px; display: flex; align-items: center; justify-content: center; gap: 10px; }");
            client.println("h1 span { background: linear-gradient(135deg, #667eea, #764ba2); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }");
            client.println(".color-preview { width: 150px; height: 150px; border-radius: 50%; margin: 20px auto; border: 4px solid #333; box-shadow: 0 8px 25px rgba(0,0,0,0.2); transition: all 0.3s ease; }");
            client.println(".control-group { margin: 20px 0; }");
            client.println(".control-group label { display: block; font-weight: 600; margin-bottom: 5px; color: #555; }");
            client.println(".control-group .value { display: inline-block; font-weight: 700; color: #333; min-width: 30px; }");
            client.println("input[type='range'] { width: 100%; height: 8px; -webkit-appearance: none; background: linear-gradient(to right, #ddd, #ddd); border-radius: 5px; outline: none; margin: 10px 0; }");
            client.println("input[type='range']::-webkit-slider-thumb { -webkit-appearance: none; width: 20px; height: 20px; border-radius: 50%; background: #667eea; cursor: pointer; box-shadow: 0 2px 8px rgba(102,126,234,0.4); }");
            client.println(".red-slider::-webkit-slider-thumb { background: #ff4444; }");
            client.println(".green-slider::-webkit-slider-thumb { background: #44ff44; }");
            client.println(".blue-slider::-webkit-slider-thumb { background: #4444ff; }");
            client.println(".preset-colors { display: grid; grid-template-columns: repeat(4, 1fr); gap: 8px; margin: 20px 0; }");
            client.println(".preset-btn { height: 40px; border-radius: 10px; border: 2px solid #ddd; cursor: pointer; transition: all 0.2s ease; }");
            client.println(".preset-btn:hover { transform: scale(1.05); border-color: #667eea; box-shadow: 0 4px 12px rgba(0,0,0,0.2); }");
            client.println(".apply-btn { width: 100%; padding: 14px; background: linear-gradient(135deg, #667eea, #764ba2); color: white; border: none; border-radius: 12px; font-size: 18px; font-weight: 600; cursor: pointer; transition: all 0.3s ease; margin-top: 10px; }");
            client.println(".apply-btn:hover { transform: translateY(-2px); box-shadow: 0 8px 25px rgba(102,126,234,0.4); }");
            client.println(".apply-btn:active { transform: translateY(0); }");
            client.println("#statusMessage { margin-top: 15px; padding: 10px; border-radius: 8px; font-weight: 500; transition: all 0.3s ease; }");
            client.println(".status-success { background: #d4edda; color: #155724; }");
            client.println(".status-error { background: #f8d7da; color: #721c24; }");
            client.println(".color-values { display: flex; justify-content: center; gap: 20px; margin: 10px 0; font-size: 14px; color: #666; }");
            client.println(".color-values span { font-weight: 700; }");
            client.println(".rgb-red { color: #ff4444; }");
            client.println(".rgb-green { color: #44bb44; }");
            client.println(".rgb-blue { color: #4444ff; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<div class='container'>");
            //client.println("<h1> <span>LED Control</span></h1>");
            
            client.println("<div class='color-preview' id='colorPreview' style='background-color: rgb(0,0,0);'></div>");
            
            client.println("<div class='color-values'>");
            client.println("<span>R: <span class='rgb-red' id='redDisplay'>0</span></span>");
            client.println("<span>G: <span class='rgb-green' id='greenDisplay'>0</span></span>");
            client.println("<span>B: <span class='rgb-blue' id='blueDisplay'>0</span></span>");
            client.println("</div>");
            
            client.println("<div class='control-group'>");
            client.println("<label> Red <span class='value' id='redValue'>0</span></label>");
            client.println("<input type='range' class='red-slider' id='redSlider' min='0' max='255' value='0' oninput='updateColor()'>");
            client.println("</div>");
            
            client.println("<div class='control-group'>");
            client.println("<label> Green <span class='value' id='greenValue'>0</span></label>");
            client.println("<input type='range' class='green-slider' id='greenSlider' min='0' max='255' value='0' oninput='updateColor()'>");
            client.println("</div>");
            
            client.println("<div class='control-group'>");
            client.println("<label> Blue <span class='value' id='blueValue'>0</span></label>");
            client.println("<input type='range' class='blue-slider' id='blueSlider' min='0' max='255' value='0' oninput='updateColor()'>");
            client.println("</div>");
            
            client.println("<div class='preset-colors'>");
            client.println("<div class='preset-btn' style='background: #FF0000;' onclick='setColor(255,0,0)' title='Red'></div>");
            client.println("<div class='preset-btn' style='background: #00FF00;' onclick='setColor(0,255,0)' title='Green'></div>");
            client.println("<div class='preset-btn' style='background: #0000FF;' onclick='setColor(0,0,255)' title='Blue'></div>");
            client.println("<div class='preset-btn' style='background: #FFFF00;' onclick='setColor(255,255,0)' title='Yellow'></div>");
            client.println("<div class='preset-btn' style='background: #FF00FF;' onclick='setColor(255,0,255)' title='Magenta'></div>");
            client.println("<div class='preset-btn' style='background: #00FFFF;' onclick='setColor(0,255,255)' title='Cyan'></div>");
            client.println("<div class='preset-btn' style='background: #FFFFFF; border-color: #999;' onclick='setColor(255,255,255)' title='White'></div>");
            client.println("<div class='preset-btn' style='background: #000000;' onclick='setColor(0,0,0)' title='Off'></div>");
            client.println("</div>");
            
            client.println("<button class='apply-btn' onclick='sendColor()'>✨ Apply Color</button>");
            client.println("<div id='statusMessage'></div>");
            
            client.println("<script>");
            client.println("function updateColor() {");
            client.println("  var r = document.getElementById('redSlider').value;");
            client.println("  var g = document.getElementById('greenSlider').value;");
            client.println("  var b = document.getElementById('blueSlider').value;");
            client.println("  document.getElementById('redValue').textContent = r;");
            client.println("  document.getElementById('greenValue').textContent = g;");
            client.println("  document.getElementById('blueValue').textContent = b;");
            client.println("  document.getElementById('redDisplay').textContent = r;");
            client.println("  document.getElementById('greenDisplay').textContent = g;");
            client.println("  document.getElementById('blueDisplay').textContent = b;");
            client.println("  document.getElementById('colorPreview').style.backgroundColor = 'rgb(' + r + ',' + g + ',' + b + ')';");
            client.println("}");
            client.println("function setColor(r, g, b) {");
            client.println("  document.getElementById('redSlider').value = r;");
            client.println("  document.getElementById('greenSlider').value = g;");
            client.println("  document.getElementById('blueSlider').value = b;");
            client.println("  updateColor();");
            client.println("  sendColor(); // Auto-apply preset colors");
            client.println("}");
            client.println("function sendColor() {");
            client.println("  var r = document.getElementById('redSlider').value;");
            client.println("  var g = document.getElementById('greenSlider').value;");
            client.println("  var b = document.getElementById('blueSlider').value;");
            client.println("  var statusDiv = document.getElementById('statusMessage');");
            client.println("  statusDiv.textContent = 'Applying color...';");
            client.println("  statusDiv.className = '';");
            client.println("  var xhr = new XMLHttpRequest();");
            client.println("  xhr.open('GET', '/setcolor?r=' + r + '&g=' + g + '&b=' + b, true);");
            client.println("  xhr.onreadystatechange = function() {");
            client.println("    if (xhr.readyState == 4) {");
            client.println("      if (xhr.status == 200) {");
            client.println("        statusDiv.textContent = ' Color applied successfully!';");
            client.println("        statusDiv.className = 'status-success';");
            client.println("        setTimeout(function() { statusDiv.textContent = ''; statusDiv.className = ''; }, 2500);");
            client.println("      } else {");
            client.println("        statusDiv.textContent = 'Error applying color';");
            client.println("        statusDiv.className = 'status-error';");
            client.println("      }");
            client.println("    }");
            client.println("  };");
            client.println("  xhr.send();");
            client.println("}");
            client.println("// Initialize color on page load");
            client.println("updateColor();");
            client.println("</script>");
            
            client.println("</div>");
            client.println("</body>");
            client.println("</html>");
            break;
          }
          else {     
            currentLine = "";
          }
        }
        else if (c != '\r') {    
          currentLine += c;     
        }

        // Handle color setting requests
        if (currentLine.startsWith("GET /setcolor?")) {
          // Parse RGB values from URL
          int rStart = currentLine.indexOf("r=") + 2;
          int rEnd = currentLine.indexOf("&", rStart);
          int gStart = currentLine.indexOf("g=") + 2;
          int gEnd = currentLine.indexOf("&", gStart);
          int bStart = currentLine.indexOf("b=") + 2;
          int bEnd = currentLine.indexOf(" ", bStart);
          
          if (rStart > 1 && rEnd > rStart && gStart > 1 && gEnd > gStart && bStart > 1 && bEnd > bStart) {
            String rStr = currentLine.substring(rStart, rEnd);
            String gStr = currentLine.substring(gStart, gEnd);
            String bStr = currentLine.substring(bStart, bEnd);
            
            redValue = rStr.toInt();
            greenValue = gStr.toInt();
            blueValue = bStr.toInt();
            setLEDColor(redValue, greenValue, blueValue);
            
            Serial.print("Color set to RGB(");
            Serial.print(redValue);
            Serial.print(",");
            Serial.print(greenValue);
            Serial.print(",");
            Serial.print(blueValue);
            Serial.println(")");
            
            // Send response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/plain");
            client.println();
            client.println("OK");
          }
        }
      }
    }
    client.stop();
    Serial.println("client disconnected");
  }
}

void setLEDColor(int red, int green, int blue) {
  uint32_t color = strip.Color(red, green, blue);
  // Set all LEDs to the same color
  for(int i=0; i<NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  
  Serial.print("LED strip updated: ");
  Serial.print(NUM_LEDS);
  Serial.println(" LEDs");
}

void printWiFiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
  Serial.println("LED Strip Controller Ready!");
  Serial.print("Number of LEDs: ");
  Serial.println(NUM_LEDS);
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

