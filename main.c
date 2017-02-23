#include <stdio.h>
#include <string.h>
#include <Carbon/Carbon.h>
#include "osxautomation.h"

#define IS_CMD( x, y ) strncmp( x, y, strlen( y ) ) == 0
#define CMD_STRING_MAXLEN 256
#define MOUSE_SPEED 4000 // bigger = slower
#define MOUSE_RESOLUTION 2.5 //how much to move the cursor each interval
#define TYPOMATIC_RATE 100000

#define NO_MOUSE_BUTTON 0
#define LEFT_MOUSE 1
#define RIGHT_MOUSE 2

#define MOUSE_DOWN 0
#define MOUSE_UP 1
#define MOUSE_DRAGGED 2
#define MOUSE_MOVED 2

#define SINGLE_CLICK 1
#define DOUBLE_CLICK 2
#define TRIPLE_CLICK 3

bool bDragging = false;

int main (int argc, const char * argv[]) {
  int cnt;
  for (cnt = 1; cnt < argc; cnt++) {
    processCommand(argv[cnt]);
  }
  return 0;
}

void processCommand(const char *cmd) {
  int tmpx, tmpy, btn;
  float tmpInterval;
  UInt32 tmpkc;
  char str[CMD_STRING_MAXLEN];

  bzero(str, CMD_STRING_MAXLEN);
  if (IS_CMD(cmd, "mouselocation")) {

    CGPoint cgLoc = mouseLoc();
    printf("%.f %.f\n", cgLoc.x, cgLoc.y);

  } else if (IS_CMD(cmd, "mousewarp ")) {

    print_msg("Warping mouse to location.");
    sscanf(cmd, "mousewarp %d %d", &tmpx, &tmpy);
    mouseMove(tmpx, tmpy);

  } else if (IS_CMD(cmd, "mousemove ")) {

    print_msg("Moving mouse.");
    sscanf(cmd, "mousemove %d %d", &tmpx, &tmpy);
    mouseMoveTo(tmpx, tmpy, 1);

  } else if (IS_CMD(cmd, "mousedown")) {

    print_msg("Pressing mouse button.");
    sscanf(cmd, "mousedown %d", &btn);
    mousePress(btn, SINGLE_CLICK);

  } else if (IS_CMD(cmd, "mouseup")) {

    print_msg("Releasing mouse button.");
    sscanf(cmd, "mouseup %d", &btn);
    mouseRelease(btn, SINGLE_CLICK);

  } else if (IS_CMD(cmd, "mouseclick")) {

    print_msg("Clicking mouse.");
    sscanf(cmd, "mouseclick %d", &btn);
    mouseClick(btn, SINGLE_CLICK);

  } else if (IS_CMD(cmd, "mousedoubleclick")) {

    print_msg("Double-clicking mouse.");
    sscanf(cmd, "mousedoubleclick %d", &btn);
    mouseClick(btn, DOUBLE_CLICK);

  } else if (IS_CMD(cmd, "mousetripleclick")) {

    print_msg("Triple-clicking mouse.");
    sscanf(cmd, "mousetripleclick %d", &btn);
    mouseClick(btn, TRIPLE_CLICK);

  } else if (IS_CMD(cmd, "mousedrag ")) {

    print_msg("Dragging mouse.");
    sscanf(cmd, "mousedrag %d %d", &tmpx, &tmpy);
    mouseDrag(LEFT_MOUSE, tmpx, tmpy);

  } else if (IS_CMD(cmd, "press ")) {

    print_msg("Pressing key.");
    sscanf(cmd, "press %x", &tmpkc);
    keyPress((CGKeyCode)tmpkc, NULL);

  } else if (IS_CMD(cmd, "release ")) {

    print_msg("Releasing key.");
    sscanf(cmd, "release %x", &tmpkc);
    keyRelease((CGKeyCode)tmpkc, NULL);

  } else if (IS_CMD(cmd, "hit ")) {

    print_msg("Hitting key.");
    sscanf(cmd, "hit %x", &tmpkc);
    keyHit((CGKeyCode)tmpkc, NULL);

  } else if (IS_CMD(cmd, "type ")) {

    print_msg("Typing.");
    strncpy(str, &cmd[5], CMD_STRING_MAXLEN);
    typeString(str);

  } else if (IS_CMD(cmd, "wait")) {

    print_msg("Waiting.");
    sscanf(cmd, "wait %f", &tmpInterval);
    usleep(1000000 * tmpInterval);

  } else {

    print_msg("I don't know what you want to do.");

  }
}

void print_msg(const char *msg) {
  printf("%s\n", msg);
}

/* MOUSE INPUT */

CGPoint mouseLoc() {
  Point currLoc;
  GetGlobalMouse(&currLoc);
  CGPoint cgLoc = {.x = currLoc.h, .y = currLoc.v};
  return cgLoc;
}

// btn: 0 = none, 1 = left, 2 = right, etc
CGEventType mouseEventType(int btn, int btnState) {
  switch(btn) {
    case NO_MOUSE_BUTTON:
      return kCGEventMouseMoved;
    case LEFT_MOUSE:
      switch(btnState) {
        case MOUSE_UP:
          return kCGEventLeftMouseUp;
        case MOUSE_DRAGGED:
          return kCGEventLeftMouseDragged;
        default:
          return kCGEventLeftMouseDown;
      }
    case RIGHT_MOUSE:
      switch(btnState) {
        case MOUSE_UP:
          return kCGEventRightMouseUp;
        case MOUSE_DRAGGED:
          return kCGEventRightMouseDragged;
        default:
          return kCGEventRightMouseDown;
      }
    default:
      switch(btnState) {
        case MOUSE_UP:
          return kCGEventOtherMouseUp;
        case MOUSE_DRAGGED:
          return kCGEventOtherMouseDragged;
        default:
          return kCGEventOtherMouseDown;
      }
  }
}

void mouseEvent(int btn, int btnState, int clickType) {
  CGPoint currLoc;
  currLoc = mouseLoc();
  CGEventType mouseType = mouseEventType(btn, btnState);

  CGMouseButton mb = (btn == LEFT_MOUSE) ?
    kCGMouseButtonLeft :
    (btn == RIGHT_MOUSE) ?
      kCGMouseButtonRight :
      kCGMouseButtonCenter;

  CGEventRef theEvent = CGEventCreateMouseEvent(NULL, mouseType, currLoc, mb);

  if (clickType) {
    CGEventSetIntegerValueField(theEvent, kCGMouseEventClickState, clickType);
  }

  CGEventPost(kCGHIDEventTap, theEvent);
  CFRelease(theEvent);
}

/* MOUSE MOVEMENT */

void mouseMove(int posX, int posY) {
  CGPoint dest = { .x = posX, .y = posY };
  CGWarpMouseCursorPosition(dest);
  if (bDragging) {
    mouseEvent(LEFT_MOUSE, MOUSE_DRAGGED, 0);
  } else {
    mouseEvent(NO_MOUSE_BUTTON, MOUSE_MOVED, 0);
  }
}

void mouseMoveTo(int posX, int posY, float speed) {
  CGPoint currLoc = mouseLoc();
  CGPoint destLoc = { .x = posX, .y = posY };
  float x = currLoc.x;
  float y = currLoc.y;
  float xrat, yrat;

  int diffX = abs(currLoc.x - destLoc.x);
  int diffY = abs(currLoc.y - destLoc.y);
  int dirX = currLoc.x > destLoc.x ? -1 : 1;
  int dirY = currLoc.y > destLoc.y ? -1 : 1;

  if (diffX == 0 && diffY == 0) {
    return;
  }

  if (diffX > diffY) {
    xrat = MOUSE_RESOLUTION * dirX;
    if (diffY == 0) {
      yrat = 0;
    } else {
      yrat = (((float)diffY / diffX) * dirY) * MOUSE_RESOLUTION;
    }
  } else {
    yrat = MOUSE_RESOLUTION * dirY;
    if (diffX == 0) {
      xrat = 0;
    } else {
      xrat = (((float)diffX / diffY) * dirX) * MOUSE_RESOLUTION;
    }
  }

  int xArrived = 0, yArrived = 0, diff;
  float accelerant;
  while (!xArrived && !yArrived) {
    diffX = abs(destLoc.x - x);
    diffY = abs(destLoc.y - y);
    diff = diffX > diffY ? diffX : diffY;
    accelerant = diff > 70 ? diff / 40 : (diff > 40 ? diff / 20 : 1);

    if (!xArrived && diffX < abs(xrat)) {
      xArrived = 1;
      x = destLoc.x;
    } else {
      x += xrat * accelerant;
    }

    if (!yArrived && diffY < abs(yrat)) {
      yArrived = 1;
      y = destLoc.y;
    } else {
      y += yrat * accelerant;
    }

    mouseMove((int)x, (int)y);
    usleep((int)(speed * (MOUSE_SPEED * MOUSE_RESOLUTION)));
  }
}

/* MOUSE CLICKING */

void mousePress(int btn, int clickType) {
  mouseEvent(btn, MOUSE_DOWN, clickType);
}

void mouseRelease(int btn, int clickType) {
  mouseEvent(btn, MOUSE_UP, clickType);
}

void mouseClick(int btn, int clickType) {
  mousePress(btn, clickType);
  usleep(400000);
  mouseRelease(btn, clickType);
}

void mouseDrag(int btn, int posX, int posY) {
  bDragging = true;
  mouseEvent(btn, MOUSE_DOWN, SINGLE_CLICK);
  usleep(50000);
  mouseMoveTo(posX, posY, 5);
  usleep(50000);
  mouseEvent(btn, MOUSE_UP, SINGLE_CLICK);
}

/* KEYBOARD INPUT */

void typeString(char *str) {
  Ascii2KeyCodeTable ttable;
  InitAscii2KeyCodeTable(&ttable);

  int i;
  char chr;
  for (i = 0; i < strlen(str); i++) {
    chr = str[i];
    UInt32 kc = AsciiToKeyCode(&ttable, (int)chr);

    // if the kc doesn't match the char when we convert it back, assume Shift
    CGEventFlags flags = NULL;
    if (KeyCodeToAscii(kc) != chr) {
      flags = kCGEventFlagMaskShift;
    }
    keyHit((CGKeyCode)kc, flags);
  }
}

void keyHit(CGKeyCode kc, CGEventFlags flags) {
  keyPress(kc, flags);
  usleep(TYPOMATIC_RATE);
  keyRelease(kc, flags);
}

void keyPress(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, true);
}

void keyRelease(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, false);
}

void toKey(CGKeyCode kc, CGEventFlags flags, bool upOrDown) {
  CGEventRef theEvent = CGEventCreateKeyboardEvent(NULL, kc, upOrDown);
  if (flags) {
    CGEventSetFlags(theEvent, flags);
  }
  CGEventPost(kCGAnnotatedSessionEventTap, theEvent);
  CFRelease(theEvent);
}

/*================
 * iGetKey
 *--------------*/

/**
 * initAscii2KeyCodeTable initializes the ascii to key code
 * look up table using the currently active KCHR resource. This
 * routine calls GetResource so it cannot be called at interrupt time.
 */
static OSStatus InitAscii2KeyCodeTable(Ascii2KeyCodeTable* ttable) {
  unsigned char* theCurrentKCHR, *ithKeyTable;
  short count, i, j, resID;
  Handle theKCHRRsrc;
  ResType rType;

  // set up our table to all minus ones
  for (i = 0; i < 256; i++)
    ttable->transtable[i] = -1;

  // find the current kchr resource ID
  ttable->kchrID = (short)GetScriptVariable(smCurrentScript, smScriptKeys);

  // get the current KCHR resource
  theKCHRRsrc = GetResource('KCHR', ttable->kchrID);
  if (theKCHRRsrc == NULL)
    return resNotFound;
  GetResInfo(theKCHRRsrc, &resID, &rType, ttable->KCHRname);

  // dereference the resource
  theCurrentKCHR = (unsigned char *)(*theKCHRRsrc);

  // get the count from the resource
  count = *(short*)(theCurrentKCHR + kTableCountOffset);

  // build inverse table by merging all key tables
  for (i = 0; i < count; i++) {
    ithKeyTable = theCurrentKCHR + kFirstTableOffset + (i * kTableSize);
    for (j = 0; j < kTableSize; j++) {
      if (ttable->transtable[ ithKeyTable[j] ] == -1)
        ttable->transtable[ ithKeyTable[j] ] = j;
    }
  }

  return noErr;
}

/**
 * asciiToKeyCode looks up the ascii character in the key
 * code look up table and returns the virtual key code for that
 * letter. If there is no virtual key code for that letter, then
 * the value -1 will be returned.
 */
static short AsciiToKeyCode(Ascii2KeyCodeTable* ttable, short asciiCode) {
  if (asciiCode >= 0 && asciiCode <= 255) {
    return ttable->transtable[asciiCode];
  } else {
    return false;
  }
}

static char KeyCodeToAscii(short virtualKeyCode) {
  unsigned long state;
  long keyTrans;
  char charCode;
  Ptr kchr;
  state = 0;
  kchr = (Ptr)GetScriptVariable(smCurrentScript, smKCHRCache);
  keyTrans = KeyTranslate(kchr, virtualKeyCode, &state);
  charCode = keyTrans;
  if (!charCode) {
    charCode = (keyTrans >> 16);
  }
  return charCode;
}
