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
//int index = 0;
//int mFlag = 0;
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
  /*function polls connection status - exit only upon connection to the source*/
    char temp;
    int mFlag = 0;
    int index = 0;                            //reset index

    Serial.println("TRACE: establish_source_connection called...");
    while(mFlag == 0){
    index = 0;                            //reset index
    digitalWrite(YEL_LED, LOW);           //turn off LED
    mserial_1.println("q");                 //send query command to device
    delay(500);
    while(mserial_1.available() > 0)      //while device still has data to input
    {
      temp = mserial_1.read();
      board[index++] =  temp; //read input into buffer
    }

    if (index == 0 ) //checks connection to Source module
          digitalWrite(RED_LED, HIGH);
    else
          digitalWrite(RED_LED, LOW);

    digitalWrite(YEL_LED, HIGH);          //turn on LED
    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Source Status");
      Serial.print(board[0]);Serial.print(board[1]);Serial.print(board[2]);Serial.println(board[3]);
      if (board[3] == '3') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      }
    }
    index = 0;                            //reset index
  }
}
int verify_source_connection(){
  /*function queries source module
      returns:
      -1 - Error on communicating with module
      0  - Module responding, but no connection is established
      1  - Module responding and connected to source*/
    char temp;
    int mFlag = 0;
    int index = 0;                            //reset index
    int loop_control = 5;
    mserial_1.begin(9600);//reinitialize the UART connection
    delay(1000);
    Serial.println("TRACE: verify_source_connection called...");
    while(loop_control-- > 0){
         mserial_1.println("q");                 //send query command to device
        delay(1000);
        while(mserial_1.available() > 0)      //while device still has data to input
        {
          temp = mserial_1.read();
          board[index++] =  temp; //read input into buffer
        }
        if(index>0) loop_control =0;
    }
    
    if (index == 0 ) {//checks connection to Source module
          digitalWrite(RED_LED, HIGH);
          Serial.println("TRACE: verify_source_connection returning -1 due to index 0 on read ");
          return -1;
    }
    else
          digitalWrite(RED_LED, LOW);


    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Source Status: ");
        Serial.print(board[0]);Serial.print(board[1]);Serial.print(board[2]);Serial.println(board[3]);
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
void establish_sink_2_connection(){
    char temp;
    int mFlag = 0;
    int index = 0;                            //reset index
    int mFound1 = 0;
    int mFound2 = 0;
    Serial.println("TRACE: establish_sink_2_connection called...");
    while (mFlag == 0){
          index = 0;                            //reset index
          digitalWrite(BLU_LED, LOW);           //turn off LED
          mserial_2.println("f,9");                 //send query command to device
          delay(10000);
          while(mserial_2.available() > 0)      //while device still has data to input
          {
            temp = mserial_2.read();  //read input into buffer
            board[index++] = temp;  //read input into buffer
          }
      
          digitalWrite(BLU_LED, HIGH);          //turn on LED
          delay(500);
          
          for (int i = 0; i < index; i++) {
            if ((board[i] == 'F')  && (board[i+6]==':')) {
              Serial.println("found available connection");
              if (mFlag==0) {
                mFound1 = 1;
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
                 mFound2 = 1;
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
      Serial.print(board[0]);Serial.print(board[1]);Serial.print(board[2]);Serial.println(board[3]);
      if (board[3] == '8' || board[3] == '9') {               //4th HEX hold connection status - 3 connected
        index = 0;                        //reset index of buffer
        mFlag = 1;                        //connection made - move on
      } else {
        if (mFound1) mserial_2.println(sink1);  
        delay(50);
        if (mFound2) mserial_2.println(sink2);
        delay(10000); 
      }
    }
    index = 0;
  }
}
int verify_sink_2_connection(){
    /*function queries sink module
      returns:
      -1 - Error on communicating with module
      0  - Module responding, but no connection is established
      1  - Module responding and connected to sink*/
    char temp;   
    int mFlag = 0;
    int index = 0;                            //reset index
    int loop_control = 5;
 
    mserial_2.begin(9600);//reinitialize the UART connection
    delay(1000);   
    Serial.println("TRACE: verify_sink_1_connection called...");
    while(loop_control-- > 0){
         mserial_2.println("q");                 //send query command to device
        delay(1000);
        while(mserial_2.available() > 0)      //while device still has data to input
        {
          temp = mserial_2.read();
          board[index++] =  temp; //read input into buffer
        }
        if(index>0) loop_control =0;
    }

    if (index == 0 ) {//checks connection to Source module
          digitalWrite(RED_LED, HIGH);
          return -1;
    }
    else
          digitalWrite(RED_LED, LOW);


    delay(500);
    if(index == 6) {       //if the correct two byte response was recieved
      Serial.print("Sink Status: ");
      Serial.print(board[0]);Serial.print(board[1]);Serial.print(board[2]);Serial.println(board[3]);
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
void shutdown_sink_2_module(){
  Serial.println("TRACE: vshutdown_sink_2_module called...");
  digitalWrite(M2_EN, LOW);
  mserial_2.begin(9600);
  mserial_2.println("r,1");//turn of sink module
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
  
  
  //turn on serial interfaces to first module and forward messages to DEBUG
  Serial.begin(115200); 
 Serial.println("TRACE: setup preparing modules");
  mserial_1.begin(9600);
  
  
  //GREen led ON
  digitalWrite(GRE_LED, HIGH);
  shutdown_sink_2_module();
  reset_source();
  mState = boot;
}

void loop() // run over and over
{
  
  switch(mState){
    case boot:
        establish_source_connection();
        Serial.println("Connected to BT source");
        mState = source_connected;
        break;
    case source_connected:
        setup_sink_module_2();
        establish_sink_2_connection();
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
                shutdown_sink_2_module();
                mState = error;
                break;
          case 1:
                Serial.println("Connection to BT source OK"); 
                switch(verify_sink_2_connection()){
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

