// Joystic image
// https://wiki.keyestudio.com/images/1/18/KS0153-2.png
// The following is a simple stepper motor control procedures
# define EN 8 // stepper motor enable , active low
# define X_DIR 5 // X -axis stepper motor direction control
# define X_STP 2 // x -axis stepper control


//Buttons setups
# define F 13 //Button F
# define E 12 //Button E
# define D 7 //Button D
# define C 6 //Button C
# define B 4 //Button B
# define A 3 //Button A


#include <AccelStepper.h>
AccelStepper stepper(1, X_STP, X_DIR);  // driver step direction

/* -- Constants -- */
float SYRINGE_VOLUME_ML = 10.0;
float SYRINGE_BARREL_LENGTH_MM = 80.0;

float THREADED_ROD_PITCH = 1.33;
int STEPS_PER_REVOLUTION = 100;

float MICROSTEPS_PER_STEP = 16;

float ML_POR_SEGUNDO_CONTINUO = 0.001;

float ML_DE_RETRACAO = 0.8;

int ustepsPerMM;
int ustepsPerML;

int stepsEnUsoContinuo;
int stepsEnUsoRetracao;

int speedRetract = 3200;

boolean flagImput = false;
int flagCount = 0;
boolean modoContinuo = false;


void setup()
{
  calculations();
  stepsEnUsoContinuo = stepsEnUso(ML_POR_SEGUNDO_CONTINUO);
  stepsEnUsoRetracao = stepsEnUso(ML_DE_RETRACAO);

  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(500);
  stepper.setSpeed(0);

  pinMode(13, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  pinMode (EN, OUTPUT);
  digitalWrite (EN, LOW);

  Serial.begin(9600);
  Serial.write("Started");
}

void loop()
{
  readDataSerial();
  readButtonData();
  updateFlagImput();

  if (modoContinuo) {
    stepper.runSpeed();
  }
}

void updateFlagImput() {
  if (flagImput) {
    if (flagCount < 30) {
      flagCount++;
    } else {
      flagCount = 0;
      flagImput = false;
    }
  }
}


void calculations() {
  ustepsPerMM = MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION / THREADED_ROD_PITCH;
  ustepsPerML = (MICROSTEPS_PER_STEP * STEPS_PER_REVOLUTION * SYRINGE_BARREL_LENGTH_MM) / (SYRINGE_VOLUME_ML * THREADED_ROD_PITCH );
}

int stepsEnUso(float ML_) {
  int interSteps_;
  interSteps_ = ustepsPerML * ML_;
  return interSteps_;
}

void playConstante(boolean play_) {
  modoContinuo = play_;
  stepper.setSpeed(stepsEnUsoContinuo);
}

void retracao(boolean direcao_) {//true retração, false purga
  boolean modoContinuo = false;
  int interSteps_ = stepsEnUsoRetracao;
  if (direcao_) {
    interSteps_ = interSteps_ * -1;
  }
  stepper.setSpeed(speedRetract);
  stepper.setCurrentPosition(0);
  stepper.runToNewPosition(interSteps_);
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void readDataSerial() {
  //Serial.write("serial read");
  // if there's any serial available, read it:
  while (Serial.available() > 0) {
    // get incoming byte:
    int imput_ = Serial.read();

    if (imput_ == '1') {
      comandos(1);
    }
    if (imput_ == '2') {
      comandos(2);
    }
    if (imput_ == '3') {
      comandos(3);
    }
    if (imput_ == '4') {
      comandos(4);
    }
    if (imput_ == '5') {
      comandos(5);
    }
    if (imput_ == '6') {
      comandos(6);
    }
    if (imput_ == '7') {
      comandos(7);
    }
  }
}

void readButtonData() {

  if (!flagImput) {
    flagImput = true;
    if (!digitalRead(F)) {
      comandos(6);
    }
    else if (!digitalRead(E)) {
      comandos(5);
    }
    else if (!digitalRead(D)) {
      comandos(2);
    }
    else if (!digitalRead(C)) {
      comandos(1);
    }
    else if (!digitalRead(B)) {
      comandos(4);
    }
    else if (!digitalRead(A)) {
      comandos(3);
    }
  }
  //Serial.write("button read");
}

void comandos(int comando_) {

  if (comando_ == 1) {
    Serial.write("prepara \n");
    retracao(false);
  }
  if (comando_ == 2) {
    Serial.write("velocidade constante \n");
    playConstante(true);
  }
  if (comando_ == 3) {
    Serial.write("retract \n");
    retracao(true);
  }
  if (comando_ == 4) {
    Serial.write("stop velocidade constante \n");
    playConstante(false);
  }
  if (comando_ == 5) {
    Serial.write("+ vasão \n");
    ML_POR_SEGUNDO_CONTINUO = ML_POR_SEGUNDO_CONTINUO + 0.001;
    calculations();
    stepsEnUsoContinuo = stepsEnUso(ML_POR_SEGUNDO_CONTINUO);
    stepsEnUsoRetracao = stepsEnUso(ML_DE_RETRACAO);
  }
  if (comando_ == 6) {
    Serial.write("- vasão \n");
    ML_POR_SEGUNDO_CONTINUO = ML_POR_SEGUNDO_CONTINUO - 0.0001;
    calculations();
    stepsEnUsoContinuo = stepsEnUso(ML_POR_SEGUNDO_CONTINUO);
    stepsEnUsoRetracao = stepsEnUso(ML_DE_RETRACAO);
  }
}
