/**
Authors:
Natu Benyam Demeke
Violet Enslow
Ryanne Wilson


Precharge Controller
*/

#define ULONG_MAX 4294967295UL //Serves as a 'null' state for the timers

//pin IDs
const uint8_t AIR_Precharge = 2; 
const uint8_t AIR_Main = 3;
const uint8_t AIR_Discharge = 5;
const uint8_t LED_Discharge = 9;
const uint8_t LED_Fault = 10;

const uint8_t Optocoupler = 14;
uint8_t BMS = 7;
uint8_t Switch2 = 8;

bool carRunning = false; //True when the car can start driving ; as in, when precharging has finished and is successful.
bool prechargeFailed = false; //True if the precharge failed.
bool dischargeFinished = false; // True if the discharge finished!

//All measurements of time are in milliseconds!

unsigned long prechargeStart = ULONG_MAX; // The time at which precharge started; in millis; ULONG_MAX acts as a 'null' here.
const unsigned long prechargeTimeoutInterval = 10e4; // 10s ---amount of time that has to pass to mean the precharge failed; in millis
const unsigned long prechargeInterval = 1.5e4; // 1.5s -- amount of time necessary to precharge; in millis

unsigned long optocouplerActivatedStart = ULONG_MAX; // The time at which Optocoupler was active; in millis

unsigned long dischargeStart = ULONG_MAX;
const unsigned long dischargeInterval = 7.79e7; // 77.9s; the amount of time it takes to discharge; in millis;


// /**
//  * Author: Ryanne Wilson
//  * BPS Arduino for Props.
//  * 
//  */

// // INCLUDE

// #include <Arduino_CAN.h>

// const int HITEMP_INDEX = 8;
// const int OFFSET = -40;

// void setup(){
//   Serial.begin(115200)
//   while(!Serial);

//   if(!CAN.begin(CanBitRate::BR_250k)){
//     Serial.println("CAN.begin(...) failed.");
//     for(;;) {}
//   }
// }

// // If MPS is HIGH or Hitemp >= 55, Fault (HIGH; 5V) else output LOW; 0V

// void loop(){
//   if(CAN.available()){
//     CanMsg const msg = CAN.read();
//     Serial.println(msg);
//     int idMessage = msg.getStandardid();
//     Serial.print("IDMessage= ");
//     Serial.println(idMessage);

//     // offset -40
//     int temp = msg.data[HITEMP_INDEX];
//     temp -= OFFSET;

//     if (temp >= 55){
//       // Send out a fault!
//     }
    
//   }
// }


void setup() {
  // put your setup code here, to run once:
  pinMode(AIR_Precharge, OUTPUT);     // Normally open- LOW = open, HIGH = closed
  pinMode(AIR_Main, OUTPUT);          // Normally open
  pinMode(AIR_Discharge, OUTPUT);     // Normally open
  //TODO: Add Fault LED
  pinMode(LED_Discharge, OUTPUT);
  pinMode(Optocoupler, INPUT_PULLUP); //Uses internal pullup resistors; default HIGH -> active LOW
  pinMode(LED_Fault, OUTPUT);
  
  pinMode(BMS, INPUT);
  pinMode(Switch2, INPUT);
}

void precharge() {
  /**
  Precharge function for the circuit.

  if precharge failed -> deactivates AIR Precharge.
  if Optocoupler sends signal -> if has been precharging for enough time -> start car; deactivate AIR Precharge.
  */

  unsigned long now = millis();
  if (digitalRead(Optocoupler) == LOW) { //if Optocoupler is active
    
    
    //This part waits for a steady signal from the optocoupler, 
    //current duration .5s but that's just a guess.
    if (optocouplerActivatedStart == ULONG_MAX) optocouplerActivatedStart = now;

    unsigned long chargingInterval = now - optocouplerActivatedStart;

    if (chargingInterval > prechargeInterval) {
      carRunning = true;
      digitalWrite(AIR_Precharge, LOW);
      digitalWrite(AIR_Main, HIGH);
    }
  } 

  //timeout check
  unsigned long timeoutInterval = now - prechargeStart;
  if (timeoutInterval > prechargeTimeoutInterval) { 
    // Timed out; precharge has failed! fault :(
    prechargeFailed = true;
    digitalWrite(AIR_Precharge, LOW);
    digitalWrite(AIR_Main, LOW);
    // digitalWrite(AIR_Discharge, LOW);
    fault();
  }
}

void discharge() {
  digitalWrite(AIR_Discharge, HIGH);
  unsigned long now = millis();
  if (now - dischargeStart > dischargeInterval) {
    digitalWrite(LED_Discharge, HIGH);
    dischargeFinished = true;
  }
}

void fault() {
  //still not sure what this should do
  digitalWrite(LED_Fault, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (carRunning == false) {
      if (digitalRead(Switch2) == HIGH ) { //&& digitalRead(BMS) == HIGH
        if (prechargeStart == ULONG_MAX) {   //if the prechargeStart hasn't yet been assigned         
            digitalWrite(AIR_Precharge, HIGH); //Closes AIR precharge
            prechargeStart = millis();
            }
        precharge();
      }
  }
  // else {
  //   if (!Switch2) {
  //     if (dischargeStart == ULONG_MAX) dischargeStart = millis();
  //     if (dischargeFinished == false) discharge();
  //   }
  // }
}
