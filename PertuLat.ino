#include "Nintendo.h" // For GC Mode

// light sensor
#define LIGHT_SENSOR_PIN 11
#define LIGHT 0
#define NO_LIGHT 1

//button
#define BUTTON_PIN 10
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

//GC Stuff
#define GC_PIN 5
CGamecubeConsole GamecubeConsole(GC_PIN);
Gamecube_Data_t data = defaultGamecubeData;
CGamecubeController GamecubeController1(7);
unsigned int actionCounter = 0;
bool testReady = 0;

//modes
#define PC_MODE 0
#define GC_MODE 1

//Mode var
unsigned int mode = PC_MODE;

// used in tests to get time when test starts
unsigned long startTime;
// used in tests to get time when sensor responded to pixel changes
unsigned long endTime;
// variable for reading the light sensor state
bool lightSensorState = 0;

//Control vars
float click = -1.0;
byte * bClick = (byte *) &click;
float gcWriteError = -2.0;
byte * bGcWriteError = (byte *) &gcWriteError;

void setup() {
  // initialize serial communication at 19200 bits per second:
  Serial.begin(19200);
  // initialize light sensor pin as an input
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);

  //Initialize button as input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(digitalRead(BUTTON_PIN) == BUTTON_PRESSED){
    GC_Mode_Setup();

  }
  else{
    for(int i = 0; i < 2; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(750);
    }
  }
}

void GC_Mode_Setup()
{
  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  TCCR1B |= B00000101;  // Prescaler = 256
  OCR1A = 62500;        // Timer Compare1A Register
  TIMSK1 |= B00000010;  // Enable Timer COMPA Interrupt

  GamecubeController1.read();

  for(int i = 0; i < 2; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  data.report.a = 0;
  data.report.b = 0;
  data.report.x = 0;
  data.report.y = 0;
  data.report.ddown = 0;
  data.report.dleft = 0;
  data.report.dright = 0;
  data.report.dup = 0;

  mode = GC_MODE;
}

ISR(TIMER1_COMPA_vect)
{
  // Handle The Timer Interrupt
  if(actionCounter == 0){ //Fake a press because it gets skipped
    actionCounter++;
    data.report.a = 1;
    OCR1A += 7813; // 0.5sec
  }else if(actionCounter== 1){ //Fake a press release
    actionCounter++;
    data.report.a = 0;
    OCR1A += 15624; //1sec
  }else if(actionCounter == 2){ //Dpad Right
    actionCounter++;
    data.report.dright = 1;
    OCR1A += 15624; //1sec
  }else if(actionCounter == 3){ //Dpad Up
    actionCounter++;
    data.report.dright = 0;
    data.report.dup = 1;
    OCR1A += 15624; //1sec
  }else if(actionCounter == 4){ //A - Enter Games & More
    actionCounter++;
    data.report.dup = 0;
    data.report.a = 1;
    OCR1A += 15624; //1sec
  }else if(actionCounter == 5){ //Dpad Down
    actionCounter++;
    data.report.a = 0;
    data.report.ddown = 1;
    OCR1A += 1563; // 0.1sec
  }else if(actionCounter == 6){ //Dpad Left
    actionCounter++;
    data.report.ddown = 0;
    data.report.dleft = 1;
    OCR1A += 15624; // 1sec
  }else if(actionCounter == 7){ //A - Enter training mode
    actionCounter++;
    data.report.dleft = 0;
    data.report.a = 1;
    OCR1A += 46875; // 3sec
  }else if(actionCounter == 8){ //Dpad right
    actionCounter++;
    data.report.a = 0;
    data.report.dright = 1;
    OCR1A += 14062; // 0.9sec
  }else if(actionCounter == 9){ //Dpad down
    actionCounter++;
    data.report.dright = 0;
    data.report.ddown = 1;
    OCR1A += 3906; // 0.25sec
  }else if(actionCounter == 10){ //X - change to battlefield form
    actionCounter++;
    data.report.ddown = 0;
    data.report.x = 1;
    OCR1A += 3906; // 0.25sec
  }else if(actionCounter == 11){ //release X
    actionCounter++;
    data.report.x = 0;
    OCR1A += 3906; // 0.25sec
  }else if(actionCounter == 12){ //X - change to omega form
    actionCounter++;
    data.report.x = 1;
    OCR1A += 3906; // 0.25sec
  }else if(actionCounter == 13){ //A - choose 75m omega
    actionCounter++;
    data.report.x = 0;
    data.report.a = 1;
    OCR1A += 62500; // 4sec
  }else if(actionCounter == 14){ //Wait extented
    actionCounter++;
    OCR1A += 62500; // 4sec
  }else if(actionCounter == 15){ //Dpad up
    actionCounter++;
    data.report.a = 0;
    data.report.dup = 1;
    OCR1A += 7032; // 0.45sec
  }else if(actionCounter == 16){ //Dpad right
    actionCounter++;
    data.report.dup = 0;
    data.report.dright = 1;
    OCR1A += 17968; // 1.15sec
  }else if(actionCounter == 17){ //A - Pick Little Mac
    actionCounter++;
    data.report.dright = 0;
    data.report.a = 1;
    OCR1A += 3906; // 0.25sec
  }else if(actionCounter == 18){ //Start - Start Match
    actionCounter++;
    data.report.a = 0;
    data.report.start = 1;
    OCR1A += 62500; // 4sec
  }else if(actionCounter == 19){ //Wait for match to load
    actionCounter++;
    data.report.start = 0;
    digitalWrite(LED_BUILTIN, HIGH);
    while(digitalRead(BUTTON_PIN) == BUTTON_RELEASED){
      GamecubeConsole.write(data);
    }
    digitalWrite(LED_BUILTIN, LOW);
    testReady = 1;
    TIMSK1 = B00000000; //Disable timer interrupts
  }
}

void GC_loop(){
  if(testReady){
    data.report.r = 1;
    data.report.right = 200; //Analog R

    lightSensorState = digitalRead(LIGHT_SENSOR_PIN); // get state of light sensor
    startTime = micros();
 
    // wait until sensor's state is changed
    while(lightSensorState == NO_LIGHT){
      GamecubeConsole.write(data);
      lightSensorState = digitalRead(LIGHT_SENSOR_PIN); // get state of light sensor
    }
    
    endTime = micros();

    data.report.r = 0;
    data.report.right = 35; // Analog R
    
    float responseTime = (endTime - startTime)/1000.0;
    byte * b = (byte *) &responseTime;
    Serial.write(b,4);

    // wait 1000 polls (around 1 second. Depends on the adapter polling rate)
    for(int i = 0; i < 1000; i++){
      GamecubeConsole.write(data);
    }
  }
  else{
    // Write controller data to the console
    if (!GamecubeConsole.write(data)){
      Serial.write(bGcWriteError,4);
      delay(1000);
    }
  }
}

void PC_loop(){
  // turn LED on:
  digitalWrite(LED_BUILTIN, HIGH);

  lightSensorState = digitalRead(LIGHT_SENSOR_PIN); // get state of light sensor
  
  Serial.write(bClick,4);
  startTime = micros();

  // wait until sensor's state is changed
  while(lightSensorState == NO_LIGHT)
  {
    lightSensorState = digitalRead(LIGHT_SENSOR_PIN); // get state of light sensor
  }
  
  endTime = micros();
  Serial.write(bClick,4);
  
  float responseTime = (endTime - startTime)/1000.0;
  byte * b = (byte *) &responseTime;
  Serial.write(b,4);

  digitalWrite(LED_BUILTIN, LOW);
  delay(random(400, 600));
}

void loop() {
  if(mode == GC_MODE){
    GC_loop();
  }
  else{
    PC_loop();
  }
}
