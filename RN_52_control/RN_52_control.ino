#include <SoftwareSerial.h>
#define RX1 2
#define TX1 3
#define RX2 4
#define TX2 5

#define GRE_LED 8
#define YEL_LED 9
#define BLU_LED 10
#define RED_LED 11

#define M1_EN 13
#define M2_EN 12

SoftwareSerial mserial_1 =  SoftwareSerial(RX1,TX1);
SoftwareSerial mserial_2 =  SoftwareSerial(RX2,TX2);
char key_input;
char board[500];
char sink1[15];
char sink2[15];
int index = 0;
int mFlag = 0;
int loopControl = 0;
void setup()  
{
  //set all relevent pins as outputs
  pinMode(GRE_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(M1_EN  , OUTPUT);
  pinMode(M2_EN  , OUTPUT);
  pinMode(RX1  , INPUT);
  pinMode(TX1  , OUTPUT);
  pinMode(RX2  , INPUT);
  pinMode(TX2  , OUTPUT);

  //set EN pins to low
  digitalWrite(M1_EN, HIGH);
  digitalWrite(M2_EN, LOW);
  
  //turn on serial interfaces to first module and forward messages to DEBUG
  Serial.begin(115200); 
 Serial.println("modules");
  mserial_1.begin(9600);
  mserial_2.begin(9600);
  
  //GREen led ON
  digitalWrite(GRE_LED, HIGH);
}

void loop() // run over and over
{
  char temp;
  if (loopControl==0){
  Serial.println("Resetting modules in 10 seconds");
  mserial_1.println("r,1");
    mserial_2.println("r,1");
  delay(15000);
    mserial_1.begin(9600);

        delay(2000);
          mserial_1.println("@,1");
  index = 0;
  mFlag = 0;
  //Query Module 1 until connection is established - blink YEL_LED while waiting
  while(mFlag == 0){
    index = 0;                            //reset index
    digitalWrite(YEL_LED, LOW);           //turn off LED
    mserial_1.println("q");                 //send query command to device
   delay(500);
    while(mserial_1.available() > 0)      //while device still has data to input
    {
      temp = mserial_1.read();
      board[index++] =  temp; //read input into buffer
      Serial.write(temp);
    }

    if (index == 0 ) 
          digitalWrite(RED_LED, HIGH);
    else
          digitalWrite(RED_LED, LOW);
    //DEBUG printout
  //  Serial.println("DEBUG");          
  //  Serial.println(index);
  //  Serial.println(mFlag);
  //  for (int i = 0; i<index ; i++)
  //    Serial.write(board[i]);

    digitalWrite(YEL_LED, HIGH);          //turn on LED
    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.println("DEBUG-2");
      Serial.println(board[0]);
      Serial.println(board[1]);
      Serial.println(board[2]);
      Serial.println(board[3]);
      if (board[3] == '3') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      }
    }
    index = 0;                            //reset index

  }
  Serial.println("Connected to BT source");
  digitalWrite(M2_EN, HIGH);              //turn on second module
  delay(10000);
   mserial_2.begin(9600);
      mserial_2.println("u");
      delay(1000);
   mserial_2.println("r,1");
  delay(15000);
    mserial_2.begin(9600);
        delay(2000);
  mFlag = 0;         //reset flag for Module_2 search pattern
  index = 0;
  while (mFlag == 0){
    index = 0;                            //reset index
    digitalWrite(BLU_LED, LOW);           //turn off LED
    mserial_2.println("f");                 //send query command to device
    delay(10000);
    while(mserial_2.available() > 0)      //while device still has data to input
    {
      temp = mserial_2.read();  //read input into buffer
      board[index++] = temp;  //read input into buffer
      Serial.print(temp);
    }
   Serial.print(temp);
    digitalWrite(BLU_LED, HIGH);          //turn on LED
    delay(500);
    for (int i = 0; i < index; i++) {
      if ((board[i] == 'F')  && (board[i+6]==':')) {
        Serial.println("found available connection");
        if (mFlag==0) {
          Serial.println("first ");
          Serial.println(i);
          sink1[0]='c';sink1[1]=',';
          for (int j = 0; j <= 12;j++) {
            sink1[j+2] = board[i+j+7];
          }
          sink1[15]='\0';
          Serial.println("Sending connection commmand on second discoverd device");
          Serial.println(sink1);
          mserial_2.println(sink1);  
          delay(1000);
          mFlag=1;
        } else {
           Serial.println("second ");
           Serial.println(i);
           sink2[0]='c';sink2[1]=',';
          for (int j = 0; j <= 12;j++) {
            sink2[j+2] = board[i+j+7];
          }
          sink2[15]='\0';
            Serial.println("Sending connection commmand on first discoverd device");
            Serial.println(sink2);
            mserial_2.println(sink2);  
            delay(500);
        }
      }
     
    }
  }
  mFlag=0;
  while(mFlag ==0){
    mserial_2.println("q");                 //send query command to device
   delay(500);
    while(mserial_2.available() > 0)      //while device still has data to input
    {
      temp = mserial_2.read();
      board[index++] =  temp; //read input into buffer
      Serial.write(temp);
    }
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.println("DEBUG-2");
      Serial.println(board[0]);
      Serial.println(board[1]);
      Serial.println(board[2]);
      Serial.println(board[3]);
      if (board[3] == '8' || board[3] == '9') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      } else {
        mserial_2.println(sink1);  
          delay(1000);
          mserial_2.println(sink2);
         delay(10000); 
      }
    }
    index = 0;
  }
  
  loopControl = 1;
  while(1){Serial.println("connection was made going to hibernate");delay(5000);};
  }
}

