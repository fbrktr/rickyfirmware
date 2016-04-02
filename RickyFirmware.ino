unsigned int localPort = 9999;  // Listen port, server sends data here
char udpBuff[512];              // Buffer to hold received commands
int remotePort = 9999;          // Servers listen port
IPAddress server;               // Servers listen address
UDP Udp;

char localIP[15] = "";          // Exposes wifi local IP to the particle cloud

int lastHeartBeat = 0;          // HeartBeat timing holder

int led = D7;                   // Onboard blue led

void setup() {
  pinMode(led, OUTPUT);                   // Set led output
  Particle.variable("localip", localIP);  // Exposes localIP variable

  IPAddress myLocalIP = WiFi.localIP();   // Updates localIP
  sprintf(localIP, "%d.%d.%d.%d", myLocalIP[0], myLocalIP[1], myLocalIP[2], myLocalIP[3]);

  Udp.begin(localPort);                   // Opens udp connection
  Serial.begin(9600);                     // Starts usb serial port
}

// Udp commands handler
void handleCommand(){
  String command  = String(udpBuff);
  // Checks if server IP defined before executing anything
  if(server){
    Serial.println(command);
  }
  // Check for handshake
  else{
    if(command == "Hey !"){       // Server has to say hello
      digitalWrite(led, HIGH);
      server = Udp.remoteIP();    // defines server so we can use normal handler
      sendHeartBeat();            // Reply by sending HeartBeat
      Serial.println(server);
    }
  }
}

void loop() {
  sendHeartBeat();
  handleUDP();
}

// HeartBeat sending routine
void sendHeartBeat(){
  if(millis() - lastHeartBeat > 10000UL && server){
    Udp.beginPacket(server, remotePort);
    Udp.write('Y');
    Udp.write('o');
    Udp.write('!');
    Udp.endPacket();
    lastHeartBeat = millis();
  }
}

// Udp input handler
void handleUDP(){
  if (Udp.parsePacket()) {    // Checks Data Availabilty
    Udp.read(udpBuff, 512);   // Writes to command buffer
    handleCommand();          // Launches command handler
  }
}
