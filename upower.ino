#include <SimpleTimer.h>
#include <ModbusMaster.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

float utility_voltage, utility_current, utility_wattage;
float battery_voltage, battery_temp, battery_soc;
float pv_voltage, pv_current, pv_wattage;
float cell_data[30];
float inverter_voltage, inverter_current, inverter_wattage;
float pv_charging_voltage, pv_charging_current, pv_charging_wattage;

float battery_discharging_voltage, battery_discharging_current, battery_discharging_wattage, battery_capacity, battery_current;
float temp_mosfet, temp_balance, temp_cell1, temp_cell5;
char *charge_mosfet_status_describe, *discharge_mosfet_status_describe, *balance_status_describe;

float utility_charging_voltage, utility_charging_current, utility_charging_wattage;
float bypass_voltage, bypass_current, bypass_wattage;

float inverter_frequency;

const char* ssid = "SSID";
const char* password = "SSID_PW";

const char* mqtt_server = "mqtt_server";
const char* mqttClientName = "ESP8266Client";
const char* mqttUserId = "mqtt_ID";
const char* mqttUserPw = "mqtt_PW";

long lastMsg = 0;
long lastMsg2 = 0;


uint8_t result;
const int debug=1;

SoftwareSerial Serial2;


// bool rs485DataReceived = true;

ModbusMaster node;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void preTransmission() {}
void postTransmission() {}

typedef void (*RegistryList[])();
RegistryList Registries = {
  AddressRegistry_3500,
  AddressRegistry_352f,
  AddressRegistry_3519,
  AddressRegistry_354c,
  getBMSAttributes
};

uint8_t currentRegistryNumber = 0;

void nextRegistryNumber() {
  currentRegistryNumber = (currentRegistryNumber + 1) % ARRAY_SIZE( Registries);
}

void setup()
{
  Serial.begin(19200);
  Serial1.begin(115200);

  Serial2.begin(115200, SWSERIAL_8N1, 12, 13, false, 256);//rx, tx


  node.begin(10, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(gotMessage);

  establish_connections();
  setup_mqtt_device();
}

char* assemble_device(){
  char buf[1000];
  char device_id[300];

  sprintf(device_id, "%06X", ESP.getChipId());
  sprintf(buf, "{\"identifiers\":[\"%s\"],\"connections\":[[\"mac\",\"%s\"]],\"name\":\"%s\",\"model\":\"%s\",\"sw_version\":\"%s\",\"manufacturer\":\"%s\"}", device_id, WiFi.macAddress().c_str(), "Battery", "ESP8266", "1.0.0", "nam6411");
  return buf;
}

char* assemble_discover_switch_message(char* uniq_id, char* name, char* state_topic, char* command_topic){

  char buf[1000];
  char device_id[300];

  sprintf(device_id, "%06X", ESP.getChipId());

  sprintf(buf, "{\"device\":%s, \"uniq_id\":\"%s_%s\",  \"name\": \"%s\", \"state_topic\": \"%s\", \"command_topic\": \"%s\" }", assemble_device(), device_id, uniq_id, name, state_topic, command_topic);
//   "{ \"uniq_id\":\"93069C_\",  \"name\": \"Inverter\", \"command_topic\": \"homeassistant/switch/epever/inverter/set\", \"state_topic\": \"homeassistant/switch/epever/inverter/state\"}"
  return buf;
}

char* assemble_discover_sensor_message(char* uniq_id, char* name, char* unit, char* device_class, char* state_topic, char* value_json){

  char buf[1000];
  char device_id[300];

  sprintf(device_id, "%06X", ESP.getChipId());

  sprintf(buf, "{\"device_class\": \"%s\", \"device\":%s, \"uniq_id\":\"%s_%s\",  \"name\": \"%s\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"%s\", \"value_template\": \"{{ value_json.%s}}\" }", device_class, assemble_device(), device_id, uniq_id, name, state_topic, unit, value_json);
  return buf;
}

void setup_mqtt_device(){

//    mqtt_publish_P ("homeassistant/sensor/bms/cell1/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell2/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell3/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell4/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell5/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell6/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell7/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/cell8/config", "");
//    mqtt_publish_P ("homeassistant/switch/epever/inverter/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/battery_temp/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/battery_soc/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/battery_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/inverter_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/inverter_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/inverter_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/inverter_frequency/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/bypass_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/bypass_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/bypass_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/utility_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/epever/pv_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/temp_mosfet/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/temp_balance/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/temp_cell1/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/temp_cell5/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_voltage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_current/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_wattage/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/battery_capacity/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/charge_mosfet_status_describe/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/discharge_mosfet_status_describe/config", "");
//    mqtt_publish_P ("homeassistant/sensor/bms/balance_status_describe/config", "");
    


    
    mqtt_publish_P ("homeassistant/sensor/bms/temp_mosfet/config", assemble_discover_sensor_message("tm", "Temp Mosfet", "°C", "temperature", "homeassistant/sensor/bms/state", "temp_mosfet"));
    mqtt_publish_P ("homeassistant/sensor/bms/temp_balance/config", assemble_discover_sensor_message("tb", "Temp Balance", "°C", "temperature", "homeassistant/sensor/bms/state", "temp_balance"));
    mqtt_publish_P ("homeassistant/sensor/bms/temp_cell1/config", assemble_discover_sensor_message("tc1", "Temp Cell1", "°C", "temperature", "homeassistant/sensor/bms/state", "temp_cell1"));
    mqtt_publish_P ("homeassistant/sensor/bms/temp_cell5/config", assemble_discover_sensor_message("tc5", "Temp Cell5", "°C", "temperature", "homeassistant/sensor/bms/state", "temp_cell5"));
    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_voltage/config", assemble_discover_sensor_message("bdv", "Battery Dischg Voltage", "V", "power", "homeassistant/sensor/bms/state", "battery_discharging_voltage"));
    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_current/config", assemble_discover_sensor_message("bdc", "Battery Dischg Current", "A", "power", "homeassistant/sensor/bms/state", "battery_discharging_current"));
    mqtt_publish_P ("homeassistant/sensor/bms/battery_discharging_wattage/config", assemble_discover_sensor_message("bdw", "Battery Dischg Wattage", "W", "power", "homeassistant/sensor/bms/state", "battery_discharging_wattage"));
    mqtt_publish_P ("homeassistant/sensor/bms/battery_capacity/config", assemble_discover_sensor_message("btc", "Battery Capacity", "Ah", "battery", "homeassistant/sensor/bms/state", "battery_capacity"));
    mqtt_publish_P ("homeassistant/sensor/bms/charge_mosfet_status_describe/config", assemble_discover_sensor_message("cms", "Charge Mosfet Status", "", "", "homeassistant/sensor/bms/state", "charge_mosfet_status_describe"));
    mqtt_publish_P ("homeassistant/sensor/bms/discharge_mosfet_status_describe/config", assemble_discover_sensor_message("dms", "Discharge Mosfet Status", "", "", "homeassistant/sensor/bms/state", "discharge_mosfet_status_describe"));
    mqtt_publish_P ("homeassistant/sensor/bms/balance_status_describe/config", assemble_discover_sensor_message("bals", "Balance Status", "", "", "homeassistant/sensor/bms/state", "balance_status_describe"));
    
    mqtt_publish_P ("homeassistant/sensor/epever/battery_soc/config", assemble_discover_sensor_message("soc", "Battery Remain", "%", "battery", "homeassistant/sensor/epever/battery/state", "battery_soc"));
    mqtt_publish_P ("homeassistant/sensor/epever/battery_voltage/config", assemble_discover_sensor_message("bv", "Battery Voltage", "V", "power", "homeassistant/sensor/epever/battery/state", "battery_voltage"));
    
    mqtt_publish_P ("homeassistant/sensor/bms/cell1/config", assemble_discover_sensor_message("cell1", "Cell 1", "V", "power", "homeassistant/sensor/bms/state", "cell1"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell2/config", assemble_discover_sensor_message("cell2", "Cell 2", "V", "power", "homeassistant/sensor/bms/state", "cell2"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell3/config", assemble_discover_sensor_message("cell3", "Cell 3", "V", "power", "homeassistant/sensor/bms/state", "cell3"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell4/config", assemble_discover_sensor_message("cell4", "Cell 4", "V", "power", "homeassistant/sensor/bms/state", "cell4"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell5/config", assemble_discover_sensor_message("cell5", "Cell 5", "V", "power", "homeassistant/sensor/bms/state", "cell5"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell6/config", assemble_discover_sensor_message("cell6", "Cell 6", "V", "power", "homeassistant/sensor/bms/state", "cell6"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell7/config", assemble_discover_sensor_message("cell7", "Cell 7", "V", "power", "homeassistant/sensor/bms/state", "cell7"));
    mqtt_publish_P ("homeassistant/sensor/bms/cell8/config", assemble_discover_sensor_message("cell8", "Cell 8", "V", "power", "homeassistant/sensor/bms/state", "cell8"));
    mqtt_publish_P ("homeassistant/switch/epever/inverter/config", assemble_discover_switch_message("isw", "Inverter Switch", "homeassistant/switch/epever/inverter/state", "homeassistant/switch/epever/inverter/set"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_current/config", assemble_discover_sensor_message("ucc", "Utility Chg Current", "A", "power", "homeassistant/sensor/epever/battery_charge/state", "utility_charging_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_voltage/config", assemble_discover_sensor_message("ucv", "Utility Chg Voltage", "V", "power", "homeassistant/sensor/epever/battery_charge/state", "utility_charging_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_charging_wattage/config", assemble_discover_sensor_message("ucw", "Utility Chg Wattage", "W", "power", "homeassistant/sensor/epever/battery_charge/state", "utility_charging_wattage"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_current/config", assemble_discover_sensor_message("pcc", "PV chg Current", "A", "power", "homeassistant/sensor/epever/battery_charge/state", "pv_charging_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_voltage/config", assemble_discover_sensor_message("pcv", "PV chg Voltage", "V", "power", "homeassistant/sensor/epever/battery_charge/state", "pv_charging_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_charging_wattage/config", assemble_discover_sensor_message("pcw", "PV chg Wattage", "W", "power", "homeassistant/sensor/epever/battery_charge/state", "pv_charging_wattage"));
    
    mqtt_publish_P ("homeassistant/sensor/epever/battery_temp/config", assemble_discover_sensor_message("bt", "Battery Inverter", "°C", "temperature", "homeassistant/sensor/epever/battery/state", "battery_temp"));
    mqtt_publish_P ("homeassistant/sensor/epever/battery_soc/config", assemble_discover_sensor_message("soc", "Battery Remain", "%", "battery", "homeassistant/sensor/epever/battery/state", "battery_soc"));
    mqtt_publish_P ("homeassistant/sensor/epever/battery_voltage/config", assemble_discover_sensor_message("bv", "Battery Voltage", "V", "power", "homeassistant/sensor/epever/battery/state", "battery_voltage"));
    
    mqtt_publish_P ("homeassistant/sensor/epever/inverter_voltage/config", assemble_discover_sensor_message("iv", "Inverter Voltage", "V", "power", "homeassistant/sensor/epever/inverter/state", "inverter_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/inverter_current/config", assemble_discover_sensor_message("ic", "Inverter Current", "A", "power", "homeassistant/sensor/epever/inverter/state", "inverter_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/inverter_wattage/config", assemble_discover_sensor_message("iw", "Inverter Wattage", "W", "power", "homeassistant/sensor/epever/inverter/state", "inverter_wattage"));
    mqtt_publish_P ("homeassistant/sensor/epever/inverter_frequency/config", assemble_discover_sensor_message("if", "Inverter Frequency", "Hz", "power", "homeassistant/sensor/epever/inverter/state", "inverter_frequency"));
    mqtt_publish_P ("homeassistant/sensor/epever/bypass_voltage2/config", assemble_discover_sensor_message("bpv", "Bypass Voltage", "V", "power", "homeassistant/sensor/epever/bypass/state", "bypass_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/bypass_current/config", assemble_discover_sensor_message("bc", "Bypass Current", "A", "power", "homeassistant/sensor/epever/bypass/state", "bypass_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/bypass_wattage/config", assemble_discover_sensor_message("bw", "Bypass Wattage", "W", "power", "homeassistant/sensor/epever/bypass/state", "bypass_wattage"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_voltage/config", assemble_discover_sensor_message("uv", "Utility Voltage", "V", "power", "homeassistant/sensor/epever/utility/state", "utility_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_current/config", assemble_discover_sensor_message("uc", "Utility Current", "A", "power", "homeassistant/sensor/epever/utility/state", "utility_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/utility_wattage/config", assemble_discover_sensor_message("uw", "Utility Wattage", "W", "power", "homeassistant/sensor/epever/utility/state", "utility_wattage"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_voltage/config", assemble_discover_sensor_message("pv", "PV Voltage", "V", "power", "homeassistant/sensor/epever/pv/state", "pv_voltage"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_current/config", assemble_discover_sensor_message("pc", "PV Current", "A", "power", "homeassistant/sensor/epever/pv/state", "pv_current"));
    mqtt_publish_P ("homeassistant/sensor/epever/pv_wattage/config", assemble_discover_sensor_message("pw", "PV Wattage", "W", "power", "homeassistant/sensor/epever/pv/state", "pv_wattage"));

}

void mqtt_publish_P(char topic[], char message[]){

  if(debug == 1){
    Serial1.print("topic : ");
    Serial1.println(topic);
    Serial1.print("message (");
    Serial1.print(sizeof(message));
    Serial1.print(") : ");
    Serial1.println(message);
  }
  
  mqttClient.publish_P(topic, message, sizeof(message));
  delay(100);
}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network

  if(debug == 1){
    Serial1.println();
    Serial1.print("Connecting to ");
    Serial1.println(ssid);
  }

  

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  if(debug == 1){
    Serial1.println("");
    Serial1.println("WiFi connected");
    Serial1.println("IP address: ");
    Serial1.println(WiFi.localIP());
  }
}


void gotMessage(char* topic, byte* message, unsigned int length) {
//  if(debug == 1){
  Serial1.print("Message arrived on topic: ");
  Serial1.print(topic);
  Serial1.print(". Message: ");  
//  }
  

  String messageTemp;

  for (int i = 0; i < length; i++) {
    if(debug == 1){

      Serial1.print((char)message[i]);
    }
    messageTemp += (char)message[i];
  }

//  Serial.println();

  if (String(topic) == "homeassistant/switch/epever/inverter/set") {
//    if(debug == 1){
      Serial1.print("Inverter Turn ");
      Serial1.println(messageTemp);
//    }
      lastMsg = millis();
      delay(100);


    int result = -1 ;
    if(messageTemp == "ON"){
    //0a 05 01 06 ff 00
    // node.writeSingleCoil(0x0501,0xff);
      result = node.writeSingleCoil(0x0106,0xff);
      if(result == 0){
        mqttClient.publish("homeassistant/switch/epever/inverter/state", "ON");  
      }else{
        Serial1.print("inverter switch failcode : ");
        Serial1.println(result);
      }
    }else{
    //0a 05 01 06 00 00
      result = node.writeSingleCoil(0x0106,0x00);
      if(result == 0){
        mqttClient.publish("homeassistant/switch/epever/inverter/state", "OFF");  
      }else{
        Serial1.print("inverter switch failcode : ");
        Serial1.println(result);
      }
    }
    

    //to refresh inverter state
//    delay(500);
//    AddressRegistry_352f();
  }
}


void setup_mqtt() {
  while (!mqttClient.connected()) {
    if(debug == 1){
      Serial1.print("Attempting MQTT connection...");  
    }
    
    if (mqttClient.connect(mqttClientName, mqttUserId, mqttUserPw)) {
      if(debug == 1){
        Serial1.println("connected");
      }
      
      mqttClient.subscribe("homeassistant/switch/epever/inverter/set");

    } else {
      if(debug == 1){
        Serial1.print("failed, rc=");
        Serial1.print(mqttClient.state());
        Serial1.println(" try again in 5 seconds");
      }
    }
  }
}


void establish_connections(){
  if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
  }

  if (!mqttClient.connected()) {
    setup_mqtt();
  }
}


void doRegistryNumber() {
  Registries[currentRegistryNumber]();
}

void AddressRegistry_3500() {
  // 한전 입력  3500  0
  // 배터리 충전 전압  3505  5
  // 배터리 충전 전류  3506  6
  // 배터리 충전 전력  3507  7
  result = node.readInputRegisters(0x3500, 19);

  if (result == node.ku8MBSuccess)
  {
    // utility_voltage = node.getResponseBuffer(0x00) / 100.0f;
    // utility_current = node.getResponseBuffer(0x03) / 100.0f;
//     pv_voltage = node.getResponseBuffer(0x05) / 100.0f;
    // battery_charging_current = node.getResponseBuffer(0x06) / 100.0f;
    // battery_charging_voltage = node.getResponseBuffer(0x07) / 100.0f;

    utility_voltage = node.getResponseBuffer(0x00) / 100.0f;
//    utility_current = node.getResponseBuffer(0x02) / 100.0f;//??

    utility_charging_voltage= node.getResponseBuffer(0x05) / 100.0f;
    utility_charging_current = node.getResponseBuffer(0x06) / 100.0f;
    utility_charging_wattage = node.getResponseBuffer(0x07) / 100.0f;


    if (debug == 1) {
      Serial1.print("utility input Voltage: ");
      Serial1.println(utility_voltage);
      Serial1.print("utility input Current: ");
      Serial1.println(utility_current);

      Serial1.print("utility charge Current: ");
      Serial1.println(utility_charging_current);
      Serial1.print("utility charge voltage: ");
      Serial1.println(utility_charging_voltage);
      Serial1.print("utility charge wattage: ");
      Serial1.println(utility_charging_wattage);
    }

    
    char buf[500];

    sprintf(buf, "{\"utility_voltage\": %3.2f, \"utility_current\": %2.2f}", utility_voltage, utility_current);
    mqtt_publish_P("homeassistant/sensor/epever/utility/state", buf);

    sprintf(buf, "{\"utility_charging_current\": %2.2f, \"utility_charging_voltage\": %2.2f, \"utility_charging_wattage\": %3.2f, \"pv_charging_current\": %2.2f, \"pv_charging_voltage\": %2.2f, \"pv_charging_wattage\": %3.2f}", utility_charging_current, utility_charging_voltage, utility_charging_wattage, pv_charging_current, pv_charging_voltage, pv_charging_wattage);
    mqtt_publish_P("homeassistant/sensor/epever/battery_charge/state", buf);
    
  }else{
    if (debug == 1) {
      Serial1.print("result=");
      Serial1.println(result);
    }
    utility_voltage = -1;
    utility_current = -1;
    utility_charging_current = -1;
    utility_charging_voltage = -1;
    utility_charging_wattage = -1;
  }

}


void AddressRegistry_3519() {
  result = node.readInputRegisters(0x3519, 16);

  if (result == node.ku8MBSuccess)
  {

    pv_voltage = node.getResponseBuffer(0x00) / 100.0f; 
    pv_current = node.getResponseBuffer(0x01) / 100.0f; 
    pv_wattage = node.getResponseBuffer(0x02) / 100.0f; 
    
    pv_charging_voltage = node.getResponseBuffer(0x04) / 100.0f;
    //351d
    
    pv_charging_current = node.getResponseBuffer(0x05) / 100.0f;
    //351e
    
    pv_charging_wattage = node.getResponseBuffer(0x06) / 100.0f;
    //351f


    if (debug == 1) {
      Serial1.print("pv charge Current: ");
      Serial1.println(pv_charging_current);
      Serial1.print("pv charge voltage: ");
      Serial1.println(pv_charging_voltage);
      Serial1.print("pv charge wattage: ");
      Serial1.println(pv_charging_wattage);
    }
    
    char buf[500];

    sprintf(buf, "{\"utility_charging_current\": %2.2f, \"utility_charging_voltage\": %2.2f, \"utility_charging_wattage\": %3.2f, \"pv_charging_current\": %2.2f, \"pv_charging_voltage\": %2.2f, \"pv_charging_wattage\": %3.2f}", utility_charging_current, utility_charging_voltage, utility_charging_wattage, pv_charging_current, pv_charging_voltage, pv_charging_wattage);
    mqtt_publish_P("homeassistant/sensor/epever/battery_charge/state", buf);

    sprintf(buf, "{\"pv_voltage\": %3.2f, \"pv_current\": %2.2f, \"pv_wattage\": %2.2f}", pv_voltage, pv_current, pv_wattage);
    mqtt_publish_P("homeassistant/sensor/epever/pv/state", buf);
    
  }else{
    if (debug == 1) {
      Serial1.print("result=");
      Serial1.println(result);
    }
    pv_voltage = -1;
    pv_current = -1;
    pv_wattage = -1;
    
    pv_charging_current = -1;
    pv_charging_voltage = -1;
    pv_charging_wattage = -1;
  }

}

void AddressRegistry_352f() {
  result = node.readInputRegisters(0x352f, 16);

  if (result == node.ku8MBSuccess)
  {
    inverter_voltage = node.getResponseBuffer(4) / 100.0f;
    inverter_current = node.getResponseBuffer(5) / 100.0f;
    inverter_wattage = node.getResponseBuffer(7) / 100.0f;
    inverter_frequency = node.getResponseBuffer(12) / 100.0f;

//    Serial1.printf("freq : %02X\n", node.getResponseBuffer(0x12));

    if (debug == 1) {
      Serial1.print("inverter Voltage: ");
      Serial1.println(inverter_voltage);
      Serial1.print("inverter Current: ");
      Serial1.println(inverter_current);
      Serial1.print("inverter Watt: ");
      Serial1.println(inverter_wattage);
      Serial1.print("inverter Freq: ");
      Serial1.println(inverter_frequency);
    }
    
    char buf[500];

    sprintf(buf, "{\"inverter_voltage\": %3.2f, \"inverter_current\": %2.2f, \"inverter_wattage\": %2.2f, \"inverter_frequency\": %2.2f }", inverter_voltage, inverter_current, inverter_wattage, inverter_frequency);
    mqtt_publish_P("homeassistant/sensor/epever/inverter/state", buf);

    if(inverter_voltage > -1 && inverter_voltage < 1 ){
        mqttClient.publish("homeassistant/switch/epever/inverter/state", "OFF");
    }else if( inverter_voltage > 1){
        mqttClient.publish("homeassistant/switch/epever/inverter/state", "ON");
    }
    
  }else{
    if (debug == 1) {
      Serial1.print("result=");
      Serial1.println(result);
    }
    inverter_voltage = -1;
    inverter_current = -1;
    inverter_voltage = -1;
    inverter_frequency = -1;
  }

}


void AddressRegistry_354c() {

  // 배터리 전압  354c  0
  // 배터리 온도 354f  3
  // 배터리 SOC  3550  4
  // 바이패스 볼트  3558  12
  // 바이패스 암페어 3559  13
  // 바이패스 와트  355a  14

  result = node.readInputRegisters(0x354c, 16);

  if (result == node.ku8MBSuccess)
  {
    battery_voltage= node.getResponseBuffer(0) / 100.0f;
    battery_temp= node.getResponseBuffer(3) / 100.0f;
    battery_soc= node.getResponseBuffer(4);


    bypass_voltage= node.getResponseBuffer(12) / 100.0f;
    bypass_current= node.getResponseBuffer(13) / 100.0f;
//    bypass_wattage= (((int)node.getResponseBuffer(0x14)*256)+(int)node.getResponseBuffer(0x15))/100.0f;
    bypass_wattage= node.getResponseBuffer(14) / 100.0f;


    if (debug == 1) {
      Serial1.print("Battery Voltage: ");
      Serial1.println(battery_voltage);
      Serial1.print("Battery Temp: ");
      Serial1.println(battery_temp);
      Serial1.print("Battery SOC: ");
      Serial1.println(battery_soc);

      Serial1.print("Bypass Voltage: ");
      Serial1.println(bypass_voltage);
      Serial1.print("Bypass Current: ");
      Serial1.println(bypass_current);
      Serial1.print("Bypass Wattage: ");
      Serial1.println(bypass_wattage);

    }
    
    char buf[500];
    
    sprintf(buf, "{\"battery_voltage\": %2.2f, \"battery_temp\": %2.1f, \"battery_soc\": %3.1f}", battery_voltage, battery_temp, battery_soc);
    mqtt_publish_P("homeassistant/sensor/epever/battery/state", buf);
    
    sprintf(buf, "{\"bypass_voltage\": %3.2f, \"bypass_current\": %2.2f, \"bypass_wattage\": %2.2f}", bypass_voltage, bypass_current, bypass_wattage);
    mqtt_publish_P("homeassistant/sensor/epever/bypass/state", buf);

  }else{
    if (debug == 1) {
      Serial1.print("result=");
      Serial1.println(result);
    }
    battery_voltage = -1;
    battery_temp = -1;
    battery_soc = -1;
    bypass_voltage = -1;
    bypass_current = -1;
    bypass_wattage = -1;
  }

}




void getBMSAttributes() {
  byte message[] = {0x5A, 0x5A, 0x00, 0x00, 0x01, 0x01};
  char bms_data[140];
  Serial.readBytes(bms_data, 140);
  Serial.flush();

  Serial.write(message, sizeof(message));
  int cnt = 0;
  int size = -1;
  size = Serial.readBytes(bms_data, 140);
  if( debug == 1){
    Serial1.printf("size : %d\r\n",size);
    Serial1.printf("size : %d\r\n",size);  
  }

  float voltage = 0.00f;
  
  if(size >= 140){
      int index=0;
      for(int i = 4; i <70 ; i+=2){
        int tempval = ((int)bms_data[i] << 8 | (int)bms_data[i+1]);
        cell_data[index] = tempval/1000.0f;

//        Serial1.println(tempval);

        if(index > 8){
          break;
        }
        index++;
      }

      for(int i = 1 ; i < 9 ; i ++){
        voltage+=cell_data[i];
      }


//      battery_capacity = (double)(*(int *)bms_data[79])/1000000;
      battery_capacity = (double)((int)bms_data[79] << 24 | (int)bms_data[80] << 16 | (int)bms_data[81] << 8  | (int)bms_data[82])/1000000;
//      battery_capacity = (double)(*(int*)bms_data[70])/10;
      battery_current = (double)((int)bms_data[70] << 24 | (int)bms_data[71] << 16 | (int)bms_data[72] << 8  | (int)bms_data[73])/10;
      
      int charge_mosfet_status = (int)(bms_data[103]);
      switch(charge_mosfet_status){
        case 0 :
           charge_mosfet_status_describe="is turn off";
           break;
          
        case 1 :
           charge_mosfet_status_describe="is turn on";
           break;
          
        case 2 :
           charge_mosfet_status_describe="over charge protect";
           break;
          
        case 3 :
           charge_mosfet_status_describe="over current protect";
           break;
          
        case 4 :
           charge_mosfet_status_describe="battery is full charged";
           break;
          
        case 5 :
           charge_mosfet_status_describe="the total voltage of battery pack is over";
           break;
          
        case 6 :
           charge_mosfet_status_describe="battery over temperature";
           break;
          
        case 7 :
           charge_mosfet_status_describe="the MOSFET over temperature";
           break;
          
        case 8 :
           charge_mosfet_status_describe="Abnormal current";
           break;
          
        case 9 :
           charge_mosfet_status_describe="Balanced string out (a battery is not detected)";
           break;
          
        case 10 :
           charge_mosfet_status_describe="Motherboard over temperature";
           break;
          
        case 13 :
           charge_mosfet_status_describe="Discharge MOSFET abnormality";
           break;
          
        case 15 :
           charge_mosfet_status_describe="Manually turn off";
           break;
      }


      int discharge_mosfet_status = (int)(bms_data[104]);
      switch(discharge_mosfet_status){
        case 0 :
           discharge_mosfet_status_describe="turn off";
           break;
          
        case 1 :
           discharge_mosfet_status_describe="turn on";
           break;
          
        case 2 :
           discharge_mosfet_status_describe="over discharge protect (single battery)";
           break;
          
        case 3 :
           discharge_mosfet_status_describe="over current protect";
           break;
          
        case 5 :
           discharge_mosfet_status_describe="over discharge protect ( battery pack)";
           break;
          
        case 6 :
           discharge_mosfet_status_describe="battery over temperature";
           break;
          
        case 7 :
           discharge_mosfet_status_describe="the MOSFET over temperature";
           break;
          
        case 8 :
           discharge_mosfet_status_describe="Abnormal current";
           break;
          
        case 9 :
           discharge_mosfet_status_describe="Balanced string out (a battery is not detected)";
           break;
          
        case 10 :
           discharge_mosfet_status_describe="Motherboard over temperature";
           break;
          
        case 11 :
           discharge_mosfet_status_describe="charge MOSFET turn on";
           break;
          
        case 12 :
           discharge_mosfet_status_describe="short protect";
           break;
          
        case 13 :
           discharge_mosfet_status_describe="Discharge MOSFET abnormality";
           break;
          
        case 14 :
           discharge_mosfet_status_describe="Start exception";
           break;
          
        case 15 :
           discharge_mosfet_status_describe="Manually turn off";
           break;
      }


      int balance_status = (int)(bms_data[105]);
      switch(balance_status){
        case 0 :
           balance_status_describe="turn off";
           break;
          
        case 1 :
           balance_status_describe="Exceeding limit trigger balance";
           break;
          
        case 2 :
           balance_status_describe="When charging, the voltage difference is too big, trigger balance ";
           break;
          
        case 3 :
           balance_status_describe="balance over temperature";
           break;
          
        case 4 :
           balance_status_describe="Automatic balance";
           break;
          
        case 10 :
           balance_status_describe="Motherboard over temperature";
           break;
          
      }
      
      temp_mosfet = (double)((uint16_t)bms_data[91] << 8  | (uint16_t)bms_data[92]);
      temp_balance = (double)((uint32_t)bms_data[93] << 8  | (uint16_t)bms_data[94]);
      temp_cell1 = (double)((uint16_t)bms_data[95] << 8  | (uint16_t)bms_data[96]);
      temp_cell5 = (double)((uint16_t)bms_data[97] << 8  | (uint16_t)bms_data[98]);

      
      char buf[500];
      sprintf(buf, "{\"battery_discharging_voltage\":%2.2f, \"battery_discharging_current\":%2.2f, \"battery_discharging_wattage\":%3.2f, \"battery_capacity\":%3.2f, ",voltage,battery_current, (voltage * battery_current), battery_capacity  );
      sprintf(&buf[strlen(buf)], "\"temp_mosfet\":%3.2f, \"temp_balance\":%3.2f, \"temp_cell1\":%3.2f, \"temp_cell5\":%3.2f,", temp_mosfet, temp_balance, temp_cell1, temp_cell5);
      sprintf(&buf[strlen(buf)], "\"charge_mosfet_status_describe\":\"%S\", \"discharge_mosfet_status_describe\":\"%S\", \"balance_status_describe\":\"%S\", ", charge_mosfet_status_describe, discharge_mosfet_status_describe, balance_status_describe);
      sprintf(&buf[strlen(buf)], "\"cell1\":%1.3f,\"cell2\":%1.3f,\"cell3\":%1.3f,\"cell4\":%1.3f,\"cell5\":%1.3f,\"cell6\":%1.3f,\"cell7\":%1.3f,\"cell8\":%1.3f}", cell_data[1], cell_data[2], cell_data[3], cell_data[4], cell_data[5], cell_data[6], cell_data[7], cell_data[8]);
      
      mqtt_publish_P("homeassistant/sensor/bms/state", buf);
  }
}





void loop(){
  
  long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;
    doRegistryNumber();
    nextRegistryNumber();
  }
  if (now - lastMsg2 > 50) {
    establish_connections();
    mqttClient.loop();
  }
  
}
