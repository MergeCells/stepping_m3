
#define TIMER_OUT 0

#define MA 10
#define MB 11
#define MC 12
#define MD 13

#define NA A0
#define NB A1
#define NC A2
#define ND A3

#define SA 8
#define SB 5

double constHz = 4;

double dltHz = 0.1;
double initHz = 0.2;

// 70分でオーバーフローすることに注意
unsigned long timerM = 0;
unsigned long timerN = 0;
unsigned long pitchM = 0;
unsigned long pitchN = 0;

// 2000us未満で1-2相ステップを回すと脱調が顕著
unsigned long pitchMax = 2000;

bool signM = false;
bool signN = false;

double delsec = 5 / constHz * 1000;
double delsec2 = delsec / 2;
double waitTime;

int phaseM = 0;
int phaseN = 0;
// phase : 00 ~ 35
// x0 : 1phase   x5 : 2phase

int count = 0;
long timeC = 0;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2);
  pinMode(SA, INPUT_PULLUP);  pinMode(SB, INPUT_PULLUP);
  
  pinMode(MA, OUTPUT);  pinMode(MB, OUTPUT);  pinMode(MC, OUTPUT);  pinMode(MD, OUTPUT);
  pinMode(NA, OUTPUT);  pinMode(NB, OUTPUT);  pinMode(NC, OUTPUT);  pinMode(ND, OUTPUT);
  digitalWrite(MA, LOW);  digitalWrite(MB, LOW);  digitalWrite(MC, LOW);  digitalWrite(MD, LOW);
  digitalWrite(NA, LOW);  digitalWrite(NB, LOW);  digitalWrite(NC, LOW);  digitalWrite(ND, LOW);
  delay(100);
  
  waitTime = 1000;
  phaseM = 0;
  phaseN = 0;
  
  pitchM = 4000;
  pitchN = 3000;

#if TIMER_OUT
  timeC = millis();
  timerM = timerN = micros();
#endif
}


void loop() {
  
  if(micros() - timerM > pitchM){
    sgdbPhaseM(signM);
    timerM += pitchM;
  }
  
  if(micros() - timerN > pitchN){
    sgdbPhaseN(signN);
    timerN += pitchN;
  }
  
  
  if(Serial.available()){
    pitchM = Serial.parseInt() / 2;
  }
  
  if(!digitalRead(SA)){
    noPower();
    while(digitalRead(SB))  delay(10);
    timerM = timerN = micros();
  }
#if TIMER_OUT
  if(count++ > 10000){
    Serial.println((millis() - timeC));
    timeC = millis();
    count = 0;
  }
#endif
}

/*
 * レ　スピード指定で左右独立に回転させよう
 * レ　まずはスムージングなしで開発しよう
 * 
 * シリアル通信で指令を受け取ろう
 * 車輪径車輪間隔のパラメータ指定で速度角度指定して動かせるようにしよう
 * 
 * カウンタを入れよう
 * パラメータから理想状態の自己位置を出力しよう
 * 
 * スムージングしながら正確に移動量を指定しよう
*/

void noPower(){
  digitalWrite(MA, LOW);  digitalWrite(MB, LOW);  digitalWrite(MC, LOW);  digitalWrite(MD, LOW);
  digitalWrite(NA, LOW);  digitalWrite(NB, LOW);  digitalWrite(NC, LOW);  digitalWrite(ND, LOW);
}

/*** motorM ***/
void sgPhaseM(bool rot){
  if(rot){
    phaseM += 10;
    if(phaseM % 10) phaseM -= 5;
    if(phaseM > 35) phaseM -= 40;
  }else{
    phaseM -= 10;
    if(phaseM % 10) phaseM += 5;
    if(phaseM < 0) phaseM += 40;
  }
  switch(phaseM){
    case 0:     phaseM1A();      break;
    case 10:    phaseM1B();      break;
    case 20:    phaseM1C();      break;
    case 30:    phaseM1D();      break;
    default:    phaseM = 0;
  }
}

void dbPhaseM(bool rot){
  if(rot){
    phaseM += 10;
    if(!phaseM % 5) phaseM -= 5;
    if(phaseM > 35) phaseM -= 40;
  }else{
    phaseM -= 10;
    if(phaseM % 5) phaseM += 5;
    if(phaseM < 0) phaseM += 40;
  }
  switch(phaseM){
    case 5:     phaseM2AB();      break;
    case 15:    phaseM2BC();      break;
    case 25:    phaseM2CD();      break;
    case 35:    phaseM2DA();      break;
    default:    phaseM = 5;
  }
}

void sgdbPhaseM(bool rot){
  if(rot){
    phaseM += 5;
    if(phaseM > 35) phaseM -= 40;
  }else{
    phaseM -= 5;
    if(phaseM < 0) phaseM += 40;
  }
  switch(phaseM){
    case 0:     phaseM1A();     break;
    case 5:     phaseM2AB();    break;
    case 10:    phaseM1B();     break;
    case 15:    phaseM2BC();    break;
    case 20:    phaseM1C();     break;
    case 25:    phaseM2CD();    break;
    case 30:    phaseM1D();     break;
    case 35:    phaseM2DA();    break;
    default:    phaseM = 0;
  }
}
/* motorM */

/*** motorN ***/
void sgPhaseN(bool rot){
  if(rot){
    phaseN += 10;
    if(phaseN % 10) phaseN -= 5;
    if(phaseN > 35) phaseN -= 40;
  }else{
    phaseN -= 10;
    if(phaseN % 10) phaseN += 5;
    if(phaseN < 0) phaseN += 40;
  }
  switch(phaseN){
    case 0:     phaseN1A();      break;
    case 10:    phaseN1B();      break;
    case 20:    phaseN1C();      break;
    case 30:    phaseN1D();      break;
    default:    phaseN = 0;
  }
}

void dbPhaseN(bool rot){
  if(rot){
    phaseN += 10;
    if(!phaseN % 5) phaseN -= 5;
    if(phaseN > 35) phaseN -= 40;
  }else{
    phaseN -= 10;
    if(phaseN % 5) phaseN += 5;
    if(phaseN < 0) phaseN += 40;
  }
  switch(phaseN){
    case 5:     phaseN2AB();      break;
    case 15:    phaseN2BC();      break;
    case 25:    phaseN2CD();      break;
    case 35:    phaseN2DA();      break;
    default:    phaseN = 5;
  }
}

void sgdbPhaseN(bool rot){
  if(rot){
    phaseN += 5;
    if(phaseN > 35) phaseN -= 40;
  }else{
    phaseN -= 5;
    if(phaseN < 0) phaseN += 40;
  }
  switch(phaseN){
    case 0:     phaseN1A();     break;
    case 5:     phaseN2AB();    break;
    case 10:    phaseN1B();     break;
    case 15:    phaseN2BC();    break;
    case 20:    phaseN1C();     break;
    case 25:    phaseN2CD();    break;
    case 30:    phaseN1D();     break;
    case 35:    phaseN2DA();    break;
    default:    phaseN = 0;
  }
}
/* motorN */


/*** motor M ***/
void phaseM1A(){  digitalWrite(MA, HIGH);  digitalWrite(MB, LOW);  digitalWrite(MC, LOW);  digitalWrite(MD, LOW);}
void phaseM1B(){  digitalWrite(MA, LOW);  digitalWrite(MB, HIGH);  digitalWrite(MC, LOW);  digitalWrite(MD, LOW);}
void phaseM1C(){  digitalWrite(MA, LOW);  digitalWrite(MB, LOW);  digitalWrite(MC, HIGH);  digitalWrite(MD, LOW);}
void phaseM1D(){  digitalWrite(MA, LOW);  digitalWrite(MB, LOW);  digitalWrite(MC, LOW);  digitalWrite(MD, HIGH);}

void phaseM2AB(){  digitalWrite(MA, HIGH);  digitalWrite(MB, HIGH);  digitalWrite(MC, LOW);  digitalWrite(MD, LOW);}
void phaseM2BC(){  digitalWrite(MA, LOW);  digitalWrite(MB, HIGH);  digitalWrite(MC, HIGH);  digitalWrite(MD, LOW);}
void phaseM2CD(){  digitalWrite(MA, LOW);  digitalWrite(MB, LOW);  digitalWrite(MC, HIGH);  digitalWrite(MD, HIGH);}
void phaseM2DA(){  digitalWrite(MA, HIGH);  digitalWrite(MB, LOW);  digitalWrite(MC, LOW);  digitalWrite(MD, HIGH);}
/* motor M */

/*** motor N ***/
void phaseN1A(){  digitalWrite(NA, HIGH);  digitalWrite(NB, LOW);  digitalWrite(NC, LOW);  digitalWrite(ND, LOW);}
void phaseN1B(){  digitalWrite(NA, LOW);  digitalWrite(NB, HIGH);  digitalWrite(NC, LOW);  digitalWrite(ND, LOW);}
void phaseN1C(){  digitalWrite(NA, LOW);  digitalWrite(NB, LOW);  digitalWrite(NC, HIGH);  digitalWrite(ND, LOW);}
void phaseN1D(){  digitalWrite(NA, LOW);  digitalWrite(NB, LOW);  digitalWrite(NC, LOW);  digitalWrite(ND, HIGH);}

void phaseN2AB(){  digitalWrite(NA, HIGH);  digitalWrite(NB, HIGH);  digitalWrite(NC, LOW);  digitalWrite(ND, LOW);}
void phaseN2BC(){  digitalWrite(NA, LOW);  digitalWrite(NB, HIGH);  digitalWrite(NC, HIGH);  digitalWrite(ND, LOW);}
void phaseN2CD(){  digitalWrite(NA, LOW);  digitalWrite(NB, LOW);  digitalWrite(NC, HIGH);  digitalWrite(ND, HIGH);}
void phaseN2DA(){  digitalWrite(NA, HIGH);  digitalWrite(NB, LOW);  digitalWrite(NC, LOW);  digitalWrite(ND, HIGH);}
/* motor N */





