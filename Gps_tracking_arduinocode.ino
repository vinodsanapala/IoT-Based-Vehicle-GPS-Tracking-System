#include <SoftwareSerial.h>

SoftwareSerial sim7600(10, 11); // RX, TX

String latitude = "";
String longitude = "";

void sendAT(String cmd, int delayTime) {
  sim7600.println(cmd);
  delay(delayTime);
  while (sim7600.available()) {
    Serial.write(sim7600.read());
  }
}

void getGPS() {
  sim7600.println("AT+CGPSINFO");
  delay(2000);

  String data = "";
  while (sim7600.available()) {
    char c = sim7600.read();
    data += c;
  }

  Serial.println("Raw GPS: " + data);

  // Basic parsing (simple method)
  if (data.indexOf(",") > 0) {
    latitude = data.substring(12, 21);
    longitude = data.substring(24, 34);
  }
}

void sendSMS(String message) {
  sendAT("AT+CMGF=1", 1000); // Text mode
  sim7600.println("AT+CMGS=\"+919948318795\""); // your number
  delay(1000);

  sim7600.print(message);
  delay(500);

  sim7600.write(26); // CTRL+Z
  delay(5000);
}

void setup() {
  Serial.begin(115200);
  sim7600.begin(115200);

  Serial.println("Starting GPS Tracking...");

  delay(5000);

  // Enable GPS
  sendAT("AT+CGPS=1", 2000);
}

void loop() {
  getGPS();

  if (latitude != "" && longitude != "") {
    String msg = "Location:\n";
    msg += "Lat: " + latitude + "\n";
    msg += "Lng: " + longitude + "\n";
    msg += "https://maps.google.com/?q=" + latitude + "," + longitude;

    Serial.println("Sending SMS...");
    sendSMS(msg);

    delay(60000); // send every 1 min
  } else {
    Serial.println("Waiting for GPS fix...");
    delay(5000);
  }
}
