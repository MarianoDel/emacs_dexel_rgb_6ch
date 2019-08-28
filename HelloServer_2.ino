#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//--- This is the AP Side Conf
#include <DNSServer.h>

//--- This is the AP Side Conf
const char* softAP_ssid = "Kirno_RGB";

//--- This is the Client Side Conf
// const char* ssid = "Kirno_RGB";
// const char* password = "";


const char index_html [] PROGMEM = {"<!DOCTYPE html>\n<html>\n  <head>\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n    <style>\n      /* For the Slider Control */\n      .slidecontainer {\n      width: 100%;\n      }\n\n      .slider {\n      -webkit-appearance: none;\n      width: 100%;\n      height: 15px;\n      border-radius: 5px;\n      background: #d3d3d3;\n      outline: none;\n      opacity: 0.7;\n      -webkit-transition: .2s;\n      transition: opacity .2s;\n      }\n\n      .slider:hover {\n      opacity: 1;\n      }\n\n      .slider::-webkit-slider-thumb {\n      -webkit-appearance: none;\n      appearance: none;\n      width: 35px;\n      height: 35px;\n      border-radius: 50%;\n      background: #4CAF50;\n      cursor: pointer;\n      }\n\n      .sl1::-webkit-slider-thumb {\n      background: #FF0000;\n      }\n      .sl2::-webkit-slider-thumb {\n      background: #00FF00;\n      }\n      .sl3::-webkit-slider-thumb {\n      background: #0000FF;\n      }\n      .sl4::-webkit-slider-thumb {\n      border-style: solid;\n      border-color: #000000;\n      background: #FFFFFF;\n      }\n\n      .sl1 {\n      background: #F5A9A9;\n      }\n      .sl2 {\n      background: #81F781;\n      }\n      .sl3 {\n      background: #8181F7;\n      }\n      .sl4 {\n      background: #BDBDBD;\n      }\n\n      /* Styles for the Buttons */\n      .button {\n      background-color: #4CAF50;\n      border: none;\n      color: white;\n      padding: 15px 32px;\n      text-align: center;\n      text-decoration: none;\n      display: inline-block;\n      font-size: 16px;\n      margin: 4px 2px;\n      cursor: pointer;\n      }\n      \n    </style>\n  </head>\n\n  <body>\n    <h1>Round Range Slider</h1>\n\n    <input type=\"range\" min=\"0\" max=\"255\" value=\"127\" class=\"slider sl1\" id=\"Range_R\">\n    <p>Red Intensity: <span id=\"Value_R\"></span></p>\n\n    <input type=\"range\" min=\"0\" max=\"255\" value=\"127\" class=\"slider sl2\" id=\"Range_G\">\n    <p>Green Intensity: <span id=\"Value_G\"></span></p>\n    \n    <input type=\"range\" min=\"0\" max=\"255\" value=\"127\" class=\"slider sl3\" id=\"Range_B\">\n    <p>Blue Intensity: <span id=\"Value_B\"></span></p>\n\n    <input type=\"range\" min=\"0\" max=\"255\" value=\"127\" class=\"slider sl4\" id=\"Range_W\">\n    <p>White Intensity: <span id=\"Value_W\"></span></p>\n\n\n    <button class=\"button\" onclick=\"localButtonRGB()\">\n      Apagar RGB\n    </button>\n    <button class=\"button\" onclick=\"localButtonW()\">\n      Apagar Blanco\n    </button>\n    <button class=\"button\" onclick=\"checkButton('FADE_RGB')\" id=\"FADE_RGB\">\n      Fundido RGB\n    </button>\n    <button class=\"button\" onclick=\"checkButton('FADE_W')\" id=\"FADE_W\">\n      Fundido Blanco\n    </button>\n\n    <p>Comms Status: <span id=\"CommStatus\"></span></p>\n\n    <script>\n      var slider_r = document.getElementById(\"Range_R\");\n      var slider_g = document.getElementById(\"Range_G\");\n      var slider_b = document.getElementById(\"Range_B\");\n      var slider_w = document.getElementById(\"Range_W\");\n      var value_r = document.getElementById(\"Value_R\");\n      var value_g = document.getElementById(\"Value_G\");\n      var value_b = document.getElementById(\"Value_B\");\n      var value_w = document.getElementById(\"Value_W\");\n\n      value_r.innerHTML = slider_r.value;\n      value_g.innerHTML = slider_g.value;\n      value_b.innerHTML = slider_b.value;\n      value_w.innerHTML = slider_w.value;\n\n      <!-- Cambio de valores de sliders -->\n      slider_r.oninput = function() {\n      value_r.innerHTML = this.value;\n      }\n      slider_g.oninput = function() {\n      value_g.innerHTML = this.value;\n      }\n      slider_b.oninput = function() {\n      value_b.innerHTML = this.value;\n      }\n      slider_w.oninput = function() {\n      value_w.innerHTML = this.value;\n      }\n\n      <!-- Cambio Texto en Botones -->\n      function checkButton(who) {\n      if (who == 'FADE_W') {\n      if (document.getElementById(\"FADE_W\").innerHTML == \"Fundido Blanco\") {\n      document.getElementById(\"FADE_W\").innerHTML = \"Detener Fundido Blanco\";\n      document.getElementById(\"FADE_RGB\").innerHTML = \"Fundido RGB\";\n      sendButtons('fade_w','on')\n      }\n      else {\n      document.getElementById(\"FADE_W\").innerHTML = \"Fundido Blanco\";\n      sendButtons('fade_w','off');\n      }\n      }\n      if (who == 'FADE_RGB') {\n      if (document.getElementById(\"FADE_RGB\").innerHTML == \"Fundido RGB\") {\n      document.getElementById(\"FADE_RGB\").innerHTML = \"Detener Fundido RGB\";\n      document.getElementById(\"FADE_W\").innerHTML = \"Fundido Blanco\";\n      sendButtons('fade_rgb','on')\n      }\n      else {\n      document.getElementById(\"FADE_RGB\").innerHTML = \"Fundido RGB\";\n      sendButtons('fade_rgb','off');\n      }\n      }      \n      }\n\n      <!-- Handlers de Botones Locales -->\n      function localButtonRGB() {\n      slider_r.value = 0;\n      slider_g.value = 0;\n      slider_b.value = 0;\n      value_r.innerHTML = slider_r.value;\n      value_g.innerHTML = slider_g.value;\n      value_b.innerHTML = slider_b.value;\n      }\n\n      function localButtonW() {\n      slider_w.value = 0;\n      value_w.innerHTML = slider_w.value;\n      }\n      \n      <!-- Interacion AJAX -->\n      function sendButtons(action,value) {\n      var xhttp = new XMLHttpRequest();\n      xhttp.onreadystatechange = function() {\n      if (this.readyState == 4 && this.status == 200) {\n      document.getElementById(\"CommStatus\").innerHTML =\n      this.responseText;\n      }\n      };\n      xhttp.open(\"GET\", \"Button?\"+action+\"=\"+value, true);      \n      xhttp.send();\n      }\n\n      function sendSliders() {\n      var xhttp = new XMLHttpRequest();\n      xhttp.onreadystatechange = function() {\n      if (this.readyState == 4 && this.status == 200) {\n      document.getElementById(\"CommStatus\").innerHTML =\n      this.responseText;\n      }\n      };\n      xhttp.open(\"GET\", \"Slider?R=\"+value_r.innerHTML\n      +\"&G=\"+value_g.innerHTML\n      +\"&B=\"+value_b.innerHTML\n      +\"&W=\"+value_w.innerHTML, true);      \n      xhttp.send();\n      }\n\n      <!-- Two seconds Timer -->\n      setInterval(function() {\n      sendSliders();\n      }, 2000); //2000mSeconds update rate \n    </script>\n  </body>\n</html>"};



ESP8266WebServer server(80);
const int led = 2;

//--- This is the AP Side Conf
// Soft AP network parameters
IPAddress apIP(192, 168, 20, 1);
IPAddress netMsk(255, 255, 255, 0);
// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;
//--- End of AP Side Conf


// Server Handlers
void handleRoot() {
  digitalWrite(led, 0);
  server.send_P(200, "text/html", index_html);
  digitalWrite(led, 1);
}


void handleSliders() {
    String message = "";
    digitalWrite(led, 0);

    //busco solo 4 argumentos (RGBW)
    if (server.args() == 4)
    {
        for (int i = 0; i < server.args(); i++)
        {
            message += server.argName(i) + ":";     //Get the name of the parameter
            message += server.arg(i) + ";";              //Get the value of the parameter
        }
        Serial.println(message);
        server.send(200, "text/html", "Done!");       //Response to the HTTP request
    }
    else
        server.send(200, "text/html", "Error in Args");
    
    digitalWrite(led, 1);
}


void handleButtons() {
    String message = "";
    digitalWrite(led, 0);

    //busco solo un solo argumento
    if (server.args() == 1)
    {
        message += server.argName(0) + ":";     //Get the name of the parameter
        message += server.arg(0) + ";";              //Get the value of the parameter
        Serial.println(message);
        server.send(200, "text/html", "Done!");       //Response to the HTTP request
    }
    else
        server.send(200, "text/html", "Error in Args");
    
    digitalWrite(led, 1);
}

// void handleNotFound(){
//   digitalWrite(led, 0);
//   String message = "File Not Found\n\n";
//   message += "URI: ";
//   message += server.uri();
//   message += "\nMethod: ";
//   message += (server.method() == HTTP_GET)?"GET":"POST";
//   message += "\nArguments: ";
//   message += server.args();
//   message += "\n";
//   for (uint8_t i=0; i<server.args(); i++){
//     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//   }
//   server.send(404, "text/plain", message);
//   digitalWrite(led, 1);
// }

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);

  //--- This is the Client Side Conf
  // WiFi.begin(ssid, password);
  // Serial.println("");

  // // Wait for connection
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  //--- End of Client Side Conf

  //--- This is the AP Side Conf
  Serial.println("");  
  Serial.println("Configuring access point...");
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  //--- End of AP Side Conf
  
  // Server routes
  server.on("/", handleRoot);
  server.on("/Button", handleButtons);
  server.on("/Slider", handleSliders);

  //--- This is the Client Side Conf
  // server.onNotFound(handleNotFound);
  //--- This is the AP Side Conf
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
    dnsServer.processNextRequest();
    server.handleClient();
}
