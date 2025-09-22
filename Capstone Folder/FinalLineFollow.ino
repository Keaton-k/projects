#include <_Teensy.h>

#include <SD.h>
#include <SPI.h>


#include <vector>

#include <Wire.h>

#include "motorDrive.hpp"

#include "tree4.hpp"


//#include "readWrite.hpp"
// File inititalization
Junction junc0(0);
Junction* currJunc;
Junction* root = &(junc0);

Junction ju1(1), ju2(2), ju3(3), ju4(4);

//volatile std::vector<char> preSetPath = {'r','l','l'};
char preSetPath[] = {'r','l','l','\0'};
int turnProgress = 0;



int RMS = 85;
int LMS = 105;
int lastUpdate=0;
int updateTime = 50;
bool endFound = false;
// PID variables

// For your Robot to follow a line you need a setpoint in this case it is distance
// Now the process value we will change to adjust our set point is ultimatly the speed of my motors

// the set point is an arbitrary value you can choose but you must rember that 
int set_point = 2000; // This can change later
//int position = 0;


int P=0; // proportional is considered error in this situation
int I=0; // Integral
int D=0; // Derivative
int last_proportional=0;

int sensorsD[5] = {5,6,7,8,9};
int digData[5];
int ArraySensors[6]={0,0,0,0,0,0};
bool typeOfJunction[3]={false,false,false};
int tempIR[5];
int numReads = 0;
int falseReads = 0;


const int buttonPin = 33;
bool buttonState = false;
int limitTurnAround = 0;

File debugFile;


void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);


  for(int sensor=0; sensor<5;sensor++){
    pinMode(sensorsD[sensor],INPUT);
    //pinMode(sensorsA[sensor],INPUT);
  }
  // put your setup code here, to run once:
  if (!SD.begin(BUILTIN_SDCARD)) { // Use BUILTIN_SDCARD for onboard slot
        Serial.println("SD card initialization failed!");
        return;
    }
  debugFile = SD.open("debug.txt",FILE_WRITE);
  if(debugFile){
    debugFile.println("------------------------");
    debugFile.close();
  }
  Serial.println("SD card initialized.");
  

  attachMotors();
  //Junction j0(0);
  //Junction* root = &(j0);
  //intialSD();
  dfs(root);


  delay(2000);
  //runBot();
}

// This takes the weighted avg of sensors to determine postion
// (0*Sen[0]+1000*Sen[1]+2000*Sen[2]+ ...)
// ---------------------------------------
//    (  Sen[0]  + Sen[1] + Sen[2]  )


int calc_position(){
  int numarator=0;
  int denomanator=1;
  int numOfSensorActivated=0;
  int position;
  for(int numSensors=0; numSensors<5; numSensors++){
    if(digData[numSensors]==0){
      numarator += numSensors*1000;
      numOfSensorActivated += 1;
    }
  }
  if(numOfSensorActivated>0){
    position = numarator/numOfSensorActivated;
  }
  else{
    position=numarator/denomanator;
    //turnAround();
  }
  return int(position);
}


int calcNumTurns(){
  int numOfTurns=0;
  for(int i = 0; i<3; i++){
    if(typeOfJunction[i]==true){
      numOfTurns+=1;
    }
  }
  return numOfTurns;
}

std::string idTypeJunc(){
  if(typeOfJunction[0]==true && typeOfJunction[1]==false && typeOfJunction[2]==false){
    return "ll";
  }
  else if(typeOfJunction[0]==false && typeOfJunction[1]==false && typeOfJunction[2]==true){
    return "rl";
  }
  else if(typeOfJunction[0]==true && typeOfJunction[1]==true && typeOfJunction[2]==false){
    return "lt";
  }
  else if(typeOfJunction[0]==false && typeOfJunction[1]==true && typeOfJunction[2]==true){
    return "rt";
  }
  else if(typeOfJunction[0]==true && typeOfJunction[1]==false && typeOfJunction[2]==true){
    return "t";
  }
  else if(typeOfJunction[0]==true && typeOfJunction[1]==true && typeOfJunction[2]==true){
    return "c";
  }

}

void makeNewJuncAndTurn(){
  if(totalJunctions == 0){
    junc0.visted = true;
    junc0.maxTurns = calcNumTurns();
    junc0.junctionType = idTypeJunc();
    currJunc = &junc0;
    totalJunctions +=1;
    debugFile = SD.open("debug.txt",FILE_WRITE);
    if(debugFile){
      std::string temp = junc0.junctionType;
      debugFile.println(temp.c_str());
      debugFile.close();
    }else{
      debugFile.println("error");
      debugFile.close();
    }
  }
  else if(currJunc->visted == false){
    currJunc->maxTurns = calcNumTurns();
    currJunc->junctionType = idTypeJunc();
    currJunc->visted = true;
    debugFile = SD.open("debug.txt",FILE_WRITE);
    if(debugFile){
      std::string temp = currJunc->junctionType;
      debugFile.println(temp.c_str());
      debugFile.close();
    }else{
      debugFile.println("error");
      debugFile.close();
    }
  }
  //else if(currJunc.id )
  if(currJunc->maxTurns == 0){
        currJunc = currJunc->parent;
        turnAround();
      }
  else if(currJunc->turnsMade <= currJunc->maxTurns){
      Junction* tempJunc = new Junction(totalJunctions);
      tempJunc->turnsMade = 0;
      tempJunc->parent = currJunc;
      tempJunc->visted = false;
      //tempJunc.maxTurns = calcNumTurns();
      //currJunc->turnsMade += 1;
      totalJunctions +=1;
      if(currJunc->maxTurns == 1){
          if(currJunc->junctionType == "rl"){
              if(currJunc->turnsMade==0){
                currJunc->turnsMade += 1;
                currJunc->right = tempJunc;
                currJunc = currJunc->right;
                _90DegreeTurn('r');
              }
              else{
                currJunc = currJunc->parent;
                _90DegreeTurn('l');
              }
          }
          else if(currJunc->junctionType == "ll"){
              if(currJunc->turnsMade==0){
                currJunc->turnsMade += 1;
                currJunc->left = tempJunc;
                currJunc = currJunc->left;
                _90DegreeTurn('l');
              }
              else{
                currJunc->turnsMade += 1;
                currJunc = currJunc->parent;
                _90DegreeTurn('r');
              }
          }
        }
      // types of junctions this identifies --| or |-- or _|_
      else if(currJunc->maxTurns == 2){
          // this one is |--
          if(currJunc->junctionType == "rt"){
              if(currJunc->turnsMade == 0){
                  currJunc->turnsMade += 1;
                  currJunc->right = tempJunc;
                  currJunc = currJunc->right;
                  _90DegreeTurn('r');
              }
              else if(currJunc->turnsMade == 1){
                  currJunc->turnsMade += 1;
                  currJunc->forward = tempJunc;
                  currJunc = currJunc->forward;
                  _90DegreeTurn('r');
              }
              else if(currJunc->turnsMade == 2){
                currJunc->turnsMade += 1;
                currJunc = currJunc->parent;
                moveForward();
              }
          }
          // this is --|
          else if(currJunc->junctionType == "lt"){
              if(currJunc->turnsMade == 0){
                  currJunc->turnsMade += 1;
                  currJunc->forward = tempJunc;
                  currJunc = currJunc->forward;
                  moveForward();
              }
              else if(currJunc->turnsMade == 1){
                  currJunc->turnsMade += 1;
                  currJunc->left = tempJunc;
                  currJunc = currJunc->left;
                  _90DegreeTurn('r');
              }
              else if(currJunc->turnsMade == 2){
                currJunc->turnsMade += 1;
                currJunc = currJunc->parent;
                _90DegreeTurn('r');
              }
            }
          // this is __|__
          else if(currJunc->junctionType == "t"){
              if(currJunc->turnsMade == 0){
                  currJunc->turnsMade += 1;
                  currJunc->right = tempJunc;
                  currJunc = currJunc->right;
                  _90DegreeTurn('r');
              }
              else if(currJunc->turnsMade == 1){
                  currJunc->turnsMade += 1;
                  currJunc->left = tempJunc;
                  currJunc = currJunc->left;
                  moveForward();
              }
              else if(currJunc->turnsMade == 2){
                currJunc->turnsMade += 1;
                currJunc = currJunc->parent;
                _90DegreeTurn('r');
              }
            }
      }
      // this is checking for T intersection  so like -> + type it has three turns to make
      // and below is what it needs to do for each turn
      else if(currJunc->maxTurns == 3){
          if(currJunc->turnsMade == 0){
              currJunc->turnsMade += 1;
              currJunc->right = tempJunc;
              currJunc = currJunc->right;
              _90DegreeTurn('r');
          }
          else if(currJunc->turnsMade == 1){
              currJunc->turnsMade += 1;
              currJunc->forward = tempJunc;
              currJunc = currJunc->forward;
              _90DegreeTurn('r');
          }
          else if(currJunc->turnsMade == 2){
              currJunc->turnsMade += 1;
              currJunc->left = tempJunc;
              currJunc = currJunc->left;
              _90DegreeTurn('r');
          }
          else if(currJunc->turnsMade == 3){
            currJunc->turnsMade += 1;
            currJunc = currJunc->parent;
            _90DegreeTurn('r');
          }
        }
      else{
        currJunc = currJunc->parent;
        turnAround();
      }
      }
}
void scanIR(){
  //static int tempIR[5];
  for(int numReads=0; numReads<5; numReads++){
    tempIR[numReads]=digitalRead(sensorsD[numReads]);
    //anaData[numReads]=analogRead(sensorsA[numReads]);
    }
}

// This function detects all possible paths for a junction
void identifyJunction(){
  // does it have forward or not
  for(int i=0;i<3;i++){
    typeOfJunction[i]=false;
  }
  motorStop();
  //This detects left turn
  if(digData[0]==0 && digData[1]==0 && digData[2]==0){
    typeOfJunction[0] = true;
  }
  //detects right turn
  if(digData[2]==0 && digData[3]==0 && digData[4]==0){
    typeOfJunction[2] = true;
  }
  delay(1000);
  moveForward();
  delay(500);
  scanIR();
  //moveBackward();
  // has forward
  if(tempIR[1]==0||tempIR[2]==0||tempIR[3]==0){
    typeOfJunction[1] = true;
  }
  // This checks for the end for a black square
  if(tempIR[0]==0 && tempIR[4]==0){
    //moveForward();
    //moveForward();
    scanIR();
    if(tempIR[0]==0 && tempIR[1]==0 && tempIR[2]==0 && tempIR[3]==0 && tempIR[4]==0){
      motorStop();
      // write tree to text file
      //writeTreeData();
      //getInfoAboutTree(root);
      endFound = true;
      currJunc->end = true;
      dfs(root);
      listOfTurn[counter] = 'e';
      //counter += 1;
      debugFile = SD.open("debug.txt",FILE_WRITE);
      if(debugFile){
        for(int i=0; i<counter;i++){
          debugFile.print(listOfTurn[i]);
        }
        debugFile.close();
      }else{
        debugFile.println("error");
        debugFile.close();
      }

      delay(5000);
      turnAround();
      return;

      //turnAround();
      //motorStop();
      //delay(5000);
    }
  }
  moveBackward();
  delay(1000);
  //determineTurnUnvisted();
  makeNewJuncAndTurn();
  delay(1000);
 
}

void followDirections(){
  //char turnToMake = *preSetPath.begin();
  //char turnToMake = preSetPath[turnProgress];
  char turnToMake = listOfTurn[turnProgress];

  turnProgress += 1;
  //motorStop();
  //delay(1000);
  if(turnToMake == 'r'||turnToMake == 'l'){
    _90DegreeTurn(turnToMake);
  }
  else{
    moveForward();
  }
  //preSetPath.erase(listOfTurns.begin());

}

void ReadArrayOfIRSensor(){
  //int offset;
  numReads += 1;
  for(int numReads=0; numReads<5; numReads++){
      digData[numReads]=digitalRead(sensorsD[numReads]);
      //anaData[numReads]=analogRead(sensorsA[numReads]);

    }
  // develop reading tree instructions find end and then go back to start
  if(endFound == false){
    //This detects left turn
    if(digData[0]==0 && digData[1]==0 && digData[2]==0){
      identifyJunction();
      //followDirections();
      falseReads = 0;
    }
    //detects right turn
    else if(digData[2]==0 && digData[3]==0 && digData[4]==0){
      identifyJunction();
      //followDirections();
      falseReads = 0;
    }
    else if(digData[0]==1 && digData[1]==1 && digData[2]==1 && digData[3]==1 && digData[4]==1){
      falseReads +=1;
      if(falseReads == 7){
        identifyJunction();
        falseReads = 0;
      }
    }
    if(numReads >= 7){
      numReads = 0;
      falseReads = 0;
    }
  }
  else if(endFound == true){
    //This detects left turn
    if(digData[0]==0 && digData[1]==0 && digData[2]==0){
      //identifyJunction();
      followDirections();
      falseReads = 0;
    }
    //detects right turn
    else if(digData[2]==0 && digData[3]==0 && digData[4]==0){
      //identifyJunction();
      followDirections();
      falseReads = 0;
    }
    else if(digData[0]==1 && digData[1]==1 && digData[2]==1 && digData[3]==1 && digData[4]==1 && listOfTurn[turnProgress]=='e'){
      falseReads +=1;
      motorStop();
      delay(10000);
      /*if(falseReads == 7){
        //identifyJunction();
        motorStop();
        delay(10000);
        falseReads = 0;
      }
    }
    if(numReads >= 7){
      numReads = 0;
      falseReads = 0;
    }*/
  }
  }
}




void pid_calc(){
  float Kp = 0.055; //set Kp Value og 0.055
  float Ki = 0.000; //set Ki Value
  float Kd = .05; //set Kd Value og .1
  int output;
  //int baseRight = 85;
  //int baseLeft = 105;
  int base = 90;
  // This calculates the output for PID
  int position = calc_position();
  P = 2000 - position;
  I = I + P;
  D = P - last_proportional;
  last_proportional = P; // Also known as last error
  output = int(P*Kp + I * Ki + D * Kd); // slash error value
  //Serial.println(output);
  
  
  RMS = int(base - output);
  LMS = int(base - output);
  //getInfoFromPID(output,P,I,D,RMS,LMS,position);
  //getInfoFromSensors(digData);

  drive(RMS,LMS);
}



void loop() {
  runBot();

}
void  runBot(){
  //Junction j0(0);
  //Junction* root = &(j0);
  //listOfTurns[0];
  /*for(char n:listOfTurns){
    Serial.println(n);
    delay(3000);
    //cout<<n<<endl;
  }*/
  while (true){
    ReadArrayOfIRSensor();
    pid_calc();
    delay(50);
  }
}

