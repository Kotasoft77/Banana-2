unsigned char Key;

// Functions

#define Out(port_, value_)  { static __sfr __at (port_) tmp_; tmp_ = (value_); }
#define Peek(addr_)         *((unsigned char *) (addr_))
#define Poke(addr_, value_) *((unsigned char *) (addr_)) = (value_)

static unsigned char Last_IRQ_Cnt = 0;

void Set_EXOS_Var(unsigned char Variable, unsigned char Value) __naked
{
  (void) Variable;
  (void) Value;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      C,(HL)\n"  // C = Variable
    "inc     HL\n"
    "ld      D,(HL)\n"  // D = Value
    "ld      B,1\n"     // SET mode
    "rst     0x30\n"
    ".byte   16\n"
    "ret\n"
  );
}

void Open_Channel(unsigned char Channel, unsigned int FileNamePtr) __naked
{
  (void) Channel;
  (void) FileNamePtr;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "inc     HL\n"
    "ld      E,(HL)\n"  // DE = FileNamePtr
    "inc     HL\n"
    "ld      D,(HL)\n"
    "rst     0x30\n"
    ".byte   1\n"
    "ret\n"
  );
}

void Close_Channel(unsigned char Channel) __naked
{
  (void) Channel;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "rst     0x30\n"
    ".byte   3\n"
    "ret\n"
  );
}

unsigned char Channel_Status(unsigned char Channel) __naked
{
  (void) Channel;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "rst     0x30\n"
    ".byte   9\n"
    "ld      L,C\n"     // C = 0 if key pressed, otherwise C = 1
    "ret\n"
  );
}

void Get_Block(unsigned char Channel, unsigned int Bytes, unsigned int BufPtr) __naked
{
  (void) Channel;
  (void) Bytes;
  (void) BufPtr;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "inc     HL\n"
    "ld      C,(HL)\n"  // BC = Bytes
    "inc     HL\n"
    "ld      B,(HL)\n"
    "inc     HL\n"
    "ld      E,(HL)\n"  // DE = BufPtr
    "inc     HL\n"
    "ld      D,(HL)\n"
    "rst     0x30\n"
    ".byte   6\n"
    "ret\n"
  );
}

unsigned char Write_Block(unsigned char Channel, unsigned int Bytes, unsigned int BufPtr) __naked
{
  (void) Channel;
  (void) Bytes;
  (void) BufPtr;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "inc     HL\n"
    "ld      C,(HL)\n"  // BC = Bytes
    "inc     HL\n"
    "ld      B,(HL)\n"
    "inc     HL\n"
    "ld      E,(HL)\n"  // DE = BufPtr
    "inc     HL\n"
    "ld      D,(HL)\n"
    "rst     0x30\n"
    ".byte   8\n"
    "ld      L,A\n"     // A = status
    "ret\n"
  );
}


void GetKey(unsigned char Wait_for_keypress)
{
  if (Wait_for_keypress) {
    if (!Channel_Status(105)) Get_Block(105, 1, (unsigned int) &Key);
    Get_Block(105, 1, (unsigned int) &Key);
  }
  else {
    if (Channel_Status(105)) Key = 0;
    else Get_Block(105, 1, (unsigned int) &Key);
  }
}

void Say_Banana(unsigned int Addr)
{
  unsigned int i, a;
  unsigned char j;

  Out(0xB4, 170); // Disable interrupts
  Out(0xA7, 24);  // Switch left and right output to D/A convertert
  a = Addr;
  for (i = 0; i < 2500; i++) {
    for (j = 0; j < 6; j++) Out(0xA8, Peek(a));
    Out(0xAC, Peek(a++));
  }
  Out(0xA7, 0);  // Switch back left and right output
  Out(0xB4, 85); // Enable interrupts
}

void main()
{
  static unsigned char FileName[] = { 10, 'B', 'A', 'N', 'A', 'N', 'A', '.', 'W', 'A', 'V' };
  static unsigned char OutFile[]  = { 12, 'B', 'A', 'N', 'S', 'O', 'U', 'N', 'D', '.', 'D', 'A', 'T' };
  static unsigned char Keyboard[] = { 9, 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D', ':' };

  unsigned int i, k = 0;
  unsigned char Buf[6000], Play[3000];

  Set_EXOS_Var(7, 255);  // Click
  Open_Channel(105, (unsigned int) &Keyboard[0]);

  Open_Channel(40, (unsigned int) &FileName[0]);
  Get_Block(40, 44, (unsigned int) &Buf[0]); // Skip WAV header
  Get_Block(40, 5672, (unsigned int) &Buf[0]);
  Close_Channel(40);

  for (i = 0; i < 2500; i++) Play[i] = Buf[i] / 4;

  Open_Channel(41, (unsigned int) &OutFile[0]);
  Write_Block(41, 2500, (unsigned int) &Play[0]);
  Close_Channel(41);

  do { GetKey(0); } while (!Key);
  do {
    Say_Banana((unsigned int) &Play[0]);

    do { GetKey(0); } while (!Key);
    k = (k + 1) % 256;
    Set_EXOS_Var(27, k);
  } while (Key != 27);
}
