# StepperMotor_PotentiometerControl

## 📄 Description

This project demonstrates how to control a **28BYJ-48 stepper motor** using a **ULN2003 driver module** and a **10kΩ potentiometer** connected to an **Arduino Uno**.  
The motor's rotation and number of steps are determined based on changes in the potentiometer value compared to a **reference point**.

The movement logic avoids jitter by introducing a **tolerance threshold** — only significant changes in the potentiometer value result in motor movement.

---

## 🔁 Movement Decision Logic
![RotDiagram](https://github.com/user-attachments/assets/dd703ff1-3037-4dbb-9dec-e73d78ea5e73)

A movement command is triggered **only if the change in value exceeds a defined threshold** compared to the last known position (reference point).

If the new potentiometer value is:

- **Higher than the reference + tolerance**, the motor rotates **LEFT (counter-clockwise)**.
- **Lower than the reference - tolerance**, the motor rotates **RIGHT (clockwise)**.

After movement, the **reference point is updated** to the new potentiometer value.

---

## 🛠️ Hardware Setup

### 🔌 Circuit Diagram
![HW_Diagram](https://github.com/user-attachments/assets/51e7564a-fede-4f16-9401-20889252d7ef)


### Components Used:

- 🌀 **28BYJ-48 Stepper Motor**
- 🔌 **ULN2003 Motor Driver Module**
- 🎚️ **10kΩ Potentiometer**
- 💻 **Arduino Uno**
- Breadboard + Jumper wires

---

## 💾 Software Logic

### 🔧 Functional Overview
![SW_Diagram](https://github.com/user-attachments/assets/e8bdfeeb-aef5-4e31-8d52-73504245d7bd)


| Function Name                 | Description                                                                 |
|------------------------------|-----------------------------------------------------------------------------|
| `Stepper_SetCtrlPorts()`     | Initializes stepper motor control pins as outputs                           |
| `Stepper_MoveOneStep()`      | Moves the motor one step in a given direction                               |
| `Stepper_MoveNSteps()`       | Moves the motor a defined number of steps with adjustable delay             |
| `Stepper_TakeInputFromPot()` | Reads and oversamples the analog input from the potentiometer               |
| `Stepper_DetectCommand()`    | Compares new and old potentiometer values to decide whether to move         |
| `Stepper_MoveByCommand()`    | Main logic to read input, detect changes, and trigger movement accordingly  |

### 📈 Serial Plotter View
![Serial_Plotter](https://github.com/user-attachments/assets/e10c7fda-72e2-4311-91e3-07307c9b309c)

### 💻 Serial Monitor View
![Screenshot 2025-06-25 215825](https://github.com/user-attachments/assets/6003e6e2-4005-47b7-ad98-3ddf0eafbe98)

---

### 🌀 **28BYJ-48 Stepper Motor**
![77db3b053d9e8146c8a895df04c593f7](https://github.com/user-attachments/assets/f51b1b94-9f12-4cba-98cc-26287edd68c9)

### 🔌 **ULN2003 Motor Driver Module**
![Arduino-DC-stepper-motor-driver-circuit-600x365](https://github.com/user-attachments/assets/313490bf-3071-4214-9ef3-0816d0b3fe84)

