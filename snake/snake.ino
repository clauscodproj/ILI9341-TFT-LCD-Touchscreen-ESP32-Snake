#include "FS.h"

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false


#define HEADSIZE 5
#define BOXSIZE 30


int nScreenHeight = (320 - 2*BOXSIZE) - 10;
int nScreenWidth = 240;
int touch = 0;

int playerposX = 120;
int playerposY = 30;
int playerSize = 3;

int scoreX, scoreY;

int incrementY = 5;
int incrementX = 0;
int game = 1;
int increase = 0;

typedef struct player Player;
struct player
{
    int X;
    int Y;
};

void changeIncrement(int x, int y) //find a better way to code this
{
  if(incrementX == 0)
  {
    if(y > 320 - BOXSIZE)
    {
      if(x > 2*(nScreenWidth/3) && x < nScreenWidth) // x > 160 && x < 240, nScreenWidth = 240
      // to the right
      {
        incrementX = 5;
        incrementY = 0;
        Serial.println("RIGHT");
        return;
      }
      else if(x > 0 && x < (nScreenWidth/3)) // x > 0 && x < 80
      //to the left
      {
        incrementX = -5;
        incrementY = 0;
        Serial.println("LEFT");
        return;
      }
    }
  }
  if(incrementY == 0)
  {
    if(x > nScreenWidth/3 && x < 2*(nScreenWidth/3)) // x > 80 && x < 160
    {
      if(y > 320-BOXSIZE)
      //down
      {
        incrementX = 0;
        incrementY = 5;
        Serial.println("DOWN");
        return;
      }
      else if(y > 320-2*BOXSIZE)
      //up
      {
        incrementX = 0;
        incrementY = -5;
        Serial.println("UP");
        return;
      }
    }
  }
}
Player *pl = (Player*) malloc((playerSize+1)*sizeof(Player));
void setup() {
  Serial.begin(9600);
  Serial.println("Hello, ESP32-S3!");
  randomSeed(analogRead(0));
  tft.init();
  tft.setRotation(0);
  touch_calibrate();
  if(pl == NULL)
  {
    errorFunction(2);
  }
  else
  {
    for(int x = playerSize; x > 0; x--)
    {
        pl[x].X = playerposX;
        pl[x].Y = playerposY + x*incrementY;
    }
    pl[0].X = playerposX;
    pl[0].Y = playerposY;
  }
  tft.fillScreen(ILI9341_BLACK);
  drawControls();
  //scoreY = random(1, nScreenHeight);
  //scoreX = random(1, nScreenWidth);
  randScore();
  return;
}

void randScore()
{
  scoreY = (random(0, nScreenHeight/5))*5;
  scoreX = (random(0, nScreenWidth/5))*5;
  Serial.print("scoreX:  "); Serial.print(scoreX); Serial.print("  scoreY: "); Serial.println(scoreY);
  //scoreY = 0;
  //scoreY = 0;
  tft.fillRect(scoreX, scoreY, HEADSIZE, HEADSIZE, 0xa000);
  for(int x = 0; x < playerSize ; x++)
  {
    if(scoreY == pl[x].Y && scoreX == pl[x].X)
    {
      scoreY = (random(0, nScreenHeight/5))*5;
      scoreX = (random(0, nScreenWidth/5))*5;
      x = 0;
    }
  }
}
void drawControls(){
  // <-
  tft.fillRect(0, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x9cf3);
  tft.drawRect(0, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x52aa);

  // \/
  tft.fillRect(80, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x9cf3);
  tft.drawRect(80, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x52aa);

  // ->
  tft.fillRect(160, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x9cf3);
  tft.drawRect(160, 320-BOXSIZE, nScreenWidth/3, BOXSIZE, 0x52aa);

  // /\
  //tft.fillRect(75+1*BOXSIZE, 320-2*BOXSIZE, BOXSIZE, BOXSIZE, 0x9cf3);
  tft.fillRect(80, 320-2*BOXSIZE, nScreenWidth/3, BOXSIZE, 0x9cf3);
  tft.drawRect(80, 320-2*BOXSIZE, nScreenWidth/3, BOXSIZE, 0x52aa);

  tft.fillRect(0, nScreenHeight, nScreenWidth, 4, ILI9341_WHITE);
}

void errorFunction(int ErrorCode)
{
    tft.setRotation(0); 
    tft.fillScreen(0x8430); 
    tft.setCursor(1,10); 
    tft.setTextColor(0xb000);
    tft.setTextSize(3);
    Serial.print("ERROR: ");
    Serial.println(ErrorCode);
    tft.print("ERROR: ");
    tft.println(ErrorCode);
    while(1){}
}

//TS_Point p;
void loop() {
  int z;
  //increase = 1;
  while(game == 1){
  uint16_t p_x = 0, p_y = 0; // To store the touch coordinates
  delay(80); // this speeds up the simulation
  // touch = ctp.touched();
  //t_x = 0, t_y = 0; // To store the touch coordinates
  bool pressed = tft.getTouch(&p_x, &p_y);
  if(pressed)
  { //touch
    if(p_y < BOXSIZE)
    {
      if(p_x < BOXSIZE)
      {
        increase = 1;
        Serial.println("CHEAT +1 INCREASE");
      }
    }
    //Serial.println("WORKING");
    changeIncrement(p_x, p_y);
  }
  

  playerposX = playerposX + incrementX;
  playerposY = playerposY + incrementY;

  if(playerposY < 0)
  {
    playerposY = nScreenHeight-HEADSIZE;
  }
  else if(playerposY > 245)
  {
    playerposY = 0;
  }

  if(playerposX < 0)
  {
    playerposX = nScreenWidth-HEADSIZE;
  }
  else if(playerposX > 240-HEADSIZE)
  {
    playerposX = 0;
  }
  if(increase == 1)
  {
    playerSize++;
    pl = (Player*) realloc(pl, (playerSize+1)*sizeof(Player));
    if(pl == NULL)
    {
      errorFunction(3);
    }
    increase = 0;
  }
  if(playerposY == scoreY && playerposX == scoreX)
  {
    playerSize++;
    pl = (Player*) realloc(pl, (playerSize+1)*sizeof(Player));
    randScore();
    
  }
  pl[0].X = pl[1].X;
  pl[0].Y = pl[1].Y;
  tft.fillRect(pl[0].X, pl[0].Y, HEADSIZE, HEADSIZE, ILI9341_BLACK);
  for(int x = 1; x < playerSize ; x++)
  {
    pl[x].X = pl[x+1].X;
    pl[x].Y = pl[x+1].Y;
    if(playerposY == pl[x].Y && playerposX == pl[x].X)
    {
      game = 0;
      //tft.fillScreen(ILI9341_RED);
      return;
    }
    tft.fillRect(pl[x].X, pl[x].Y, HEADSIZE, HEADSIZE, ILI9341_GREEN);
  }

  pl[playerSize].X = playerposX;
  pl[playerSize].Y = playerposY;
    tft.fillRect(pl[playerSize].X, pl[playerSize].Y, HEADSIZE, HEADSIZE, ILI9341_GREEN);
  }
  gameover();
}

void gameover()
{
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("GAME OVER");
    playerSize = 3; playerposX = 120; playerposY = 30;
    incrementY = 5;
    incrementX = 0;
    pl = (Player*) realloc(pl, (playerSize+1)*sizeof(Player));
    if(pl == NULL)
    {
      errorFunction(4);
    }
    for(int x = playerSize; x > 0; x--)
    {
      pl[x].X = playerposX;
      pl[x].Y = playerposY + x*incrementY;
    }
    pl[0].X = playerposX;
    pl[0].Y = playerposY;
    delay(5000);
    drawControls();
    game = 1;
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
