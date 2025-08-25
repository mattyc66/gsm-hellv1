#include <Arduino.h>
#include <HardwareSerial.h>

const long Baudrate = 115200;
const char RX_Pin = 16;
const char TX_Pin = 17;


HardwareSerial sim(1);

void command(String command, unsigned long timeout = 1000) {
    sim.println(command);
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (sim.available()) {
            String response = sim.readString();
            Serial.println(response);
            break;
        }
    }
}

void SetHTTPS() {
    command("");
}

void upload() {
    command("AT+CSSLCFG=\"sslversion\",0,3");
    command("AT+CSSLCFG=\"ignorelocaltime\",0,1");
    command("AT+CSSLCFG=\"seclevel\",0,0");

    command("AT+HTTPINIT", 5000);
    command("AT+HTTPPARA=\"URL\",\"https://tracking-system-database-default-rtdb.europe-west1.firebasedatabase.app/test.json?auth=mBxdxNNC3idfnxM7vmPzJ9cqHKF0s5bvcLxt8L8g\"");
    command("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

    sim.println("AT+HTTPDATA=35,10000");
    delay(3000);

    if (sim.available()) {
        String response =  sim.readString();
        if (response.indexOf("DOWNLOAD") != -1) {
            Serial.println("Sending JSON...");
            sim.println("{\"message\":\"test from SIM7600\"}");
            delay(1000);
            sim.write(26);
            delay(2000);
        }
    }

    command("AT+HTTPACTION?");
    command("AT+HTTPACTION=1", 5000);
    command("AT+HTTPREAD");
    command("AT+HTTPTERM");
}

void setup() {
    Serial.begin(115200);
    sim.begin(Baudrate, SERIAL_8N1, RX_Pin, TX_Pin);
    command("AT"); // test stuff
    command("ATI"); // module status stuff
    command("AT+CSQ"); // signal
    command("AT+CGDCONT=1,\"IP\",\"everywhere\"");
    command("AT+CGATT=1");
    command("AT+CGACT=1,1");
    command("AT+NETOPEN");
    delay(100);
    upload();
}

void loop() {
    while (sim.available()) {
        Serial.write(sim.read());
    }
}