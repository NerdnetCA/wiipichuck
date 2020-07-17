#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>

#include <wiringPiI2C.h>

//-- DEFINES

// Address of the wii chuck is 0x52
#define ADDRESS 0x52

// On older Pis, this might be i2c-0 I think
#define DEVICE "/dev/i2c-1"

// Controller initialization.  Apparently, this is 0x40 0x00 for some controllers.
#define INIT_B1 0xF0
#define INIT_B2 0x55

// Some chucks obfuscate the returned data, I am told. Uncomment to enable decryption.
// #define WIICRYPTION

// --- bad evil globals

// I use this to implement clean shutdown.
static int shutdown = 0;
// Ncurses window handle
static WINDOW * win;
// Store the window size
static int mx;
static int my;


// --- prototypes

// Draw an 8-bit integer value, with hi-lo highlighting
void draw_value(int val, int row, int col);
// Draw a string.
void draw_string(char *msg, int row, int col);

void intHandler(int v);
/** intHandler(signal)
 * handle the ctrl-C interrupt signal
 */
void intHandler(int v) {
  signal(v,SIG_IGN);
  shutdown = 1;
}

/******** main
 *
 */
int main(int ac, char **av) {
  int data[6];
  char buf[256];
  int chuc, c, z;

  // Initialize curses
  win = initscr();
  cbreak();
  noecho();
  keypad(win, TRUE);
  // install signal handler.
  signal(SIGINT, intHandler);

  // setup curses colours
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_WHITE, COLOR_GREEN);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);

  // Can we talk to the chuck? Should probably check the return value
  chuc = wiringPiI2CSetup(ADDRESS);

  // Send the init code to the chuck
  wiringPiI2CWriteReg8(chuc,INIT_B1,INIT_B2);
  // Wait a good long amount of time.
  delay(100);
  
  // Get screen size
  getmaxyx(win, my, mx);

  // Draw field labels
  sprintf(buf,"Analog X:");
  draw_string(buf, 5, mx/2-9);
  sprintf(buf,"Analog Y:");
  draw_string(buf, 6, mx/2-9);
  sprintf(buf,"X-axis:");
  draw_string(buf, 7, mx/2-9);
  sprintf(buf,"Y-axis:");
  draw_string(buf, 8, mx/2-9);
  sprintf(buf,"Z-axis:");
  draw_string(buf, 9, mx/2-9);
  sprintf(buf,"Buttons:");
  draw_string(buf, 10, mx/2-9);

  //-----MAIN LOOP-------
  while(!shutdown) {
    // Write a 0x00 to the controller to start the read.
    wiringPiI2CWrite(chuc, 0x00);
    // We need to wait for the controller before reading the data
    // but you can't wait too long. 5 seems to be a good number.
    delay(5);
    for(int i=0; i < 6; i++) {
      data[i] = wiringPiI2CRead(chuc);
#ifdef WIICRYPTION
      data[i] ^= 0x17;
      data[i] += 0x17;
#endif
    }

    // extract the z and c buttons status from the last byte
    int z = data[5] & 0x01;
    int c = (data[5] & 0x02) >> 1;
    // extract the LSBs of the accelerometer values from the last byte
    data[2] = data[2]<<2 | ((data[5] >> 2) & 0x03);
    data[3] = data[3]<<2 | ((data[5] >> 4) & 0x03);
    data[4] = data[4]<<2 | ((data[5] >> 6) & 0x03);

    // Set white on black text.
    wattron(win, COLOR_PAIR(1));

    // Render each value, discarding the 2 LSB of the accelerometer values.
    for(int j=0,r=5; j<5; j++,r++) {
      if(j<2) draw_value(data[j], r, mx/2);
      else draw_value(data[j]>>2, r, mx/2);
    }

    // Render button state
    wattron(win, COLOR_PAIR(4));
    sprintf(buf, " %s %s ", !c?"C":" ", !z?"Z":" ");
    draw_string(buf, 10, mx/2);

    // refresh window, and reset colors
    wrefresh(win);
    wattron(win, COLOR_PAIR(1));

    // wait 50, for a nice responsive controller
    delay(50);
  }
  endwin();
  return 0;
}

void draw_value(int val, int row, int col) {
    char buf[256];

    if(val < 100) wattron(win, COLOR_PAIR(3));
    else if(val > 150) wattron(win, COLOR_PAIR(2));
    else wattron(win, COLOR_PAIR(1));
    wmove(win, row, col);
    sprintf(buf, " %3d ", val);
    waddstr(win, buf);
}

void draw_string(char *msg, int row, int col) {
  wmove(win, row, col);
  waddstr(win, msg);
}
