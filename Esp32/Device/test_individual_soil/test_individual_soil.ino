////// this code saves 100 data points in the esp32's memory
///// over the period of a day
//// max sleep time for an esp32 is 40 days (continuosly).
////






const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

#define TOUCH_THRESHOLD 50

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 20       /* ESP32 should sleep more seconds  (note SIM7000 needs ~20sec to turn off if sleep is activated) */
RTC_DATA_ATTR int bootCount = 0;    
//saving bootcount into RTC memory (max 8kb)
const int total_number_of_elements = 3;
RTC_DATA_ATTR int moisture_1[total_number_of_elements];
RTC_DATA_ATTR int moisture_2[total_number_of_elements];
RTC_DATA_ATTR int moisture_3[total_number_of_elements];
RTC_DATA_ATTR int moisture_4[total_number_of_elements];
RTC_DATA_ATTR int moisture_5[total_number_of_elements];


// soil moisture sensor is connected to GPIO 34 () 
const int moisture_1_pin = 32;
const int moisture_2_pin = 33;
const int moisture_3_pin = 34;
const int moisture_4_pin = 14;
const int moisture_5_pin = 15;

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void shutdown()
{

  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}




void setup() {
  Serial.begin(115200);
  delay(1000);
  
moisture_1[bootCount]= analogRead(moisture_1_pin);
moisture_2[bootCount]= analogRead(moisture_2_pin);
moisture_3[bootCount]= analogRead(moisture_3_pin);
moisture_4[bootCount]= analogRead(moisture_4_pin);
moisture_5[bootCount]= analogRead(moisture_5_pin);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

/*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  delay(2000); 
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

for (int x =0; x <total_number_of_elements; x++){ 
 Serial.println(moisture_1[x]);
 Serial.println(moisture_2[x]);
 Serial.println(moisture_3[x]);
 Serial.println(moisture_4[x]);
 Serial.println(moisture_5[x]);
 Serial.println("----------------");
 delay(200);
}

if (bootCount == total_number_of_elements){
  bootCount=0;
}
  
  delay(1000); // required - else will shutdown too early and miss last value
  shutdown();
}

void loop() {


 
 delay(3000);
}
