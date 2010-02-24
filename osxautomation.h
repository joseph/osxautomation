void processCommand(const char *cmd);

void print_msg(const char *msg);
CGPoint mouseLoc();
CGEventType getMouseButton(int btn, int btnState);
void mouseEvent(int btn, int btnState, int clickType);
void mouseMove(int posX, int posY);
void mouseMoveTo(int posX, int posY, float speed);
void mousePress(int btn, int clickType);
void mouseRelease(int btn, int clickType);
void mouseClick(int btn, int clickType);
void mouseDrag(int btn, int posX, int posY);
void typeString(char *str);
void keyHit(CGKeyCode kc, CGEventFlags flags);
void keyPress(CGKeyCode kc, CGEventFlags flags);
void keyRelease(CGKeyCode kc, CGEventFlags flags);
void toKey(CGKeyCode kc, CGEventFlags flags, bool upOrDown);

// iGetKey crap
typedef struct {
  short kchrID;
  Str255 KCHRname;
  short transtable[256];
} Ascii2KeyCodeTable;

enum {
  kTableCountOffset = 256+2,
  kFirstTableOffset = 256+4,
  kTableSize = 128
};

static OSStatus InitAscii2KeyCodeTable(Ascii2KeyCodeTable* ttable);
static short AsciiToKeyCode(Ascii2KeyCodeTable* ttable, short asciiCode);
static char KeyCodeToAscii(short virtualKeyCode);
