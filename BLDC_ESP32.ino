const int HA = 17;
const int LA = 5;
const int HB = 18;
const int LB = 23;
const int HC = 19;
const int LC = 22;
const int ENABLE = 16;

/*
 * delay introduction
 * 
 */
 int counter, DEL, EN_DELAY;
 int delta[6];

int BEMFA = 27, BEMFB = 12, BEMFC = 14;

TaskHandle_t Task1;

int trigg = 0;

volatile int n;
int stepnum = 0;
int stepnum2;
int prevstep = 0;
int duty = 40;


const int freq = 35000;
const int ledChannelA = 0;
const int ledChannelB = 1;
const int ledChannelC = 2;
const int resolution = 8; 

int margin = 10;

void stepselect();

float xn1 = 0;
float yn1 = 0;
float CURRENT;

void CUR(){
  int a = analogRead(13);
  int b = analogRead(15);
  int c = analogRead(2);
  int val = a+b+c;
  float xn = (float) val;
  float yn = 0.969*yn1 + 0.0155*xn + 0.0155*xn1;
  xn1 = xn;
  yn1 = yn;
  CURRENT = yn1;
}
  

void All_Low(){  //safety step                        
//Phase C is floating 

   

 ledcWrite(ledChannelA, 0);
 ledcWrite(ledChannelB, 0);
 ledcWrite(ledChannelC, 0);

 digitalWrite(LA, LOW);
 digitalWrite(LB, LOW);
 digitalWrite(LC, LOW);

 
}

void AH_BL(int duty){                          
//Phase C is floating 

 ledcWrite(ledChannelA, duty);
 ledcWrite(ledChannelB, 0);
 ledcWrite(ledChannelC, 0);


 digitalWrite(LA, HIGH);
 digitalWrite(LB, HIGH);
 digitalWrite(LC, LOW);
}

void AH_CL(int duty){
//Phase B is floating 

 ledcWrite(ledChannelA, duty);
 ledcWrite(ledChannelB, 0);
 ledcWrite(ledChannelC, 0);


 digitalWrite(LA, HIGH);
 digitalWrite(LB, LOW);
 digitalWrite(LC, HIGH);
}
void BH_CL(int duty){
// Phase A is floating

 ledcWrite(ledChannelA, 0);
 ledcWrite(ledChannelB, duty);
 ledcWrite(ledChannelC, 0);

 digitalWrite(LA, LOW);
 digitalWrite(LB, HIGH);
 digitalWrite(LC, HIGH);
}
void BH_AL(int duty){
// phase C is floating

 ledcWrite(ledChannelA, 0);
 ledcWrite(ledChannelB, duty);
 ledcWrite(ledChannelC, 0);
 
 digitalWrite(LA, HIGH);
 digitalWrite(LB, HIGH);
 digitalWrite(LC, LOW);
}
void CH_AL(int duty){
//Phase B is floating

 ledcWrite(ledChannelA, 0);
 ledcWrite(ledChannelB, 0);
 ledcWrite(ledChannelC, duty);
 
 digitalWrite(LA, HIGH);
 digitalWrite(LB, LOW);
 digitalWrite(LC, HIGH);
 
}
void CH_BL(int duty){

//Phase A is floating

 ledcWrite(ledChannelA, 0);
 ledcWrite(ledChannelB, 0);
 ledcWrite(ledChannelC, duty);
  
 digitalWrite(LA, LOW);
 digitalWrite(LB, HIGH);
 digitalWrite(LC, HIGH);
}




void IRAM_ATTR detectA(){
  prevstep = stepnum;
  n = 0;
  for(int i = 0; i < margin; i++){ //check the pin 100 times to be sure of the state
  if (digitalRead(BEMFA)){
    n = n+1;   
  }
  if (!digitalRead(BEMFA)){
    n = n+4;
  }
  }
n = n/margin; 
stepnum = n; 
INTERRUPT_SELECT(stepnum, prevstep);
}

void IRAM_ATTR detectB(){
  prevstep = stepnum;
  n = 0;
  for(int i = 0; i < margin; i++){
  if (digitalRead(BEMFB)){
    n = n+3;   
  }
  if (!digitalRead(BEMFB)){
    n = n+6;
  }
  }
n = n/margin;
stepnum = n;
INTERRUPT_SELECT(stepnum, prevstep);
}

void IRAM_ATTR detectC(){
  prevstep = stepnum;
  n = 0;
  for(int i = 0; i < margin; i++){
  if (digitalRead(BEMFC)){
    n = n+5;   
  }
  if (!digitalRead(BEMFC)){
    n = n+2;
  }
  }
n = n/margin;  
stepnum = n;
INTERRUPT_SELECT(stepnum, prevstep);
}

int EN = 0;

void AmesureRISE(){
  detachInterrupt(digitalPinToInterrupt(BEMFB));
  detachInterrupt(digitalPinToInterrupt(BEMFC));
  attachInterrupt(digitalPinToInterrupt(BEMFA), detectA, RISING); 
}

void AmesureFALL(){
  detachInterrupt(digitalPinToInterrupt(BEMFB));
  detachInterrupt(digitalPinToInterrupt(BEMFC));
  attachInterrupt(digitalPinToInterrupt(BEMFA), detectA, FALLING); 
}
void BmesureRISE(){
  
  detachInterrupt(digitalPinToInterrupt(BEMFC));
  detachInterrupt(digitalPinToInterrupt(BEMFA));
  attachInterrupt(digitalPinToInterrupt(BEMFB), detectB, RISING); 
}
void BmesureFALL(){
  
  detachInterrupt(digitalPinToInterrupt(BEMFC));
  detachInterrupt(digitalPinToInterrupt(BEMFA));
  attachInterrupt(digitalPinToInterrupt(BEMFB), detectB, FALLING); 
}
void CmesureRISE(){

  detachInterrupt(digitalPinToInterrupt(BEMFB));
  detachInterrupt(digitalPinToInterrupt(BEMFA));
  attachInterrupt(digitalPinToInterrupt(BEMFC), detectC, RISING); 
}
void CmesureFALL(){

  detachInterrupt(digitalPinToInterrupt(BEMFB));
  detachInterrupt(digitalPinToInterrupt(BEMFA));
  attachInterrupt(digitalPinToInterrupt(BEMFC), detectC, FALLING); 
}


void INTERRUPT_SELECT(int stepnum, int prevstep){
    switch(stepnum){
    case 1:
    trigg = 1;
    stepnum2 = 1;
    delta[0] = counter;
    counter = 0;
    CmesureFALL();
    break;
    case 2:
    trigg = 1;
    stepnum2 = 2;
    delta[1] = counter;
    counter = 0;
    BmesureRISE();
    break;
    case 3:
    trigg = 1;
    stepnum2 = 3;
    delta[2] = counter;
    counter = 0;
    AmesureFALL();
    break;
    case 4:
    trigg = 1;
    stepnum2 = 4;
    delta[3] = counter;
    counter = 0;
    CmesureRISE();
    break;
    case 5:
    trigg = 1;
    stepnum2 = 5;
    delta[4] = counter;
    counter = 0;
    BmesureFALL();
    break;
    case 6:
    trigg = 1;
    stepnum2 = 6;
    delta[5] = counter;
    DEL = (delta[0]+delta[1]+delta[2]+delta[3]+delta[4]+delta[5])/15;
    counter = 0;
    AmesureRISE();
    break;
    default:
    stepnum = prevstep; //this operation may be totally useless
    break;
  }
}

void POWER_SELECT(int x, int trigg, int duty, int EN){  //when no control delay is applied
  if(EN == 1 && trigg == 1){
    trigg = 0;
    switch(x){
    case 0:
    All_Low();
    break;
    case 1:
    AH_BL(duty);
    break;
    case 2:
    AH_CL(duty);
    break;
    case 3:
    BH_CL(duty);
    break;
    case 4:
    BH_AL(duty);
    break;
    case 5:
    CH_AL(duty);
    break;
    case 6:
    CH_BL(duty);
    break;
  }
  }
  else if (EN == 0){
    All_Low();
  }
}

void POWER_SELECT_DELAY(int x, int C, int DEL, int duty, int EN){
  if(EN == 1 && C == DEL){
    switch(x){
    case 0:
    All_Low();
    break;
    case 1:
    AH_BL(duty);
    break;
    case 2:
    AH_CL(duty);
    break;
    case 3:
    BH_CL(duty);
    break;
    case 4:
    BH_AL(duty);
    break;
    case 5:
    CH_AL(duty);
    break;
    case 6:
    CH_BL(duty);
    break;
  }
  }
  else if (EN == 0){
    trigg = 0;
    All_Low();
  }
}

/*
int preb_state = 0;


void SECURITAS(int b_state, int preb_state, int EN){
  if(b_state == 0 && preb_state == 1){
    EN = !EN;
    digitalWrite(25, EN);
  }
  preb_state = b_state;
}*/


int prevstate;
void FALL_DETECT1(int x, int y){
  if(y == 1){
    if(x == 0){
      EN = !EN;
    }
  }
  prevstate = x;
}
int prevstate2;
void FALL_DETECT2(int x, int y){
  if(y == 1){
    if(x == 0){
      EN_DELAY = !EN_DELAY;
    }
  }
  prevstate2 = x;
}


void setup() {
  // put your setup code here, to run once:
pinMode(BEMFA, INPUT_PULLUP);
pinMode(BEMFB, INPUT_PULLUP);
pinMode(BEMFC, INPUT_PULLUP);

Serial.begin(115200);

pinMode(LA, OUTPUT);
pinMode(LB, OUTPUT); 
pinMode(LC, OUTPUT);
pinMode(ENABLE, OUTPUT);
digitalWrite(ENABLE, HIGH);

pinMode(25, OUTPUT);
pinMode(26, OUTPUT);
//pinMode(32, INPUT_PULLUP);
//pinMode(33, INPUT_PULLUP);

pinMode(0, INPUT_PULLUP);
pinMode(4, INPUT_PULLUP);

ledcSetup(ledChannelA, freq, resolution);
ledcSetup(ledChannelB, freq, resolution);
ledcSetup(ledChannelC, freq, resolution);

ledcAttachPin(HA, ledChannelA);
ledcAttachPin(HB, ledChannelB);
ledcAttachPin(HC, ledChannelC);
  
All_Low();
CmesureFALL();
 //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
 xTaskCreatePinnedToCore(
                   Task1code,   /* Task function. */
                   "Task1",     /* name of task. */
                   10000,       /* Stack size of task */
                 NULL,        /* parameter of the task */
                  1,           /* priority of the task */
                   &Task1,      /* Task handle to keep track of created task */
                  0);          /* pin task to core 0 */                  
  delay(500); 

 
}





//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){

  for(;;){
    int i = digitalRead(4);
    int j = digitalRead(0 );
    duty = analogRead(33);
    duty = map(duty, 0, 4096, 0, 255);
    FALL_DETECT1(i, prevstate);
    FALL_DETECT2(j, prevstate2);
    CUR();
    Serial.println(duty);
    digitalWrite(25, EN);
    digitalWrite(26, EN_DELAY);
    delay(10);   // SEEM'S NECESSARY, BUT WHY?
  } 
}




void loop() {
  // put your main code here, to run repeatedly:
if(!EN_DELAY){
  POWER_SELECT(stepnum2, trigg, duty, EN);
}
else if(EN_DELAY){
  POWER_SELECT_DELAY(stepnum2, counter, DEL, duty, EN);
}
counter++;
}
