#include<PS2X_lib.h>
#include <Servo.h>


//PS2設定
PS2X ps2x;
int error = 0;
byte type = 0;
byte vibrate = 0;
int tmp = 0;

 //發射器板機伺服馬達
Servo trigger;
//砲管調角度伺服馬達
Servo cannon;

//設定手把
int psdata = 8;
int pscmd = 9;
int psclk = 10;
int psatt = 11;
//設定馬達
//右輪馬達
int rightIn1 = 7;
int rightIn2 = 6;
int rightPWM = 3;
//左輪馬達
int leftIn1 = 13;
int leftIn2 = 12;
int leftPWM = 5;
//砲台馬達
int cannon_motor1 = 2;
int cannon_motor2 = A3;
int cannon_PWM = A4;
//砲台伺服馬達
//卡榫伺服馬達
//紅外線感測
const int L_IRsensor = A0;
const int C_IRsensor = 4;
const int R_IRsensor = A1;
int Ldigit, Cdigit, Rdigit;
//模式切換，true為非0，false為0
bool mode = true;


void move_(int right_speed, int left_speed);
void IRsensor (int L,int C,int R);
void setup() {
  // put your setup code here, to run once:
    //手把角位設定
    pinMode(psdata,OUTPUT);
    pinMode(pscmd,OUTPUT);
    pinMode(psatt,OUTPUT);
    pinMode(psclk,OUTPUT);
    //右邊輪子馬達
    pinMode(rightIn1,OUTPUT);
    pinMode(rightIn2,OUTPUT);
    pinMode(rightPWM,OUTPUT);
    //左邊輪子馬達
    pinMode(leftIn1,OUTPUT);
    pinMode(leftIn2,OUTPUT);
    pinMode(leftPWM,OUTPUT);
    //感測器
    pinMode(L_IRsensor,INPUT);
    pinMode(C_IRsensor,INPUT);
    pinMode(R_IRsensor,INPUT);
    //砲台馬達
    pinMode(cannon_motor1,OUTPUT);
    pinMode(cannon_motor2,OUTPUT);
    pinMode(cannon_PWM,OUTPUT);
    
    //卡榫設定
     trigger.attach(A2);  
     trigger.write(90);                              
     delay(1000); 
      
     //砲台伺服設定
     cannon.attach(A5);
     cannon.write(150);
     delay(1000);
     
     Serial.begin(57600);
     
     
    
    //手把設定程式
    Serial.begin(57600);
    Serial.println("start wire");
    do
  {
    error = ps2x.config_gamepad(psclk, pscmd, psatt, psdata, true, true);
    if (error == 0) 
    {
      Serial.println("Controller has been connected");
    }
          Serial.println("qq");

  }
  while(error!=0);
  
  type = ps2x.readType();

  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }
  
}


void loop() {
  if(mode == true)
  {
     Ldigit = digitalRead(L_IRsensor);
     Cdigit = digitalRead(C_IRsensor);
     Rdigit = digitalRead(R_IRsensor);
     if(Ldigit == 1)
        Serial.println("左邊黑色");
     else
        Serial.println("左邊白色");   
       
     if(Rdigit == 1)
        Serial.println("右邊黑色");
     else
        Serial.println("右邊白色");    
    IRsensor (Ldigit, Cdigit, Rdigit);
   
  }
  //若布林值為true，則執行手把程式
    //手把按鈕設定  
    //設定馬達以vibrate 速度震動
    ps2x.read_gamepad(false, vibrate);
    //下方為按鈕指令
    //前進
    if (ps2x.Button(PSB_PAD_UP)) {
      Serial.println("pad up");
      move_(150,150);
  }
    //後退
    else if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
        Serial.println("pad down");
        move_(-200,-150);
    }
    //右轉
    else if (ps2x.Button(PSB_PAD_RIGHT)) {
        Serial.println("pad right");
        move_(-150,150);
    
    }
    //左轉  
    else if (ps2x.Button(PSB_PAD_LEFT)) {
        Serial.println("pad left");
        move_(150,-150);
    }

    //停止所有馬達
    else if (ps2x.Button(PSB_RED)){
        Serial.println("red");
        move_(0,0);
        digitalWrite(cannon_motor1, LOW);
        digitalWrite(cannon_motor2, HIGH);
        analogWrite(cannon_PWM,0);
        
        
    }
    //發射器馬達轉緊
    else if (ps2x.Button(PSB_GREEN)){
        Serial.println("green");
        digitalWrite(cannon_motor1, HIGH);
        digitalWrite(cannon_motor2, LOW);
        analogWrite(cannon_PWM,255);
    }
    //放鬆發射器馬達
    else if(ps2x.Button(PSB_PINK)){
        Serial.println("pink");
        digitalWrite(cannon_motor1, LOW);
        digitalWrite(cannon_motor2, HIGH);
        analogWrite(cannon_PWM,255);
    }
    //發射器板機垂直（卡住）
    else if(ps2x.Button(PSB_R1)){ 
      Serial.println("R1");
      trigger.write(0);
                
    }
    //發射器板機水平（鬆開板機）
    else if (ps2x.Button(PSB_R2)){
      Serial.println("R2");
      trigger.write(90);

    }
    //發射器砲管調角度
    else if(ps2x.Button(PSB_L1)){
      Serial.println("L1");
      cannon.write(120);
    }
    else if(ps2x.Button(PSB_L2)){
      Serial.println("L1");
      cannon.write(150);
    }
    //切換模式
    else if(ps2x.Button(PSB_BLUE)){
        Serial.println("blue");
        if(mode == true){
          mode = false;
        }
        else {
          mode = true;
        }
        Serial.println(mode);
    }
    else {
    
      Serial.println("no cmd");
     }
    delay(50);
  
  
     
      
  
}

//車子底部左右輪
void move_(int right_speed, int left_speed)
{
  //若r_speed ＝ 0, 還是會向前轉，但速度是零
  // r_s = 255, l_s = 255：往前, r_s = -255, l_s = -255：往後
  // r_s = 255, l_s = -255：往左, r_s = -255, l_s = 255：往右
  if (right_speed >= 0)
  {
    digitalWrite(rightIn1, HIGH);
    digitalWrite(rightIn2, LOW);
  }
  else
  {
    digitalWrite(rightIn1, LOW);
    digitalWrite(rightIn2, HIGH);
  }
  
  if (left_speed >= 0)
  {
    digitalWrite(leftIn1, HIGH);
    digitalWrite(leftIn2, LOW);
  }
  else
  {
    digitalWrite(leftIn1, LOW);
    digitalWrite(leftIn2, HIGH);
  }
  // 控制速度的程式
  analogWrite(rightPWM, abs(left_speed));
  analogWrite(leftPWM, abs(left_speed));
  
}
void IRsensor (int L,int C,int R)  {
  
//1為黑，0為白 
  int counter = 0;
  //前進
  if ((L==0 and C==1 and R==0) or (L==1 and C==0 and R==1) or (L==0 and C==1 and R==1) or (L==1 and C==1 and R==0))
  {
      
      move_(200,200);
      delay(150);
      move_(0,0);
      delay(150);
      
          while(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
          {
             delay(150);
             
             move_(-150,-150);
             delay(150);
             move_(0,0);
             delay(150);
             //先右轉
             if(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
             {
                move_(-150, 150);
                delay(150);
                move_(0,0);
                delay(150);
                //再左轉
                if(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
             {
                      move_(170,-170);
                      delay(150);
                      move_(0,0);
                      delay(50);
             }

          
          }
        }
  }
          
      
  //右轉
  if(L==0 and C==0 and R==1)
  {
      delay(150);    
      move_(-150, 200);
      delay(150);
      move_(0,0);
      delay(150);
      //若超出範圍就左轉    
          while(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
          {
             delay(150);
             counter++;
             move_(150,0);
             delay(150);
             move_(0,0);
             delay(150);
             
             //再右轉
             if(counter>=2)
             {
                 move_(-150,150);
                 delay(150);
                 move_(0,0);
                 delay(50);
                 break;
                 
             }
             
          
          }
          
  }
      
  //左轉 
  if(L==1 and C==0 and R==0)
  {
      delay(150);
      move_(220, -170);
      delay(150);
      move_(0,0);
      delay(150);
      //若超出範圍就右轉    
          while(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
          {
            delay(150);
             counter++;
             move_(0,200);
             delay(150);
             move_(0,0);
             delay(150);
             
             if(counter>=2)
             {
                 move_(150,-150);
                 delay(150);
                 move_(0,0);
                  delay(50);
                 break;
                 
             }
             
          }
  }   
  //停止    
  if(L==1 and C==1 and R==1)
  {
    move_(0, 0);
    delay(150);
  }
  
  while(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
          {
             delay(150);
             
             move_(-150,-150);
             delay(150);
             move_(0,0);
             delay(150);
             //先右轉
             if(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
             {
                move_(-150, 150);
                delay(150);
                move_(0,0);
                delay(150);
                //再左轉
                if(digitalRead(L_IRsensor)==0 and digitalRead(C_IRsensor)==0 and digitalRead(R_IRsensor)==0)
                {
                      move_(200,-170);
                      delay(150);
                      move_(0,0);
                      delay(50);
                }
             }
             
  }
  
}     
    
      
     
      
       
      
