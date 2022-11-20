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
#define TOP_WAIT            3000    //millisceonds
#define BOTTOM_WAIT         8000    //milliseconds

/* OTHER MAGIC NUMBERS */
#define START_DELAY         1000
#define ACTUATOR_CALIBRATE  98000  //Distance beyond actual actuator length for calibration purposes 
#define INCH                1000    //Approx 3000 steps per inch
#define INIT_DIRECTION      -1      //-1 is DOWN, 1 is UP
#define UP_DIR              1
#define DOWN_DIR            -1

int currDirection;  //flag for current direction state
int currDirection_; //flag for next direction state next state
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
  currDirection_ = INIT_DIRECTION;
  currDirection = INIT_DIRECTION;
  stepper.setCurrentPosition(0);        //Sets the current position as the 0 position arbitrarily. Will be reset during calibration sequence.
  stepper.moveTo(INIT_DIRECTION*ACTUATOR_CALIBRATE);
}

void checkSwitch(){
  bottomTrigVal = digitalRead(BOTTOM_TRIGGER);
  topTrigVal = digitalRead(TOP_TRIGGER);
}

void loop() {
  checkSwitch();
  /* Actuator Calibration */
  if(bottomTrigVal==false && currDirection==DOWN_DIR){
    currDirection_ = UP_DIR;
    stepper.setCurrentPosition(0);                //Sets the bottom switch as the bottom or 0 position         
    stepper.moveTo(ACTUATOR_CALIBRATE);
    delay(BOTTOM_WAIT);
    stepper.run();
    //Serial.print("Going up\n");
  }
  /* Actuator Calibration */
  else if(topTrigVal==false && currDirection==UP_DIR){
    currDirection_ = DOWN_DIR;
    topmostPosition = stepper.currentPosition();  //Sets the top switch as the topmost position
    stepper.moveTo(INCH);                         //One inch above 0 position
    delay(TOP_WAIT);
    stepper.run();
  }
  // If at the end of travel go to the other end
  else if(stepper.distanceToGo()==0){
    //if at top go down
    if(currDirection==UP_DIR){
      currDirection_ = DOWN_DIR;
      stepper.moveTo(INCH);
      delay(TOP_WAIT);
      stepper.run();
    }
    //if at bottom go up
    else if(currDirection==DOWN_DIR){
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
