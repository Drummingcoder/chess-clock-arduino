### 1. Overview

A simple chess clock to use while playing chess with your friends.
The following basic features are implemented:

- adjust initial time
- store last configuration
- play/pause/reset
- notify players when time is over

### 2. Setting up the environment

Libraries used:

- OneButton (improving button functionality)
- HT1632-for-arduino (interfacing arduino with Sure Electronics led dot matrix)
- EEvar (storing variables in EEPROM memory)

In order to install the libraries you need to download each repo, decompress and place it in the libraries directory in your sketchbook folder.

You can find a detailed guide about how to use libraries here.

### 3. Wiring the circuit

The wiring of the circuit is described in the attached schematic.

For the Dot Led Matrix

- Pin 11 to GND
- Pin 12 to VCC
- Pin 3 (CS1) to Arduino Digital pin 9
- Pin 5 (WR) to Arduino Digital pin 10
- Pin 7 (DATA) to Arduino Digital pin 11

For the buttons and the buzzer

- Button 0 to Arduino Digital pin 3
- Button 1 to Arduino Digital pin 4
- Button PP to Arduino Digital pin 5
- Buzzer to Arduino Digital pin 7

### 4. Running the code

Clone this repo and open the source code in your arduino code editor. After importing the code run Verify and then Upload.

### 5. Troubleshooting

##### Cannot include libraries in arduino editor?

Please read the official guide on how to use libraries and always include them from your editor (Sketch -> Include Library).

##### Buttons/Buzzer not working?

Make sure that the pins are setup correctly (BUTTON_0, BUTTON_1, BUTTON_PP, BUZZER)

##### Led Dot Matrix not working?

- is USER_OPTIONS in HT1632 library set?
  In the README file of HT1632 library, in the code section you may have missed this:

  > Before running the code, go to HT1632.h and check the USER OPTIONS section and follow the instructions to specify the type of board you are using.

- is the matrix connected properly?
  Please make sure that the PINs defined in the source code (CS1, WR, DATA) match the connections on your arduino or your led matrix.

### 6. Future

- Implement time increment variation.
