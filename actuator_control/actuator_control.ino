#include <AccelStepper.h>

/* PIN DEFINITIONS */
#define PUL             36
#define DIR             32
#define BOTTOM_TRIGGER  22
#define TOP_TRIGGER     23

/* SPEED PARAMETERS */
#define MAXSPEED     1500  // steps per second
#define ACCELERATION 2000  // steps per second per second

/* HOLD TIMES */
#define TOP_WAIT            8000    //millisceonds
#define BOTTOM_WAIT         3000    //milliseconds

/* OTHER MAGIC NUMBERS */
#define START_DELAY         1000
#define ACTUATOR_CALIBRATE  26000   //Max height of travel
#define START_CALIBRATE     100000  //Distance beyond actual actuator length for calibration purposes 
#define INCH                1000    //Approx 3000 steps per inch
#define INIT_DIRECTION      -1      //-1 is DOWN, 1 is UP
#define UP_DIR              1
#define DOWN_DIR            -1

int currDirection;  //flag for current direction state
int currDirection_; //flag for next direction state next state
int currPosition;
long topmostPosition = ACTUATOR_CALIBRATE;

/* SPDT sensors are active LOW */
bool bottomTrigVal = true;
bool topTrigVal = true;

AccelStepper stepper(1, PUL, DIR);  //AccelStepper constructor

void setup() {
  //Serial.begin(9600);

  /* pin declarations */
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(BOTTOM_TRIGGER, INPUT_PULLUP);
  pinMode(TOP_TRIGGER, INPUT_PULLUP);

  /* AccelStepper init */
  stepper.setMaxSpeed(MAXSPEED);
  stepper.setAcceleration(ACCELERATION);

  delay(START_DELAY);
  initialRun();

}

/* run calibration sequence at power cycle */
void initialRun(){
  // Serial.print("Starting calibration\n");
  currDirection_ = INIT_DIRECTION;
  currDirection = INIT_DIRECTION;
  stepper.setCurrentPosition(0);        //Sets the current position as the 0 position arbitrarily. Will be reset during calibration sequence.
  stepper.moveTo(INIT_DIRECTION*START_CALIBRATE);
}

void checkSwitch(){
  bottomTrigVal = digitalRead(BOTTOM_TRIGGER);
  topTrigVal = digitalRead(TOP_TRIGGER);
}

void loop() {
  checkSwitch();
  /* Actuator Calibration */
  if(bottomTrigVal==false && currDirection==DOWN_DIR){
    currPosition = stepper.currentPosition();
    if(currPosition <= 3000){
      // Serial.print("Bottom switch active, going up\n");
      // Serial.print("bottomTrigVal ");
      // Serial.print(bottomTrigVal);
      // Serial.print("\n");
      // Serial.print("current position ");
      // Serial.print(stepper.currentPosition());
      // Serial.print("\n");
      currDirection_ = UP_DIR;
      stepper.setCurrentPosition(0);                //Sets the bottom switch as the bottom or 0 position         
      stepper.moveTo(ACTUATOR_CALIBRATE-INCH);
      delay(BOTTOM_WAIT);
      stepper.run();
    }
    else if(stepper.distanceToGo()==0){
      //if at top go down
      if(currDirection==UP_DIR){
        // Serial.print("Reached top, going down\n");
        // Serial.print("current position ");
        // Serial.print(stepper.currentPosition());
        // Serial.print("\n");
        currDirection_ = DOWN_DIR;
        stepper.moveTo(2*INCH);
        delay(TOP_WAIT);
        stepper.run();
      }
      //if at bottom go up
      else if(currDirection==DOWN_DIR){
        // Serial.print("Reached bottom, going up\n");
        // Serial.print("current position ");
        // Serial.print(stepper.currentPosition());
        // Serial.print("\n");
        currDirection_ = UP_DIR;
        stepper.moveTo(topmostPosition-INCH);   //One inch below topmost position
        delay(BOTTOM_WAIT);
        stepper.run();
      }
      else{
        stepper.run();
      }
    }
  }

  /* Fail safe */
  // else if(topTrigVal==false && currDirection==UP_DIR){
  //   Serial.print("Top switch active, going down\n");
  //   Serial.print("topTrigVal ");
  //   Serial.print(topTrigVal);
  //   Serial.print("\n");
  //   Serial.print("current position ");
  //   Serial.print(stepper.currentPosition());
  //   Serial.print("\n");
  //   currDirection_ = DOWN_DIR;
  //   topmostPosition = (stepper.currentPosition() - 3*INCH);  //Sets the topmost position as the top switch-3 inches
  //   stepper.moveTo(INCH);                         //One inch above 0 position
  //   delay(TOP_WAIT);
  //   stepper.run();
  // }
  // If at the end of travel go to the other end
  else if(stepper.distanceToGo()==0){
    //if at top go down
    if(currDirection==UP_DIR){
      // Serial.print("Reached top, going down\n");
      // Serial.print("current position ");
      // Serial.print(stepper.currentPosition());
      // Serial.print("\n");
      currDirection_ = DOWN_DIR;
      stepper.moveTo(2*INCH);
      delay(TOP_WAIT);
      stepper.run();
    }
    //if at bottom go up
    else if(currDirection==DOWN_DIR){
      // Serial.print("Reached bottom, going up\n");
      // Serial.print("current position ");
      // Serial.print(stepper.currentPosition());
      // Serial.print("\n");
      currDirection_ = UP_DIR;
      stepper.moveTo(topmostPosition-INCH);   //One inch below topmost position
      delay(BOTTOM_WAIT);
      stepper.run();
    }
  }

  else{
    stepper.run();
  }

  currDirection = currDirection_;              //set next state as current state
}
