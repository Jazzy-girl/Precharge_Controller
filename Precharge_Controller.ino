/**
Authors:
Natu Benyam Demeke
Violet Enslow
Ryanne Wilson


Precharge Controller - Arduino Nano

Feel free to change any of the output pins!
*/

#define MAX_TIMER 4294967295UL // 4294967295UL Serves as a 'null' state for the timers

// 2 TAKEN
// 3 TAKEN
// 4 TAKEN
// 5 TAKEN
// 6 (ANALOG ONLY)
// 7 (ANALOG ONLY)
// 8 TAKEN
// 9 TAKEN
// 10 TAKEN
// 11 TAKEN
// 12 TAKEN
//
// 13
// 14 TAKEN
// 15
// 16
// 17
// 18
// 19
//  output pins
#define AIR_Precharge 4
#define AIR_Main 5
#define AIR_Discharge 14
#define BPS_Fault 8
#define LED_Discharge 3
#define LED_Fault 10

// input pins
#define Optocoupler 9      // input_pullup !
#define BMS_DischargeEn 11 // active low
#define BMS_MPO 12         // BMS Multi-Purpose Output
#define Feather_Thermistor_Fault 2

bool carRunning = false;        // True when the car can start driving ; as in, when precharging has finished and is successful.
bool prechargeFailed = false;   // True if the precharge failed.
bool dischargeFinished = false; // True if the discharge finished!

// All measurements of time are in milliseconds!

unsigned long initalizeStart = MAX_TIMER; // Start time for waiting for Discharge Enable signal from BMS
#define initalizeTimeout 5000             // 5s -- amount of time to wait for Discharge Enable to initalize before faulting

unsigned long prechargeStart = MAX_TIMER;            // The time at which precharge started; in millis; MAX_TIMER acts as a 'null' here.
const unsigned long prechargeTimeoutInterval = 10e4; // 10s ---amount of time that has to pass to mean the precharge failed; in millis
#define prechargeInterval 1500                      // 1.5s -- amount of time necessary to precharge; in millis
bool prechargeTimedOut = false;

unsigned long optocouplerActivatedStart = MAX_TIMER; // The time at which Optocoupler was active; in millis

unsigned long dischargeStart = MAX_TIMER;
#define dischargeInterval 77900 // 77.9s; the amount of time it takes to discharge; in millis;

#define optoHighInterval 500 // amount of time to ensure optocoupler is not just flicking high

unsigned long optoHighStart = MAX_TIMER;

bool optoLow = false;

void setup()
{
  // put your setup code here, to run once:

  // output pins
  pinMode(AIR_Precharge, OUTPUT); // Normally open- LOW = open, HIGH = closed
  pinMode(AIR_Main, OUTPUT);      // Normally open
  pinMode(AIR_Discharge, OUTPUT); // Normally closed

  pinMode(BPS_Fault, OUTPUT); // Normally open ??
  digitalWrite(BPS_Fault, LOW);

  pinMode(LED_Fault, OUTPUT);
  pinMode(LED_Discharge, OUTPUT);

  // input pins
  pinMode(Optocoupler, INPUT_PULLUP); // Uses internal pullup resistors; default HIGH -> active LOW
  pinMode(BMS_MPO, INPUT);            // Behavior depends on BMS settings
  pinMode(BMS_DischargeEn, INPUT);    // ACTIVE LOW
  pinMode(Feather_Thermistor_Fault, INPUT);

  Serial.begin(9600);
  // while(!Serial);  // THIS FOR DEBUGGING. TURN OFF FOR ACTUAL USE.
  Serial.println("");
  Serial.println("Serial works!");

  digitalWrite(AIR_Discharge, HIGH);

  if (digitalRead(BMS_DischargeEn) != LOW)
  {
    initalizeStart = millis();
    while (digitalRead(BMS_DischargeEn) != LOW)
    {
      if (millis() - initalizeStart > initalizeTimeout)
      {
        Serial.println("Waitng for BMS Discharge Enable");
        precharge_fault();
      }
    }
  }
  // if(digitalRead(BMS_DischargeEn)) {
  //   Serial.println("Error: BMS Discharge Enable Timeout");
  //   precharge_fault();
  // }

  Serial.println("End of setup");
}

void precharge()
{
  /**
  Precharge function for the circuit.

  if precharge failed -> deactivates AIR Precharge.
  if Optocoupler sends signal -> if has been precharging for enough time -> start car; deactivate AIR Precharge.
  */

  unsigned long now = millis();
  if (digitalRead(Optocoupler) == LOW)
  {
    optoLow = true;
    optoHighStart = MAX_TIMER;
    if (optocouplerActivatedStart == MAX_TIMER)
    {
      Serial.println("Optocoupler is LOW");
      optocouplerActivatedStart = now;
    }
  }

  if (digitalRead(Optocoupler) == HIGH && optoLow == true && !carRunning)
  {
    if(optoHighStart == MAX_TIMER){
      optoHighStart = millis();
      Serial.println("Optocoupler went HIGH again!");
    }

    if(now - optoHighStart > optoHighInterval){
      Serial.println("Optocoupler read HIGH without flicking for 0.5 seconds and car is running!");
      // if the optocoupler gives a high signal, if the optocoupler has already been low, if the car is not already running:
      digitalWrite(AIR_Main, HIGH);     // Close Main AIR
      Serial.println("CLOSED MAIN AIR");
      delay(3000);                      // wait 3 seconds
      digitalWrite(AIR_Precharge, LOW); // Open Precharge AIR
      Serial.println("OPENED PRECHARGE AIR");
      carRunning = true;                // the car is now running!
    }
  }

  if (optocouplerActivatedStart != MAX_TIMER)
  {
    if (now - optocouplerActivatedStart > 5000)
    {
      Serial.println("optocoupler max timer past. fault time.");
      precharge_fault();
    }
  }
  // if (digitalRead(Optocoupler) == LOW) { //if Optocoupler is active

  //   //This part waits for a steady signal from the optocoupler,
  //   //current duration .5s but that's just a guess.
  //   if (optocouplerActivatedStart == MAX_TIMER) optocouplerActivatedStart = now;

  //   unsigned long chargingInterval = now - optocouplerActivatedStart;

  //   if (chargingInterval > prechargeInterval) {
  //     carRunning = true;
  //     digitalWrite(AIR_Precharge, LOW);
  //     digitalWrite(AIR_Main, HIGH);
  //   }
  // }else { // optocoupler is HIGH (inactive)
  //   optocouplerActivatedStart = MAX_TIMER;
  // }

  // timeout check
  //  unsigned long timeoutInterval = now - prechargeStart;
  //  if (timeoutInterval > prechargeTimeoutInterval) {
  //    // Timed out; precharge has failed! fault :(
  //    prechargeFailed = true;
  //    digitalWrite(AIR_Precharge, LOW);
  //    digitalWrite(AIR_Main, LOW);
  //    digitalWrite(LED_Fault, HIGH);
  //    prechargeTimedOut = true;
  //    precharge_fault();
  //  }
}

// unused.
void discharge()
{
  digitalWrite(AIR_Discharge, HIGH);
  unsigned long now = millis();
  if (now - dischargeStart > dischargeInterval)
  {
    digitalWrite(LED_Discharge, HIGH);
    dischargeFinished = true;
  }
}

void precharge_fault()
{
  Serial.println("PRECHARGE FAULT TRIGGERED (opening MAIN and Precharge)");
  digitalWrite(AIR_Main, LOW);       // open Main AIR
  digitalWrite(AIR_Discharge, HIGH); // close Discharge
  digitalWrite(AIR_Precharge, LOW); // Open Precharge AIR
  while (1)
    ;
}

void bps_fault()
{
  digitalWrite(BPS_Fault, HIGH);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Precharge Fault
  if (digitalRead(BMS_DischargeEn) == HIGH || prechargeTimedOut == true)
  {
    if(digitalRead(BMS_DischargeEn)){
      Serial.println("BMS Discharge High");
    }
    else{
      Serial.println("precharge timed out");
    }
    

    precharge_fault();
  }

  if (digitalRead(BMS_MPO) == LOW || digitalRead(Feather_Thermistor_Fault) == HIGH)
  {
    if (digitalRead(BMS_MPO) == LOW)
    {
      Serial.println("MPO IS LOW");
    }
    if (digitalRead(Feather_Thermistor_Fault) == HIGH)
    {
      Serial.println("Feather Thermistor Fault is HIGH");
    }
    bps_fault();
  }
  else
  {
    digitalWrite(BPS_Fault, LOW);
  }

  if (carRunning == false)
  {
    if (digitalRead(BMS_DischargeEn) == LOW)
    { // ACTIVE LOW
      
      if (prechargeStart == MAX_TIMER)
      {       
        Serial.println("BMS Discharge Low");                             // if the prechargeStart hasn't yet been assigned
        digitalWrite(AIR_Precharge, HIGH); // Closes AIR precharge
        Serial.println("CLOSED AIR PRECHARGE");
        prechargeStart = millis();
      }
      precharge();
    }
  }
}
