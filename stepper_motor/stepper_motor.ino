/* 
 * Stepper motor demo library for 28BYJ-48 Stepper motor with ULN2003 stepper motor driver
 *  - Descr:  Basic functions for stepper motor
 *  - Author: Codreanu Dan
 *  - Anex:
 *    - 28BYJ-48 Stepper motor datasheet: https://www.mouser.com/datasheet/2/758/stepd-01-data-sheet-1143075.pdf?srsltid=AfmBOoolEo83boUwfMHi0TjeZObm1F_wYZbrxmfF2AxUn51YxAvZe0lU
 *    - ULN2003 stepper motor driver datasheet: https://www.hadex.cz/spec/m513.pdf
 */

/* Variable declaration*/
#define STEPPER_STEP_REPORT true
#define STEPPER_NR_OF_PORTS 4u
#define STEPPER_TO_LEFT true
#define STEPPER_TO_RIGHT false
const uint8_t Stepper_CtrlPorts[] = {11, 10, 9, 8};

/*
 *  The stator in the Stepper Motor we have supplied has 32 magnetic poles. Therefore, to complete one full 
 * revolution requires 32 full steps. The rotor (or output shaft) of the Stepper Motor is connected to a speed 
 * reduction set of gears and the reduction ratio is 1:64. Therefore, the final output shaft (exiting the Stepper 
 * Motor’s housing) requires 32 X 64 = 2048 steps to make one full revolution.
 */
uint16_t full_rotation = 2048;

/* Time between steps 5ms-10ms-20ms (200Hz-100Hz-50Hz)*/
byte step_time = 10;

#define POT_PIN A0

/* Function prototypes */
void setup();
void loop();

void Stepper_SetCtrlPorts(const uint8_t ctrlPorts[]);
void Stepper_MoveOneStep(bool direction);
void Stepper_MoveNSteps(bool direction, uint16_t steps, byte ms);
void Stepper_MoveByCommand(uint8_t potPin);
uint16_t Stepper_TakeInputFromPot(uint8_t potPin);
bool Stepper_DetectCommand(uint16_t lastVal, uint16_t newVal, uint8_t tol);


/* Arduino core */
void setup() 
{
    Serial.begin(9600);
    pinMode(POT_PIN, INPUT);
    Stepper_SetCtrlPorts(Stepper_CtrlPorts);
}

void loop() 
{
    Stepper_MoveByCommand(POT_PIN);
    delay(25);
}


/* Stepper motor functions */
void Stepper_SetCtrlPorts(const uint8_t ctrlPorts[])
{
    for(uint8_t i = 0; i < STEPPER_NR_OF_PORTS; i++)
    {
        pinMode(ctrlPorts[i], OUTPUT);
    }
}

void Stepper_MoveOneStep(bool direction)
{
    /* Define a variable, use four low bit to indicate the state of port */ 
    static byte out = 0x01;

    /* Decide the shift direction according to the rotation direction: TRUE - Left ; FALSE - Right*/ 
    if(direction == STEPPER_TO_LEFT)
    {
        /* Shift to the left until out = 0x08, then reset it to 0x01 */
        out != 0x08 ? out = out << 1 : out = 0x01;
    }
    else if(direction == STEPPER_TO_RIGHT)
    {
        /* Shift to the right until out = 0x01, then reset it to 0x08 */
        out != 0x01 ? out = out >> 1 : out = 0x08;
    }

    /* Output singal to each port */ 
    for(uint8_t i = 0; i < STEPPER_NR_OF_PORTS; i++)
    {
        volatile uint8_t out_val = ((out & (0x01 << i)) ? HIGH : LOW);
        digitalWrite(Stepper_CtrlPorts[i], out_val);
    }

}

void Stepper_MoveNSteps(bool direction, uint16_t steps, byte ms)
{
    for(uint16_t i = 0; i < steps; i++)
    {
        Stepper_MoveOneStep(direction);
        delay(ms);
    }
}

void Stepper_MoveByCommand(uint8_t potPin)
{

    /* Value of potentiometer after oversampling*/
    uint16_t avgPotVal = 0;
    /* Tolerance to prevent accidental rotations ~ 0.5%*/
    uint8_t referencePointTol = 11;

    for(uint8_t i = 0; i < 5; i++)
    {
        uint16_t val = Stepper_TakeInputFromPot(potPin);
        avgPotVal += val;
    }

    avgPotVal = int(avgPotVal / 10);
    uint16_t potVal = (int)(avgPotVal * 2);
    /* Store the actual position of the motor shaft*/
    static uint16_t referencePoint  = potVal;

    /* Detect any movement and make decision */
    bool movementFlag = Stepper_DetectCommand(referencePoint , potVal, referencePointTol);
    if(movementFlag == true)
    {   
        
        /* 
                        <*** LEFT MOVING SCENARIOS ***>
            LEFT(0) -> *START* -> *FINSH* -> MIDPOINT(1023) -> RIGHT(2048) 
            LEFT(0) -> MIDPOINT(1023) -> *START* -> *FINSH* -> RIGHT(2048)
            LEFT(0) -> *START* -> MIDPOINT(1023) -> *FINSH* -> RIGHT(2048)
        */
        if((referencePoint > potVal - referencePointTol) && (referencePoint > potVal + referencePointTol))
        {
            /* actual steps = new_pos - refernce_point */
            uint16_t stepsToMake = abs((int)referencePoint  - (int)potVal);
            Stepper_MoveNSteps(STEPPER_TO_RIGHT, stepsToMake, step_time);
        }

        /* 
                        <*** RIGHT MOVING SCENARIOS ***>
            LEFT(0) <- *START* <- *FINSH* <- MIDPOINT(1023) <- RIGHT(2048) 
            LEFT(0) <- MIDPOINT(1023) - *START* <- *FINSH* <- RIGHT(2048)
            LEFT(0) <- *START* <- MIDPOINT(1023) <- *FINSH* <- RIGHT(2048)
        */
        if((referencePoint < potVal - referencePointTol) && (referencePoint < potVal + referencePointTol))
        {
            /* actual steps = new_pos - refernce_point */
            uint16_t stepsToMake = abs((int)potVal - (int)referencePoint );
            Stepper_MoveNSteps(STEPPER_TO_LEFT, stepsToMake, step_time);
        }
        

        /* Set new reference point */
        referencePoint  = potVal;
    }
    else 
    {

    }

}

bool Stepper_DetectCommand(uint16_t lastVal, uint16_t newVal, uint8_t tol)
{
    if(newVal < lastVal - tol || newVal > lastVal + tol)
    {
        return true;        
    }
    else 
    {
        return false;
    }
}

uint16_t Stepper_TakeInputFromPot(uint8_t potPin)
{
    volatile uint16_t avgRawVal = 0;

    for(uint8_t i = 0; i < 10; i++)
    {
        uint16_t rawVal = analogRead(potPin);
        avgRawVal += rawVal;
    }
    avgRawVal = (int)(avgRawVal / 10);

    uint16_t stepVal = (int)(avgRawVal * 2);
    return abs(stepVal);
}
