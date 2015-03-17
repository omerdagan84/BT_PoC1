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
void reset_source(){
  Serial.println("TRACE: reset_source called...");
  Serial.println("Resetting source 5 seconds");
  mserial_1.println("r,1");
  delay(5000);
  mserial_1.begin(9600);//reinitialize the UART connection
  delay(2000);
  mserial_1.println("@,1");//set mode discoverable
}
void reset_sink(){
  Serial.println("TRACE: reset_sink called...");
  Serial.println("Resetting sink 5 seconds");
  mserial_2.println("r,1");
  delay(5000);
  mserial_2.begin(9600);//reinitialize the UART connection
  delay(2000);
}
void establish_source_connection(){
    Serial.println("TRACE: establish_source_connection called...");
  /*function polls connection status - exit only upon connection to the source*/
    char temp;
    mFlag = 0;
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

    if (index == 0 ) //checks connection to Source module
          digitalWrite(RED_LED, HIGH);
    else
          digitalWrite(RED_LED, LOW);

    digitalWrite(YEL_LED, HIGH);          //turn on LED
    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Source Status");
      Serial.println(board[0]+board[1]+board[2]+board[3]);
      if (board[3] == '3') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      }
    }
    index = 0;                            //reset index
  }
}
int verify_source_connection(){
      Serial.println("TRACE: verify_source_connection called...");
  /*function queries source module
      returns:
      -1 - Error on communicating with module
      0  - Module responding, but no connection is established
      1  - Module responding and connected to source*/
    char temp;
    mFlag = 0;
    index = 0;                            //reset index

    mserial_1.println("q");                 //send query command to device
    delay(500);
    while(mserial_1.available() > 0)      //while device still has data to input
    {
      temp = mserial_1.read();
      board[index++] =  temp; //read input into buffer
      Serial.write(temp);
    }

    if (index == 0 ) {//checks connection to Source module
          digitalWrite(RED_LED, HIGH);
          return -1;
    }
    else
          digitalWrite(RED_LED, LOW);


    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Source Status");
      Serial.println(board[0]+board[1]+board[2]+board[3]);
      if (board[3] == '3') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
       digitalWrite(YEL_LED, HIGH);          //turn on LED
       return 1;
      } else {
       digitalWrite(YEL_LED, LOW);          //turn on LED
       digitalWrite(RED_LED, HIGH);
       return 0;
      }
    }
}
void setup_sink_module_2(){
        Serial.println("TRACE: setup_sink_module_2 called...");
   digitalWrite(M2_EN, HIGH);              //turn on second module
   delay(5000);
   mserial_2.begin(9600);
   mserial_2.println("u");
   delay(500);
   mserial_2.println("r,1");
   delay(5000);
   mserial_2.begin(9600);
   delay(1000);
}
void establish_sink_1_connection(){
    char temp;
    Serial.println("TRACE: establish_sink_1_connection called...");
    mFlag = 0;
    while (mFlag == 0){
    index = 0;                            //reset index
    digitalWrite(BLU_LED, LOW);           //turn off LED
    mserial_2.println("f,9");                 //send query command to device
    delay(10000);
    while(mserial_2.available() > 0)      //while device still has data to input
    {
      temp = mserial_2.read();  //read input into buffer
      board[index++] = temp;  //read input into buffer
      Serial.print(temp);
    }

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
          delay(50);
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
      Serial.print("Sink Status");
      Serial.println(board[0]+board[1]+board[2]+board[3]);
      if (board[3] == '8' || board[3] == '9') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      } else {
        mserial_2.println(sink1);  
        delay(50);
        mserial_2.println(sink2);
        delay(10000); 
      }
    }
    index = 0;
  }
}
int verify_sink_1_connection(){
    /*function queries sink module
      returns:
      -1 - Error on communicating with module
      0  - Module responding, but no connection is established
      1  - Module responding and connected to sink*/
    char temp;
        Serial.println("TRACE: verify_sink_1_connection called...");
    mFlag = 0;
    index = 0;                            //reset index

    mserial_2.println("q");                 //send query command to device
    delay(500);
    while(mserial_1.available() > 0)      //while device still has data to input
    {
      temp = mserial_1.read();
      board[index++] =  temp; //read input into buffer
      Serial.write(temp);
    }

    if (index == 0 ) {//checks connection to Source module
          digitalWrite(RED_LED, HIGH);
          return -1;
    }
    else
          digitalWrite(RED_LED, LOW);


    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Sink Status");
      Serial.println(board[0]+board[1]+board[2]+board[3]);
      if (board[3] == '8' || board[3] == '9') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
       digitalWrite(BLU_LED, HIGH);          //turn on LED
       return 1;
      } else {
       digitalWrite(BLU_LED, LOW);          //turn on LED
       digitalWrite(RED_LED, HIGH);
       return 0;
      }
    }
}
typedef enum State{
  error,
  boot,
  source_connected,
  sink_connected
};
State  mState;
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
  mserial_2.println("r,1");//turn of sink module
  reset_source();
  mState = boot;
}

void loop() // run over and over
{
  
  switch(mState){
    case boot:
        index = 0;
        mFlag = 0;
        establish_source_connection();
        Serial.println("Connected to BT source");
        mState = source_connected;
        break;
    case source_connected:
        index = 0;
        mFlag = 0;
        setup_sink_module_2();
        establish_sink_1_connection();
        Serial.println("Connected to BT sink");
        mState = sink_connected;
        break;
    case sink_connected:
        switch(verify_source_connection()) {
          case 0:
                Serial.println("Connection to BT source LOST"); 
                mState = boot;
                break;
          case -1:
                Serial.println("Connection to BT source module LOST"); 
                mState = error;
                break;
          case 1:
                Serial.println("Connection to BT source OK"); 
                switch(verify_sink_1_connection()){
                    case 0:
                          Serial.println("Connection to BT sink LOST"); 
                          mState = source_connected;
                          break;
                    case -1:         
                          Serial.println("Connection to BT sink module LOST"); 
                          reset_sink();
                          mState = source_connected;
                          break;
                    case 1:
                          Serial.println("Connection to BT sink OK"); 
                          mState = sink_connected;
                          break;  
                }
        }
        break;
    case error:
        reset_source();
        mState = boot;
        break;
  }
   delay(5000);
}

