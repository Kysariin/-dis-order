#include <Arduino.h>
#include <TFT_eSPI.h>


TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);        // sprite 1: waves
TFT_eSprite textSprite = TFT_eSprite(&tft); // sprite 2: lyrics

// --- WAVE SETTINGS ---
const int NUM_LINES = 40; 
int line_heights[NUM_LINES];
int line_shifts[NUM_LINES];
float line_speeds[NUM_LINES];
float phase = 0;

uint16_t greyColor;

// --- KARAOKE SETTINGS ---
struct LyricLine {
  unsigned long timestamp; // time in milliseconds
  String text;             // text display
};

// TIMING FOR "DISORDER"
LyricLine songLyrics[] = {
  // --- INTRO ---
  {0,       "JOY DIVISION"},
  {14000,   "UNKNOWN PLEASURES"},


  // --- VERSE 1 ---
  {29500,   "I've been waiting for a guide"},
  {31000,   "to come and take me by the hand"},
  {33500,   ""},
  {35300,   "Could these sensations make me feel"}, // moved later (was 34800)
  {37000,   "the pleasures of a normal man?"},      // moved later to speed up scroll (was 36500)
  {39000,    ""},
  {41000,   "These sensations barely interest me"}, // moved later (was 41000)
  {43000,   "for another day"},  
  {44500,   ""},                  // adjusted to fit flow
  {46650,   "I've got the spirit,"},                // moved later (was 46500)
  {47800,   "lose the feeling," },                  // moved later to fix previous speed
  {49000,   "take the shock away"},                 // moved later (was 48500)
  {51000,   ""},


  // --- BRIDGE 1 ---
  {53000,   "JOY DIVISION"},
  {68000,   "UNKNOWN PLEASURES"},


  // --- VERSE 2 ---
  {86000,   "It's getting faster, moving faster now,"}, // moved later (was 85000)
  {88000,   "it's getting out of hand"},
  {90000,   ""},
  {91800,   "On the tenth floor, down the back stairs,"}, // moved later (was 91000)
  {94000,   "it's a no man's land"},
  {96000,   ""},
  {97500,   "Lights are flashing, cars are crashing,"},
  {99700,   "getting frequent now"},
  {102000,  ""},
 
  // *** FIXED TIMING SECTION ***
  // timing on this is weird
  {102200,  "I've got the spirit,"},  // moved later (was 102000)
  {103100,  "lose the feeling,"},     // moved earlier (was 104500)
  {104500,  "let it out somehow"},    // moved earlier (was 106500)
  // that is still too late and slow
  {106700,  ""},


  // --- BRIDGE 2 ---
  {108500,  "JOY DIVISION"},
  {126000,  "UNKNOWN PLEASURES"},


  // --- VERSE 3 ---
  {142000,  "What means to you, what means to me,"},
  {144000,  "and we will meet again"},            // FIXED: was 140000 (backward time)
  {146500,  ""},
  {148000,  "I'm watching you, I'm watching her,"}, // moved later (was 148000)
  {150000,  "I'll take no pity from your friends"}, // moved later to slow down prev
  {153000,  "Who is right? Who can tell."},
  {155000,  "and who gives a damn right now?"},    // moved earlier (was 156000)


  // --- OUTRO ---
  {158500,  "Until the spirit new sensation"},
  {160500,  "takes hold, then you know"},          // moved later to slow down prev (was 160000)
  {164000,  "Until the spirit new sensation"},     // moved later (was 164000)
  {166000,  "takes hold, then you know"},          // moved earlier (was 167000)
  {170500,  "Until the spirit new sensation"},     // moved later (was 171000)
  {172000,  "takes hold, then you know"},          // moved later (was 172000)
  {176500,  ""},
  {178000,  "I've got the spirit"},                // moved later (was 177000)
  {181000,  "but lose the feeling"},
  {184000,  "I've got the spirit"},                // moved later (was 183000)
  {187000,  "but lose the feeling"},
  {189000,  "feeling, feeling"},
  {191000,  "feeling, feeling"},
  {192500,  "feeling, feeling"},
  {195000,  "feeling"},
  {198500,  "JOY DIVISION"},
  {206000,  "UNKNOWN PLEASURES"}
};

// calculate total lines automatically
const int TOTAL_LINES = sizeof(songLyrics) / sizeof(songLyrics[0]);
int currentLineIndex = 0;
unsigned long songStartTime = 0;

void setup() {
  tft.init();
  
  // --- FLIPPED ROTATION ---
  tft.setRotation(2); // 2 = portrait inverted (180 degrees)
  
  tft.fillScreen(TFT_BLACK);
  
  // create a random seed based on static noise from an empty pin
  // THIS IS WHAT MAKES IT GENERATIVE/NOT A GIF!!!
  randomSeed(analogRead(0)); 
  
  greyColor = tft.color565(128, 128, 128);

  // 1. create sprites
  img.createSprite(135, 165); 
  textSprite.createSprite(135, 75);
  textSprite.setTextDatum(TL_DATUM); 

  // 2. initialize wave randomness
  // ALSO HELPS MAKE IT NOT A GIF!!!
  for (int i = 0; i < NUM_LINES; i++) {
    line_heights[i] = random(10, 35);
    line_shifts[i]  = random(-15, 15);
    line_speeds[i]  = random(5, 20) / 10.00;
  }
  
  // 3. start timer
  songStartTime = millis();
}

void loop() {
  // ==========================================
  // PART 1: DRAW WAVES
  // ==========================================
  img.fillSprite(TFT_BLACK);
  phase += 0.5;

  int line_index = 0;
  for (int y = 20; y < 160; y += 4) {
    if (line_index >= NUM_LINES) break;
    
    int h = line_heights[line_index];
    int s = line_shifts[line_index];
    float speed = line_speeds[line_index];

    for (int x = 0; x < 135; x++) {
      float x_centered = (x - 67.5 + s);
      float bell_curve = exp(-abs(x_centered) / 10.0);
      float ripple = sin(x * 0.1 + phase * speed);
      int offset = (int)(abs(ripple) * bell_curve * h);

      img.drawFastVLine(x, y - offset, offset + 1, TFT_BLACK);
      img.drawPixel(x, y - offset, TFT_WHITE);
    }
    line_index++;
  }
  img.pushSprite(0, 0);

  // ==========================================
  // PART 2: UPDATE LYRICS
  // ==========================================
  unsigned long currentSongTime = millis() - songStartTime;

  if (currentLineIndex < TOTAL_LINES - 1) {
    if (currentSongTime >= songLyrics[currentLineIndex + 1].timestamp) {
      currentLineIndex++;
    }
  } else {
    // AT THE END ("UNKNOWN PLEASURES")
    // check if 15 seconds have passed since the last line started
    // (200000 + 15000 = 215000ms total time)
    if (currentSongTime > songLyrics[TOTAL_LINES - 1].timestamp + 15000) {
       // RESTART THE SONG LOGIC
       currentLineIndex = 0;
       songStartTime = millis(); // reset the zero point to now
    }
  }

  // clear sprite
  textSprite.fillSprite(TFT_BLACK);
  textSprite.drawLine(0, 0, 135, 0, greyColor); 

  String currentText = songLyrics[currentLineIndex].text;

  // --- LOGIC: TITLE CARDS vs SCROLLING LYRICS ---
  bool isTitleCard = (currentText == "JOY DIVISION" || currentText == "UNKNOWN PLEASURES" || currentText.indexOf('[') != -1);

  if (isTitleCard) {
      textSprite.setTextDatum(MC_DATUM); 
      if ((millis() / 500) % 2 == 0) {
          textSprite.setTextColor(greyColor, TFT_BLACK);
      } else {
          textSprite.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      textSprite.drawString(currentText, 67, 37, 2); 
  } 
  else {
      textSprite.setTextColor(TFT_WHITE, TFT_BLACK);
      textSprite.setTextDatum(TL_DATUM); 
      
      unsigned long lineStart = songLyrics[currentLineIndex].timestamp;
      unsigned long lineEnd = songLyrics[currentLineIndex + 1].timestamp;
      unsigned long lineDuration = lineEnd - lineStart;
      
      float progress = (float)(currentSongTime - lineStart) / (float)lineDuration;
      if (progress > 1.0) progress = 1.0;

      int textWidth = textSprite.textWidth(currentText, 2);
      int startX = 135;
      int endX = -textWidth;
      
      int currentX = startX + (int)((endX - startX) * progress);

      textSprite.drawString(currentText, currentX, 30, 2); 
  }

  textSprite.pushSprite(0, 165);
}
