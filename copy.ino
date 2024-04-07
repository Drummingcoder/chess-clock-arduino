#include <EEvar.h>
#include <OneButton.h>
#include <Adafruit_LiquidCrystal.h>
//change to my display
#include <HT1632.h>
#include <font_5x4.h>

# define ALLOWED_CHARS    10
//change for my connections
# define BUTTON_0         2
# define BUTTON_1         3
# define BUTTON_PP        4
# define BUZZER           5

//change to my display
# define CS1              9
# define WR               10
# define DATA             11

# define GAME_READY       0
# define GAME_STARTED     1
# define GAME_PAUSED      2
# define GAME_ENDED       3

# define DEFAULT_TIME     3
# define PLUS_TIME        0

# define WHITE            0
# define BLACK            1
# define NO_ONE          -1

# define EDIT_OFF        -1
# define EDIT_MINS        0
# define EDIT_SECS        1

# define MINUTE           60
# define SECOND           1

# define ON               1
# define OFF              0

struct Config {
  int _time = DEFAULT_TIME;
  int _plus = PLUS_TIME;
};

String printDisplayTime(int time) {
  String state;

  // parse time
  int minutes = time / 60;
  int seconds = time % 60;
  String minutesStr = String(word(minutes));
  String secondsStr = String(word(seconds));

  // format minutes and seconds
  if (minutesStr.length() == 1) {
    minutesStr = "0" + minutesStr;
  }
  if (secondsStr.length() == 1) {
    secondsStr = "0" + secondsStr;
  }

  // add spaces if needed
  int spaces = ALLOWED_CHARS - (minutesStr.length() + secondsStr.length());
  for (int i = 0; i < spaces; i ++) {
    state.concat(" ");
  }
    
  String dispTime = String(minutesStr + ":" + secondsStr);
  state.concat(dispTime);
  return state;
}

class Player {
    int id;
    int time;
  public:
    Player(int id);
    void tick(void);
    void setTime(int timeVal);
    String getDisplayTime(void);
    int getTime(void);
};

Player::Player(int id) {
   this->id = id;
}

void Player::setTime(int timeVal) {
  this->time = timeVal;
}

int Player::getTime() {
  return this->time;
}

void Player::tick(void) {
  this->time = this->time - 1;
}

String Player::getDisplayTime(void) {
  return printDisplayTime(this->time);
}

class Game {
    Player *white;
    Player *black;
    int activePlayer = WHITE;
    int winner = NO_ONE;
    int state = GAME_ENDED;
  public:
    Game(Player *p1, Player *p2);
    void tick(void);
    String getDisplayText(void);
    void setActivePlayer(int p);
    int getActivePlayer();
    void reset(int initalTime);
    void start(void);
    void pause(void);
    void resume(void);
    int getState(void);
    bool checkLoss(void);
    int getWinner(void);
};

Game::Game(Player *p1, Player *p2) {
  this->white = p1;
  this->black = p2;
}

int Game::getWinner(void) {
  return this->winner;
}

bool Game::checkLoss() {
  if (this->activePlayer == WHITE && this->white->getTime() <= 0) {
    this->winner = BLACK;
    this->state = GAME_ENDED;
    return true;
  }

  if (this->activePlayer == BLACK && this->black->getTime() <= 0) {
    this->winner = WHITE;
    this->state = GAME_ENDED;
    return true;
  }

  return false;
}

int Game::getState() {
  return this->state;
}

void Game::start(void) {
  this->state = GAME_STARTED;
}

void Game::pause(void) {
  this->state = GAME_PAUSED;
}

void Game::resume(void) {
  this->state = GAME_STARTED;
}

void Game::reset(int initalTime) {
  this->white->setTime(initalTime);
  this->black->setTime(initalTime);
  this->setActivePlayer(WHITE); 
  this->state = GAME_READY;
}

void Game::tick(void) {
  if (this->state != GAME_STARTED) {
    return;
  }

  if (this->checkLoss()) {
    return;
  }

  switch (this->activePlayer) {
    case WHITE:
      this->white->tick();
      break;
    case BLACK:
      this->black->tick();
      break;  
     default: break;
  }
}

void Game::setActivePlayer(int p) {
  this->activePlayer = p;
}

int Game::getActivePlayer() {
  return this->activePlayer;
}

String Game::getDisplayText(void) {
  if (this->activePlayer == WHITE) {
    return this->white->getDisplayTime();
  }
  return this->black->getDisplayTime();
}

Player p1(WHITE);
Player p2(BLACK);

Game game(&p1, &p2);

char dispArr[ALLOWED_CHARS];
String gameState;
int editMode = EDIT_OFF;

OneButton buttonDR0 = OneButton(BUTTON_0, true, false);
OneButton buttonDR1 = OneButton(BUTTON_1, true, false);
OneButton buttonDRPP = OneButton(BUTTON_PP, true, false);

EEvar<Config> configVar((Config()));

unsigned long start = millis();
unsigned long now = millis();
int editBlink = ON;

void setup () {
  // setup button pins
  pinMode(BUTTON_0, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_PP, INPUT);
  
  // setup piezzo pin
  pinMode(BUZZER, OUTPUT);
  
  // setup display, change to my display
  HT1632.begin(CS1, WR, DATA);
  
  // setup serial
  Serial.begin(9600);  

  // reset game
  game.reset(configVar->_time);
}

//will be changing
void blinkPixels() {
  int init = editMode == EDIT_MINS ? 10 : 21;
  for (int i = init; i < init + 10; i ++) {
    for (int j = 0; j < 8; j++) {
        HT1632.clearPixel(i, j); 
    }
  }
}

void loopEditMode() {
  buttonDRPP.attachClick([]() {
    if (editMode == EDIT_MINS) {
      editMode = EDIT_SECS;
    } else if (editMode == EDIT_SECS) {
      editMode = EDIT_OFF;
      game.reset(configVar->_time);
    }
  });

  buttonDR0.attachClick([]() {
    if (editMode == EDIT_MINS) {
      if (configVar->_time >= MINUTE) {
        configVar->_time = configVar->_time - MINUTE;
        configVar.save();           
      }
    }
    if (editMode == EDIT_SECS) {
      if (configVar->_time >= 0) {
        configVar->_time = configVar->_time - SECOND;
        configVar.save();         
      }
    }
  });
  
  buttonDR1.attachClick([]() {
    if (editMode == EDIT_MINS) {
      configVar->_time = configVar->_time + MINUTE;
      configVar.save();         
    }
    if (editMode == EDIT_SECS) {
      configVar->_time = configVar->_time + SECOND;
      configVar.save();         
    }
  });

  // handle edit blink
  now = millis();
  if (now - start >= 500) {
    editBlink = editBlink == ON ? OFF : ON;
    start = millis();
  }

  String initialTime = printDisplayTime(configVar->_time);

  //change to my display
  HT1632.clear();
  HT1632.drawText(initialTime.c_str(), 0, 2, FONT_5X4, FONT_5X4_END, FONT_5X4_HEIGHT);
  HT1632.drawImage(IMG_CLOCK, 8, 8, 1, 1);

  if (editBlink == ON) {
    blinkPixels();
  }
  HT1632.render();
}

void loopGamePlay() {
  // change active player button clicked
  buttonDR0.attachClick([]() {
    game.setActivePlayer(WHITE); 
    if (game.getState() != GAME_STARTED) {
      game.start(); 
    }
  });
  
  buttonDR1.attachClick([]() {
    game.setActivePlayer(BLACK); 
    if (game.getState() != GAME_STARTED) {
      game.start(); 
    }
  });
  
  // play/pause button clicked
  buttonDRPP.attachClick([]() {
   switch (game.getState()) {
       case GAME_READY:
          game.start();  
          break;
       case GAME_PAUSED:
          game.resume();
          break;
       case GAME_STARTED:
          game.pause();
          break;
       default: 
          break;
    }
  });

  // reset game if buttons are pressed simultaneously
  buttonDRPP.attachDoubleClick([]() {
    HT1632.clear();
    noTone(BUZZER);
    game.reset(configVar->_time);
    return;
  });

  // enter "edit mode"
  buttonDRPP.attachLongPressStart([]() {
     editMode = EDIT_MINS;
     return;
  });
  
  if (game.getState() == GAME_ENDED && game.getWinner() != NO_ONE ) {
    // show winner, change to show no image
    String winner = game.getWinner() == WHITE ? "White" : "Black";
    HT1632.drawText(winner.c_str(), 9, 2, FONT_5X4, FONT_5X4_END, FONT_5X4_HEIGHT);
    HT1632.drawImage(IMG_PAWN, 8,  8, 0, 0);
    HT1632.render();

    for (int i = 0; i < 3000; i ++) {
      tone(BUZZER, i);  
    }
    return;
  }

  gameState = game.getDisplayText();

  HT1632.clear();
  HT1632.drawText(gameState.c_str(), 0, 2, FONT_5X4, FONT_5X4_END, FONT_5X4_HEIGHT);
  HT1632.render();

  // handle game loop
  now = millis();
  if (now - start >= 1000 && game.getState() == GAME_STARTED) {
    game.tick();
    start = millis();
  }
}

void loop () {
  buttonDR0.tick();
  buttonDR1.tick();
  buttonDRPP.tick(); 

  if (editMode != EDIT_OFF) {
    loopEditMode();
  } else {
    loopGamePlay();
  }
}
