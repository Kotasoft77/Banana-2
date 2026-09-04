// ***********************************************************
// *                                                         *
// *                  B  A  N  A  N  A    2                  *
// *                                                         *
// *               Written by: Kotasoft, Korcsok Tamás       *
// *  Music, tracks and ideas: Korcsok Eliza                 *
// *               Written in: 2019, Gödöllõ                 *
// *                   System: Enterprise 128                *
// *       Source compiled by: Z88DK v1.99B (SDCC option)    *
// *      Images converted by: Kotasoft EP128pal             *
// *                Tested on: EP128emu 2.0.10 and           *
// *                           real Enterprise of course :)  *
// *        Special thanks to: http://enterpriseforever.com  *
// *                                                         *
// ***********************************************************

// Structure for loading .EPI image

struct PicType {
  unsigned char YSize, Blocks, VideoMode, Lines;
  unsigned char Pal, PixelLines;
} Pic;

// Video variables

unsigned int SaveLPTAddr, LPBAddr, VideoOfs;
unsigned char VideoSeg, VideoSeg1 = 0, VideoSeg2 = 0, VideoSeg3 = 0, VideoSeg, SegNum, DataSeg1 = 255, DataSeg2 = 255;
unsigned char Buf[92];

// Game and control variables

unsigned char Mode = 255, Key, Key_Matrix1, Key_Matrix2, Joy[3], ObjX, ObjY, Typ, Atr, FloorColour, NoMonsters = 0;
unsigned char Control[2], Energy[2], MinionX[3], MinionY[3], MonsterType, MonsterDirection;
unsigned char Player, GameEnd, SpriteCounter = 0, ControlDirection, MinionDirection, Fire, Swap, On_ladder, On_steps, Falling;
unsigned char LadderX1, LadderX2, LadderX3; // This is for skull-monster: skull can move between Ladder1 and Ladder2, Ladder3 is optional
unsigned int Points[2], KeysHave[10], Bananas, SpritePtr = 0, SecondPic = 0;

// Global variable(s) (registers) for data exchange between C and assembly parts

unsigned char RA;

// Pointers for the arrays, images and sprites on 0xB2 page

unsigned int MenuColours   = 0x8000;         //   89 (lines 94-182)
unsigned int ControlIcons  = 0x8000 + 89;    // 1088 [4][272]
unsigned int Digits        = 0x8000 + 1177;  //  160 [10][16]
unsigned int txt_Energy    = 0x8000 + 1337;  //   79
unsigned int txt_Exit      = 0x8000 + 1416;  //  114
unsigned int txt_GetReady  = 0x8000 + 1530;  //  121
unsigned int txt_GameOver  = 0x8000 + 1651;  //  121
unsigned int txt_PhilWon   = 0x8000 + 1772;  //  100
unsigned int txt_StuartWon = 0x8000 + 1872;  //  142
unsigned int Ladder        = 0x8000 + 2014;  //  407
unsigned int Brick         = 0x8000 + 2421;  //   50
unsigned int SmallKeys     = 0x8000 + 2471;  //   40 [5][8]
unsigned int Keys          = 0x8000 + 2511;  //  620 [5][2][62]
unsigned int Plant         = 0x8000 + 3131;  //  282
unsigned int Door          = 0x8000 + 3413;  //  322
unsigned int Worm          = 0x8000 + 3735;  //  226
unsigned int Objects       = 0x8000 + 3961;  // 1940 [10][2][97]
unsigned int Floor         = 0x8000 + 5901;  //  216 [8][27]
unsigned int Minions       = 0x8000 + 6117;  // 6160 [2][10][308]
unsigned int Steps         = 0x8000 + 12277; //   28 [2][14]
unsigned int I_ate_a_plant = 0x8000 + 12305; //  122
unsigned int DoorAnim      = 0x8000 + 12427; //   98
unsigned int NoMonstIcon   = 0x8000 + 12525; //  128
unsigned int Banana_Speach = 0x8000 + 12653; // 2500 (6 kHz, 8 bit, mono WAV without header)

// Musics

unsigned int MusMapAddr, MusTuneAddr; // These contain the address of the arrays MusicxMap and MusicxTune
unsigned char MusMapNum, MusMapPtr, MusTuneNum, MusTunePtr, MusTuneCnt, Envelope;
static unsigned char Music1Map[]  = { 12,0,0,0,0,65,65,0,0,0,0,114,114 };
static unsigned char Music1Tune[] = { 16,37,56,12,0b001010,41,56,12,0b101010,41, 0,12,0b010010,41,56,12,0b001010,44,58, 6,0b000110,42, 0, 6,0b000010,41,58,12,0b100110,39, 0,24,0b010010,
                                         37,56,12,0b001010,39,56,12,0b101010,39,56,12,0b011010,39,56,12,0b001010,42,54, 6,0b000110,41, 0, 6,0b000010,39,54,12,0b100110,37,54,24,0b010110,
                                      12,37,56,12,0b101010,41, 0,12,0b000010,41, 0,24,0b010010,44,58,12,0b100110,41, 0,12,0b000010,41, 0,24,0b010010,
                                         37,54,12,0b101010,39, 0,12,0b000010,39, 0,24,0b010010,42,56,12,0b100110,39, 0,12,0b000010,39, 0,24,0b010010,
                                      16,54,10,12,0b001001,54,10,12,0b100001,54,10,12,0b010001,54,10,12,0b001001,57,12, 6,0b001001,55, 0, 6,0b000001,54,12,12,0b101001,52, 0,24,0b010001,
                                         52,10,12,0b000101,52,10,12,0b100001,52,10,12,0b010001,52,10,12,0b000101,55, 9, 6,0b000101,54, 0, 6,0b000001,52, 9,12,0b100101,50,54,24,0b010001 };
static unsigned char Music2Map[]  = { 9,0,53,0,53,0,53,0,53,106 };
static unsigned char Music2Tune[] = { 13,59,56,14,0b001010,59, 0,14,0b100010,57,54,14,0b011010,55,54,14,0b000110,57,56,14,0b001010,59,56,14,0b100110,55,54,28,0b011010,
                                         52,56,14,0b001010,52, 0,14,0b100010,50,54,28,0b010110,52,56,14,0b001010,52, 0,14,0b100010,50,54,28,0b010110,
                                      13,47,56,14,0b001010,47,56,14,0b101010,45,54,14,0b010110,43,54,14,0b000110,45,56,14,0b001010,47,58,14,0b101010,43,56,28,0b010110,
                                         40,56,14,0b001010,40,56,14,0b101010,38,54,28,0b010110,40,56,14,0b001010,40,56,14,0b101010,38,54,28,0b010110,
                                       7,59,56,14,0b101010,59,56,14,0b100110,57,54,14,0b011010,57,54,14,0b010110,55,56,14,0b101010,55,56,14,0b100110,55,54,28,0b011110 };
static unsigned char Music3Map[]  = { 4,0,0,13,0 };
static unsigned char Music3Tune[] = {  3,42,48,20,0b011100,42,48,20,0b001100,43,47,40,0b101100,
                                       4,42,48,20,0b011100,42,48,20,0b001100,43,47,20,0b101100,45,45,20,0b001100 };

// Rooms
// 1=Floor 2=Steps 3=Ladder 4=Brick 5=Keys 6=Worm 7=Plant 8=Door 9=Banana
// 10=Heart 11=Broken heart 12=Bag 13=Stars 14=Skull 15=Dog 16=Fart blaster 17=Room number

unsigned int ObjPtr;
unsigned char Room, PrevRoom, ObjNum;
unsigned char RoomData[2444]; // RoomData: obj_num, X, Y, obj_type, obj_attr, X, Y, ..., the first two objects are always for the banana peels, eatable plant is always the 3rd object in the room
static unsigned int  RoomMap[]  = { 0,86,208,314,416,510,608,702,800,910,1016,1138,1248,1354,1472,1558,1672,1762,1868,1958,2060,2178,2276,2358 };
static unsigned char RoomOrig[] = { 21,02, 0,0,0,0,     0,0,0,0,      1,5,40,15,    1,5,85,15,    1,5,130,15,   3,38,40,0,   3,38,85,0,    3,38,130,0,   4,7,49,43,    7,6,95,0,
                                           7,70,140,0,  8,74,90,15,   9,25,21,0,    9,55,21,0,    9,70,66,0,    9,20,111,0,  9,7,156,0,    9,15,156,0,   9,23,156,0,   17,10,53,0,
                                           9,60,156,0,
                                    30,22, 0,0,0,0,     0,0,0,0,      7,50,140,0,   1,2,40,16,    1,2,85,16,    1,2,130,16,  8,2,90,05,    8,74,90,25,   3,14,40,0,    3,14,130,0,
                                           3,38,85,0,   3,62,40,0,    3,62,130,0,   9,8,18,0,     9,16,16,0,    9,24,15,0,   9,32,16,0,    9,40,18,0,    9,48,20,0,    9,56,21,0,
                                           9,64,20,0,   9,72,18,0,    10,40,66,0,   5,40,156,0,   7,27,140,0,   4,73,49,23,  17,75,53,0,   13,3,64,0,    9,6,151,0,    9,74,151,0,
                                    26,02, 0,0,0,0,     0,0,0,0,      1,2,40,2,     2,12,40,115,  2,54,40,15,   1,72,40,2,   1,27,85,6,    1,2,85,2,     7,3,50,0,     3,38,40,0,
                                           3,38,85,0,   11,40,21,0,   9,32,7,0,     9,48,7,0,     1,12,130,14,  3,58,130,0,  8,74,90,31,   8,2,0,15,     8,15,135,164, 4,28,139,43,
                                           17,30,143,0, 5,72,156,1,   9,49,156,0,   9,15,111,0,   12,4,156,0,   9,25,111,0,
/* 4 */                             25,31, 0,0,0,0,     0,0,0,0,      4,39,67,29,   17,41,140,0,  1,5,40,15,    1,5,85,15,   1,5,130,13,   3,13,40,0,    3,63,40,0,    6,75,143,1,
                                           7,63,95,0,   15,74,21,0,   9,6,111,0,    9,60,153,0,   3,27,85,0,    3,49,85,0,   3,27,130,0,   3,49,130,0,   8,2,135,25,   8,74,45,45,
                                           13,26,60,0,  11,54,60,0,   5,3,66,4,     9,16,111,0,   9,40,19,0,
                                    23,11, 0,0,0,0,     0,0,0,0,      7,65,95,0,    1,27,40,5,    1,2,85,3,     1,62,85,4,   1,27,130,9,   3,38,40,0,    3,38,85,0,    3,38,130,0,
                                           8,2,45,35,   8,74,45,50,   5,75,115,3,   16,4,17,0,    9,32,46,0,    9,55,156,0,  2,2,130,115,  9,7,95,0,     4,2,94,13,    17,2,104,0,
                                           12,50,111,0, 10,70,156,0,  9,25,156,0,
                                    24,30, 0,0,0,0,     0,0,0,0,      4,2,22,29,    4,74,67,29,   1,2,40,16,    1,2,85,16,   1,2,130,16,   8,2,135,45,   8,74,0,63,    17,76,98,0,
                                           3,13,40,0,   3,62,40,0,    3,13,85,0,    3,62,85,0,    3,13,130,0,   3,62,130,0,  10,40,20,0,   9,40,65,0,    9,32,110,0,   9,47,110,0,
                                           9,24,155,0,  9,55,155,0,   12,40,155,0,  14,48,64,2,
                                    23,33, 0,0,0,0,     0,0,0,0,      7,14,50,0,    1,2,40,14,    1,12,85,12,   1,39,130,1,  2,22,130,15,  2,44,130,115, 8,2,135,55,   8,74,135,75,
                                           1,2,130,2,   1,72,130,2,   3,28,40,0,    3,38,85,0,    4,48,49,23,   17,50,53,0,  7,4,5,0,      9,18,21,0,    9,5,111,0,    9,58,66,0,
                                           12,63,20,0,  10,75,111,0,  9,43,21,0,
/* 8 */                             24,00, 0,0,0,0,     0,0,0,0,      1,2,40,5,     1,67,40,3,    1,2,85,4,     1,2,130,16,  3,11,40,0,    3,11,85,0,    3,11,130,0,   2,34,40,30,
                                           8,2,0,65,    8,74,0,85,    8,74,135,185, 3,62,130,0,   4,50,94,23,   17,52,98,0,  9,3,65,0,     12,3,110,0,   9,3,155,0,    11,40,150,0,
                                           7,73,95,0,   9,59,110,0,   9,28,155,0,   9,51,155,0,
                                    27,33, 0,0,0,0,     0,0,0,0,      1,2,40,16,    1,2,85,16,    1,2,130,16,   3,14,40,0,   3,14,85,0,    3,14,130,0,   3,38,40,0,    3,38,85,0,
                                           3,38,130,0,  3,62,40,0,    3,62,85,0,    3,62,130,0,   4,26,67,29,   4,56,67,19,  17,28,140,0,  16,20,17,0,   8,2,135,75,   8,74,135,95,
                                           9,5,66,0,    10,5,111,0,   9,49,156,0,   12,74,66,0,   9,74,111,0,   5,49,111,3,  9,49,66,0,
                                    26,00, 0,0,0,0,     0,0,0,0,      1,2,40,16,    1,2,85,16,    1,2,130,16,   3,26,40,0,   3,26,85,0,    3,26,130,0,   15,10,111,0,  4,36,49,23,
                                           17,38,53,0,  8,2,0,85,     8,2,45,05,    8,2,135,75,   8,74,0,35,    8,74,45,223, 8,74,135,154, 3,59,40,0,    3,59,130,0,   4,36,139,23,
                                           8,45,45,25,  8,45,135,105, 9,13,66,0,    9,17,156,0,   9,38,21,0,    9,52,21,0,
                                    30,13, 0,0,0,0,     0,0,0,0,      1,2,85,2,     2,12,85,115,  2,54,85,15,   1,72,85,2,   1,27,130,6,   3,38,40,0,    3,38,85,0,    3,38,130,0,
                                           8,2,45,95,   8,74,45,175,  8,74,135,115, 4,2,139,33,   17,4,143,0,   1,12,40,12,  7,3,104,0,    7,73,95,0,    1,72,130,2,   10,16,20,0,
                                           10,24,20,0,  10,32,20,0,   13,40,18,0,   10,48,20,0,   10,56,20,0,   10,64,20,0,  9,19,150,0,   9,29,150,0,   9,49,150,0,   9,59,150,0,
/* 12 */                            27,30, 0,0,0,0,     0,0,0,0,      7,45,95,0,    7,65,95,0,    1,2,40,16,    1,2,85,2,    1,27,85,11,   1,2,130,2,    1,32,130,8,   13,35,19,0,
                                           3,33,40,0,   3,33,85,0,    3,33,130,0,   8,2,135,105,  8,74,0,125,   8,2,0,200,   5,56,111,0,   6,75,143,11,  4,74,49,23,   17,76,53,0,
                                           7,3,50,0,    9,4,111,0,    9,18,122,0,   12,25,21,0,   10,60,66,0,   9,55,21,0,   9,55,156,0,
                                    26,33, 0,0,0,0,     0,0,0,0,      1,12,40,12,   1,2,85,4,     1,62,85,4,    1,2,130,2,   1,22,130,8,   1,72,130,2,   3,38,40,0,    3,38,85,0,
                                           3,38,130,0,  4,34,46,17,   4,47,46,17,   8,2,135,115,  8,74,135,135, 17,47,60,0,  9,22,19,0,    9,57,19,0,    12,4,64,0,    11,75,64,0,
                                           9,4,109,0,   9,75,109,0,   13,24,109,0,  9,54,109,0,   9,24,154,0,   9,54,154,0,
                                    29,33, 0,0,0,0,     0,0,0,0,      1,24,160,7,   2,19,160,5,   2,57,160,105, 3,38,115,0,  1,32,115,4,   1,2,100,5,    1,57,100,5,   2,26,100,105,
                                           2,50,100,5,  8,2,135,125,  8,74,135,142, 3,2,55,0,     3,73,55,0,    1,11,55,1,   1,68,55,1,    1,21,40,3,    1,48,40,3,    2,14,40,5,
                                           2,61,40,105, 9,40,45,0,    16,5,17,0,    4,2,105,22,   17,4,110,0,   9,20,80,0,   9,60,80,0,    9,30,20,0,    9,50,20,0,
                                    21,00, 0,0,0,0,     0,0,0,0,      1,27,40,6,    1,27,85,6,    3,38,40,0,    9,29,19,0,   5,50,19,1,    1,2,130,2,    8,2,135,135,  1,22,130,8,
                                           9,5,50,0,    9,74,50,0,    11,40,154,0,  4,74,0,22,    17,76,6,0,    7,38,95,0,   1,72,130,2,   13,3,109,0,   12,76,109,0,  9,25,154,0,
                                           9,55,154,0,
/* 16 */                            28,03, 0,0,0,0,     0,0,0,0,      4,74,22,29,   1,2,40,16,    1,2,85,16,    1,2,130,16,  8,2,0,95,     8,74,135,105, 17,76,98,0,   3,16,40,0,
                                           3,59,40,0,   3,38,40,0,    3,16,85,0,    3,59,85,0,    3,38,85,0,    3,16,130,0,  3,59,130,0,   3,38,130,0,   7,5,50,0,     7,4,95,0,
                                           7,5,140,0,   9,50,60,0,    13,50,105,0,  9,50,150,0,   14,48,64,2,   9,29,60,0,   5,29,105,2,   11,29,150,0,
                                    22,33, 0,0,0,0,     0,0,0,0,      16,4,17,0,    4,38,67,29,   17,40,140,0,  1,2,40,16,   1,2,85,14,    1,2,130,16,   8,2,135,25,   8,74,135,175,
                                           7,63,50,0,   6,75,98,4,    3,28,130,0,   3,8,85,0,     3,8,40,0,     3,48,40,0,   3,48,85,0,    3,48,130,0,   9,70,21,0,    9,25,66,0,
                                           9,20,111,0,  9,60,156,0,
                                    26,03, 0,0,0,0,     0,0,0,0,      4,2,67,29,    4,74,22,29,   1,2,40,16,    1,2,85,16,   1,2,130,16,   8,2,0,165,    8,74,135,35,  17,4,98,0,
                                           3,13,40,0,   3,62,40,0,    3,38,40,0,    3,13,85,0,    3,62,85,0,    3,38,85,0,   3,13,130,0,   3,62,130,0,   3,38,130,0,   14,48,64,2,
                                           9,27,65,0,   9,52,65,0,    9,27,110,0,   9,52,110,0,   11,27,155,0,  13,52,150,0,
                                    23,33, 0,0,0,0,     0,0,0,0,      2,54,85,15,   1,72,85,2,    6,75,53,6,    1,2,130,16,  8,2,135,75,   8,74,135,195, 3,28,130,0,   4,74,94,23,
                                           17,77,99,0,  9,16,111,0,   7,3,95,0,     7,16,50,0,    7,30,53,0,    7,43,42,0,   7,4,25,0,     7,22,6,0,     7,36,3,0,     7,60,7,0,
                                           9,44,111,0,  9,54,156,0,
/* 20 */                            25,30, 0,0,0,0,     0,0,0,0,      4,2,22,29,    4,74,67,29,   1,2,40,16,    1,2,85,16,   1,2,130,16,   8,2,135,185,  8,74,0,75,    17,76,98,0,
                                           3,23,40,0,   3,52,40,0,    3,23,85,0,    3,52,85,0,    3,23,130,0,   3,52,130,0,  7,13,5,0,     7,13,50,0,    7,13,95,0,    7,62,50,0,
                                           7,62,95,0,   9,39,66,0,    9,39,111,0,   13,39,150,0,  14,48,64,2,
                                    29,03, 0,0,0,0,     0,0,0,0,      1,2,40,3,     1,27,40,2,    1,47,40,2,    1,67,40,3,   1,17,85,2,    1,37,85,2,    1,57,85,2,    1,2,130,3,
                                           1,27,130,2,  1,47,130,2,   1,67,130,3,   8,2,0,115,    8,74,135,215, 9,30,19,0,   9,50,19,0,    10,20,64,0,   5,40,64,4,    11,60,64,0,
                                           9,10,109,0,  12,30,109,0,  9,50,109,0,   13,70,109,0,  4,2,139,23,   17,4,143,0,  9,20,154,0,   9,40,154,0,   9,60,154,0,
                                    24,22, 0,0,0,0,     0,0,0,0,      4,2,139,33,   4,28,139,23,  4,48,139,23,  4,70,139,33, 17,50,145,0,  1,2,133,3,    1,27,133,2,   1,47,133,2,
                                           1,67,133,3,  8,2,93,205,   8,74,93,115,  3,28,88,0,    1,22,88,4,    1,57,43,5,   1,2,43,1,     2,5,43,115,   2,41,43,15,   9,4,19,0,
                                           9,60,19,0,   7,71,8,0,     12,50,114,0,  9,30,66,0,
                                    20,22, 0,0,0,0,     0,0,0,0,      4,2,139,93,   4,38,139,93,  4,74,139,23,  17,74,145,0, 1,2,133,16,   8,2,93,95,    8,74,93,231,  1,29,118,5,
                                           2,24,118,5,  2,52,118,105, 3,38,73,0,    1,2,73,16,    15,74,54,0,   9,31,98,0,   9,49,98,0,    9,12,53,0,    9,59,53,0,    12,27,53,0,
/* 24 */                            21,33, 0,0,0,0,     0,0,0,0,      8,2,135,225,  8,74,135,95,  4,2,22,29,    4,74,22,29,  17,76,25,29,  3,39,130,0,   1,18,130,9,   3,18,85,0,
                                           1,18,85,4,   3,29,40,0,    1,18,40,4,    3,52,85,0,    1,46,85,4,    3,46,40,0,   9,21,19,0,    9,59,64,0,    9,19,64,0,    13,40,109,0,
                                           12,27,156,0 };

// Functions

#define Out(port_, value_)  { static __sfr __at (port_) tmp_; tmp_ = (value_); }
#define Peek(addr_)         *((unsigned char *) (addr_))
#define Poke(addr_, value_) *((unsigned char *) (addr_)) = (value_)

static unsigned char Last_IRQ_Cnt = 0;

void Timer_Init()
{
  Out(0xB2, 0xFF);
  Last_IRQ_Cnt = Peek(0xBFEC);
  Out(0xB2, DataSeg1);
}

void Timer_Wait(unsigned char n)
{
  volatile unsigned char *p = (unsigned char *) 0xBFEC;
  unsigned char t;

  Out(0xB2, 0xFF);
  while ((unsigned char) ((t = *p) - Last_IRQ_Cnt) < n);
  Last_IRQ_Cnt = t;
  Out(0xB2, DataSeg1);
}

unsigned char Ask_Keyboard_Matrix(unsigned char Row) __naked
{
  (void) Row;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"    // Skip over return address on stack
    "ld      A,(HL)\n"   // Matrix row
    "out     (0xB5),A\n"
    "in      A,(0xB5)\n"
    "ld      L,A\n"      // HL = return parameter
    "ret\n"
  );
}

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

unsigned char Ask_EXOS_Var(unsigned char Variable) __naked
{
  (void) Variable;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      C,(HL)\n"  // C = Variable
    "ld      B,0\n"     // ASK mode
    "rst     0x30\n"
    ".byte   16\n"
    "ld      L,D\n"     // HL = return parameter
    "ret\n"
  );
}

unsigned char Get_Joy(unsigned char Channel, unsigned char Joy_number) __naked
{
  (void) Channel;
  (void) Joy_number;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      A,(HL)\n"  // A = Channel
    "inc     HL\n"
    "ld      C,(HL)\n"  // C = Joy_number
    "ld      B,9\n"
    "rst     0x30\n"
    ".byte   11\n"
    "ld      L,C\n"     // C = joystick status (1=right, 2=left, 4=down, 8=up, 16=fire)
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

void CopyMem(unsigned int From, unsigned int To, unsigned int Count) __z88dk_callee __naked
{
  (void) From;
  (void) To;
  (void) Count;
  __asm__ (
    "pop     AF\n"
    "pop     HL\n"      // HL = From
    "pop     DE\n"      // DE = To
    "pop     BC\n"      // BC = Count
    "push    AF\n"
    "ldir\n"
    "ret\n"
  );
}

void FillMem(unsigned int From, unsigned int Count, unsigned char Value) __z88dk_callee __naked
{
  (void) From;
  (void) Count;
  (void) Value;
  __asm__ (
    "pop     HL\n"      // HL = From
    "pop     DE\n"
    "pop     BC\n"      // BC = Count
    "dec     SP\n"
    "pop     AF\n"
    "push    HL\n"
    "ld      L,E\n"
    "ld      H,D\n"
    "ld      (HL),A\n"  // A = Value
    "cpi\n"
    "ret     PO\n"
    "ex      DE,HL\n"
    "ldir\n"
    "ret\n"
  );
}

unsigned char Rnd255() __naked
{
  __asm__ (
    "rst     30h\n"
    ".byte   32\n"      // Read time
    "xor     A\n"
    "ld      A,R\n"     // R = Refresh register
    "add     A,C\n"     // Hours
    "add     A,D\n"     // Minutes
    "add     A,E\n"     // Seconds

    "ld      B,A\n"     // XOR the double with R register
    "rlc     B\n"
    "ld      A,R\n"     // R = Refresh register
    "xor     B\n"

    "ld      L,A\n"     // HL = return value
    "ld      H,0\n"
    "ret\n"
  );
}

unsigned char Get_Segment()
{
  __asm__ (
    "rst     0x30\n"
    ".byte   24\n"
    "ld      (_RA),A\n"
    "ld      A,C\n"
    "ld      (_SegNum),A\n"
  );
  return RA == 0;
}

void Free_Segment(unsigned char Segment_Num) __naked
{
  (void) Segment_Num;
  __asm__ (
    "ld      HL,2\n"
    "add     HL,SP\n"   // Skip over return address on stack
    "ld      C,(HL)\n"  // C = Segment_Num
    "rst     0x30\n"
    ".byte   25\n"
    "ret\n"
  );
}

unsigned char Get_VideoSegments()
{
  unsigned char OK;

  do {
    if (OK = Get_Segment()) {
      if (SegNum >= 0xFC) {
        if (VideoSeg1 == 0) VideoSeg1 = SegNum;
        else if (VideoSeg2 == 0) VideoSeg2 = SegNum;
        else VideoSeg3 = SegNum;
      }
      else {
        if (DataSeg1 == 255) DataSeg1 = SegNum;
        else if (DataSeg2 == 255) DataSeg2 = SegNum;
      }
    }
  } while (((VideoSeg3 == 0) || (DataSeg2 == 255)) && OK);

  return OK;
}

void Set_Envelope()
{
  static unsigned char Envelope1[] = { 27,'E',0x01,0x03,0xFF,0x00,0x00,0x3F,0x17,0x01,0x00,0x00,0x02,0xD8,0x28,0x05,0x00,0x00,0x00,0xE9,0xC1,0x1E,0x00 };
  static unsigned char Envelope2[] = { 27,'E',0x02,0x03,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0x00,0xE2,0xE2,0x03,0x00,0x00,0x00,0xDF,0xDF,0x1E,0x00 };
  static unsigned char Envelope3[] = { 27,'E',0x03,0x03,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0x00,0xD8,0xD8,0x02,0x00,0x00,0x00,0xE9,0xE9,0x0A,0x00 };
  static unsigned char Envelope4[] = { 27,'E',0x04,0x03,0xFF,0x00,0x00,0x3F,0x17,0x01,0x00,0x00,0x00,0xD8,0x28,0x05,0x00,0x00,0x00,0xE9,0xC1,0x1E,0x00 };
  static unsigned char Envelope5[] = { 27,'E',0x05,0x06,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0xF6,0xEC,0xEC,0x06,0x00,0x00,0x04,0x14,0x14,0x01,0x00,0x00,0xF6,0xEC,0xEC,0x06,0x00,0x00,0x04,0x14,0x14,0x01,0x00,0x00,0xF6,0xC1,0xC1,0x12,0x00 };
  static unsigned char Envelope6[] = { 27,'E',0x06,0x04,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0x02,0xE2,0xE2,0x04,0x00,0x00,0x08,0x1E,0x1E,0x01,0x00,0x00,0x02,0xC1,0xC1,0x14,0x00 };
  static unsigned char Envelope7[] = { 27,'E',0x07,0x02,0xFF,0x00,0x28,0x2D,0x2D,0x05,0x00,0x00,0xD8,0xD3,0xD3,0x1E,0x00 };
  static unsigned char Envelope8[] = { 27,'E',0x08,0x05,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0x18,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0xD8,0x00,0x00,0x03,0x00,0x00,0x00,0xC1,0xC1,0x10, 0x00 };
  static unsigned char Envelope9[] = { 27,'E',0x09,0x03,0xFF,0x00,0x00,0x3F,0x3F,0x01,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x00,0xD8,0xC1,0xC1,0x18,0x00 };

  Write_Block(103, 23, (unsigned int) &Envelope1[0]);
  Write_Block(103, 23, (unsigned int) &Envelope2[0]);
  Write_Block(103, 23, (unsigned int) &Envelope3[0]);
  Write_Block(103, 23, (unsigned int) &Envelope4[0]);
  Write_Block(103, 41, (unsigned int) &Envelope5[0]);
  Write_Block(103, 29, (unsigned int) &Envelope6[0]);
  Write_Block(103, 17, (unsigned int) &Envelope7[0]);
  Write_Block(103, 35, (unsigned int) &Envelope8[0]);
  Write_Block(103, 23, (unsigned int) &Envelope9[0]);
}

void Clear_Sound()
{
  unsigned char ClrSnd = 0x1A;
  
  Write_Block(103, 1, (unsigned int) &ClrSnd);
}

unsigned char Sound(unsigned char Envelope, unsigned int Pitch, unsigned char VolLeft, unsigned char VolRight,
                    unsigned char Style, unsigned char Source, unsigned int Duration, unsigned char Sync)
{
  Buf[0] = 27; Buf[1] = 'S'; Buf[2] = Envelope; Buf[3] = Pitch / 256; Buf[4] = Pitch % 256; Buf[5] = VolLeft;
  Buf[6] = VolRight; Buf[7] = Style; Buf[8] = Source; Buf[9] = Duration % 256; Buf[10] = Duration / 256; Buf[11] = Sync;
  return Write_Block(103, 12, (unsigned int) &Buf[0]);
}

void Next_Sound()
{
  unsigned char DrumL, DrumR, GuitL, GuitR, Style0, Style1, Value;

  // End of tune: go to the next tune
  if (MusTuneCnt >= MusTuneNum) {
    MusMapPtr++;
    MusTunePtr = Peek(MusMapAddr + MusMapPtr); // Tune pointer points the first sound (pitch), not on the number of sounds
    MusTuneNum = Peek(MusTuneAddr + MusTunePtr++);
    MusTuneCnt = 0;
  }
  // End of whole music: restart with the first tune
  if (MusMapPtr > MusMapNum) {
    MusMapPtr = 1;
    MusTunePtr = Peek(MusMapAddr + MusMapPtr);
    MusTuneNum = Peek(MusTuneAddr + MusTunePtr++);
    MusTuneCnt = 0;
  }
  Value  = Peek(MusTuneAddr + MusTunePtr + 3);
  DrumL  = (Value & 32) * 6;
  DrumR  = (Value & 16) * 12;
  GuitL  = (Value & 8) * 16 + 31;
  GuitR  = (Value & 4) * 32 + 31;
  Style1 = (Value & 2) * 16;
  Style0 = (Value & 1) * 32;
  if (!Sound(Envelope, 2 * Peek(MusTuneAddr + MusTunePtr), 127, 127, Style0, 0, Peek(MusTuneAddr + MusTunePtr + 2), 2))
    if (!Sound(2, 2 * Peek(MusTuneAddr + MusTunePtr + 1), GuitL, GuitR, Style1, 1, Peek(MusTuneAddr + MusTunePtr + 2), 2))
      if (!Sound(3, 0, DrumL, DrumR, 0, 3, Peek(MusTuneAddr + MusTunePtr + 2), 2)) {
        MusTunePtr += 4; MusTuneCnt++;
      }
}

void Set_Character(unsigned char Code, unsigned char B1, unsigned char B2, unsigned char B3,  unsigned char B4,
                   unsigned char B5, unsigned char B6, unsigned char B7, unsigned char B8, unsigned char B9)
{
  unsigned int Address;

  Out(0xB3, 0xFF);
  Address = 0xF480 + Code;
  if (Code > 127) Address -= 32;
  Poke(Address, B1);
  Poke(Address + 128, B2);
  Poke(Address + 256, B3);
  Poke(Address + 384, B4);
  Poke(Address + 512, B5);
  Poke(Address + 640, B6);
  Poke(Address + 768, B7);
  Poke(Address + 896, B8);
  Poke(Address + 1024, B9);
}

void Print(unsigned char Spaces, unsigned int Address)
{
  unsigned char i, Space = 32;

  for (i = 1; i <= Spaces; i++) Write_Block(102, 1, (unsigned int) &Space);
  Write_Block(102, Peek(Address), Address + 1);
  Buf[0] = 13;  Buf[1] = 10;
  Write_Block(102, 2, (unsigned int) &Buf[0]);
}

void GetKey(unsigned char Wait_for_keypress)
{
  unsigned char i;

  if (Wait_for_keypress) {
    if (!Channel_Status(105)) Get_Block(105, 1, (unsigned int) &Key);
    Get_Block(105, 1, (unsigned int) &Key);
  }
  else {
    if (Channel_Status(105)) Key = 0;
    else Get_Block(105, 1, (unsigned int) &Key);
  }

  // Get the status of the joysticks
  for (i = 0; i <= 2; i++)
    if ((Control[0] == i) || (Control[1] == i)) Joy[i] = Get_Joy(105, i);

  // Get rows 1 & 2 of the keyboard matrix
  Key_Matrix1 = Ask_Keyboard_Matrix(1); // 247 = "d", 239 = "f", 251 = "g"
  Key_Matrix2 = Ask_Keyboard_Matrix(2); // 247 = "r", 253 = "q"
}

void Get_Address(unsigned char Scr, unsigned char Line)
{
  switch (Scr) {
    case 0: // LPT1
      VideoSeg = VideoSeg3;
      VideoOfs = 0;
      break;
    case 1: // LPT2
      VideoSeg = VideoSeg3;
      VideoOfs = 4144;
      break;
    case 2: // LPT3
      VideoSeg = VideoSeg3;
      VideoOfs = 7024;
      break;
    case 3: // Header
      VideoSeg = VideoSeg3;
      VideoOfs = Line * 84 + 9904;
      break;
    case 4: // Head+score
      VideoSeg = VideoSeg2;
      VideoOfs = Line * 84 + 15120;
      break;
    case 5: // Menu attr
      VideoSeg = VideoSeg2;
      VideoOfs = Line * 84;
      break;
    case 6: // Menu 16
      VideoSeg = VideoSeg2;
      VideoOfs = Line * 84 + 13608;
      break;
    case 7: // Footer
      VideoSeg = VideoSeg3;
      VideoOfs = Line * 84 + 13684;
      break;
    case 8: // Game track
      VideoSeg = VideoSeg1;
      VideoOfs = Line * 84;
      break;
    case 9: // Info
      VideoSeg = DataSeg2;
      VideoOfs = Line * 84;
      break;
    case 10: // Head+score save
      VideoSeg = VideoSeg1;
      VideoOfs = Line * 84 + 15120;
      break;
  }
  Out(0xB3, VideoSeg);
}

void Set_LPT_Address(unsigned int Nick_Address)
{
  unsigned int Calc_LPT_Addr;

  Calc_LPT_Addr = (Nick_Address / 16) + 0xC000;
  Out(0x82, Calc_LPT_Addr % 256);
  Out(0x83, Calc_LPT_Addr / 256);
}

void Add_LPB()
{
  Out(0xB3, VideoSeg3);
  CopyMem((unsigned int) &Buf[0], LPBAddr, 16);
  LPBAddr += 16;
}

void Box16B(unsigned char Screen, unsigned char X_in_bytes, unsigned char Y,
            unsigned char XSize_in_bytes, unsigned char YSize, unsigned char Colour)
{
  unsigned char i, Value;

  Get_Address(Screen, Y);
  VideoOfs += (X_in_bytes + 0xC000);
  // Bit order in 16 colours mode: 15370426, usage of 'Colour' bits: 3210 -> 00221133
  Value = (Colour & 1) * 192 + (Colour & 2) * 6 + (Colour & 4) * 12 + ((Colour & 8) / 8) * 3;
  for (i = 1; i <= YSize; i++) {
    FillMem(VideoOfs, XSize_in_bytes, Value);
    VideoOfs += 84;
  }
}

void Box16(unsigned char Screen, unsigned char X, unsigned char Y,
           unsigned char XSize, unsigned char YSize, unsigned char Colour)
{
  unsigned char i, j, Value, Pixel;
  unsigned int ScrAddress;

  Get_Address(Screen, Y);
  VideoOfs += (X / 2 + 0xC000);
  Value = (Colour & 1) * 192 + (Colour & 2) * 6 + (Colour & 4) * 12 + ((Colour & 8) / 8) * 3;

  for (i = 1; i <= YSize; i++) {
    ScrAddress = VideoOfs;

    for (j = 1; j <= XSize; j++) {

      // First pixel which is on the right (low) half byte
      if ((j == 1) && (X % 2)) Pixel = (Peek(ScrAddress) & 170) | (Value & 85);

      // Last pixel which is on the left (high) half byte
      else if ((j == XSize) && ((X + XSize) % 2)) Pixel = (Peek(ScrAddress) & 85) | (Value & 170);

      // Any other case fill the whole byte and increase 'j' (skip two pixels / one byte)
      else {
        Pixel = Value; j++;
      }

      Poke(ScrAddress++, Pixel);
    }
   VideoOfs += 84;
  }
}

unsigned char GetPixel16(unsigned char Screen, unsigned char X, unsigned char Y)
// Function returns with the stored bit order (b1,b5,b3,b7|b0,b4,b2,b6) instead of the colour code
{
  Get_Address(Screen, Y);
  VideoOfs += (X / 2 + 0xC000);
  return Peek(VideoOfs) & ((X % 2) ? 85 : 170);
}

void GetPic16B(unsigned char Screen, unsigned char X_in_bytes, unsigned char Y,
               unsigned char XSize_in_bytes, unsigned char YSize, unsigned int Address)
{
  unsigned char i;

  Get_Address(Screen, Y);
  VideoOfs += (X_in_bytes + 0xC000);
  Poke(Address++, XSize_in_bytes); Poke(Address++, YSize);
  for (i = 1; i <= YSize; i++) {
    CopyMem(VideoOfs, Address, XSize_in_bytes);
    VideoOfs += 84; Address += XSize_in_bytes;
  }
}

void PutPic16B(unsigned char Screen, unsigned char X_in_bytes, unsigned char Y, unsigned int Address)
{
  unsigned char i, XSize_in_bytes, YSize;

  Get_Address(Screen, Y);
  VideoOfs += (X_in_bytes + 0xC000);
  XSize_in_bytes = Peek(Address++);
  YSize = Peek(Address++);
  for (i = 1; i <= YSize; i++) {
    CopyMem(Address, VideoOfs, XSize_in_bytes);
    VideoOfs += 84; Address += XSize_in_bytes;
  }
}

void Number(unsigned char Scr, unsigned char X_in_bytes, unsigned char Y, unsigned char Len, unsigned int Value)
{
  unsigned int V, i;
  unsigned char PosX;

  PosX = X_in_bytes + Len * 2 - 2;
  V = Value;
  for (i = 0; i < Len; i++) {
    PutPic16B(Scr, PosX, Y, Digits + (V % 10) * 16);
    V = V / 10; PosX -= 2;
  }  
}

void Scores_Energy_Keys(unsigned char Who, unsigned char Dsp_scores, unsigned char Dsp_energy, unsigned char Dsp_keys)
{
  unsigned char i;

  if (Dsp_scores) {
    Number(4, 30 + Who * 29, 1, 3, Points[Who]);
    Number(4, 1, 1, 3, Bananas);
  }
  if (Dsp_energy) {
    i = (Energy[Who] + 1) / 2;
    if (i) Box16(4, 38 + Who * 58, 1, i, 3, 11 + Who * 2);
    Box16(4, 38 + Who * 58 + i, 1, 21 - i, 3, 8);
  }
  if (Dsp_keys) {
    for (i = 0; i <= 4; i++) {
      if (KeysHave[Who * 5 + i]) PutPic16B(4, 19 + i * 2 + Who * 29, 5, SmallKeys + i * 8);
      else Box16B(4, 19 + i * 2 + Who * 29, 5, 2, 3, 8);
    }
  }
}

void Load_Pic()
{
  unsigned char Block, y, i, j;
  unsigned int NickAddress;
  static unsigned char FileName[] = { 11, 'B', 'A', 'N', 'A', 'N', 'A', '2', '.', 'E', 'P', 'I' };
  static unsigned char TextColours[] = { 73, 217, 203, 219, 218, 146, 150, 182, 174, 188, 60, 108, 101, 109, 105 };

  Set_Character( 91, 0x07, 0x0F, 0x1C, 0x3B, 0x02, 0x3B, 0x3C, 0x37, 0x38);
  Set_Character( 93, 0xC0, 0xE1, 0x71, 0xB9, 0x81, 0xB9, 0x79, 0xD9, 0x3A);
  Set_Character( 40, 0x00, 0x40, 0x40, 0x40, 0xC0, 0xE0, 0xE0, 0xC0, 0x80);
  Set_Character(123, 0x5F, 0xEF, 0xB4, 0xAA, 0xA3, 0xB0, 0xEF, 0x46, 0x0E);
  Set_Character(125, 0xF4, 0xE9, 0x5A, 0xAC, 0x88, 0x10, 0xE0, 0xC0, 0xE0);
  Set_Character( 41, 0x80, 0x00, 0x02, 0x02, 0x03, 0x07, 0x1F, 0xFE, 0x7C);
  Buf[0] = 1; Buf[1] = 32; Print(16, (unsigned int) &Buf[0]);
  Buf[0] = 3; Buf[1] = 91; Buf[2] = 93; Buf[3] = 40; Print(16, (unsigned int) &Buf[0]);
  Buf[0] = 3; Buf[1] = 123; Buf[2] = 125; Buf[3] = 41; Print(16, (unsigned int) &Buf[0]);
  Open_Channel(40, (unsigned int) &FileName[0]);
  Get_Block(40, 2500, Banana_Speach);
  Get_Block(40, 12, (unsigned int) &Buf[0]);
  Pic.YSize = Buf[8];
  Set_EXOS_Var(28, Buf[11]); // BIAS

  LPBAddr = 0xC000;
  Buf[0] = 229; Buf[1] = 18; Buf[2] = 63; FillMem((unsigned int) &Buf[3], 13, 0); Add_LPB();

  for (Block = 3; Block <= 9; Block++) {

    // Skip the 12 bytes long headers and get the new address for the LPB data
    if (Block > 7) Get_Block(40, 12, (unsigned int) &Buf[0]);

    // Display the extra screen, jump the info screen over
    if ((Block == 9) && (Rnd255() > 200)) {
      Get_Block(40, 3, (unsigned int) &Buf[16]);
      Pic.Lines = Buf[17];
      for (i = 0; i < Pic.Lines; i++) Get_Block(40, 92, (unsigned int) &Buf[0]);
      Get_Block(40, 12, (unsigned int) &Buf[0]);
    }

    Get_Block(40, 3, (unsigned int) &Buf[16]);
    Pic.VideoMode = Buf[16];
    Pic.Lines = Buf[17];
    Pic.PixelLines = ((Buf[18] % 2) == 1);
    Pic.Pal = (Buf[18] > 1);
    j = 0;
    for (y = 0; y < Pic.Lines; y++) {
      Get_Address(Block, y);
      if (Pic.PixelLines || (y == 0)) {
        Get_Block(40, 8, (unsigned int) &Buf[8]);
        // Find the colour of the menu text (lines 94-182)
        if ((Block == 5) && (y >= 40) && (y <= 128)) { // 40 = 94th physical line - 9*6 header lines, 128 = 182 - 54
          Out(0xB2, DataSeg1);
          Poke(MenuColours + y - 40, 255);
          for (i = 0; i <= 7; i++)
            if (Buf[8 + i] == 146) {
              Poke(MenuColours + y - 40, i);
              Buf[8 + i] = TextColours[(j >> 2) % 15];
              j++;
            }
        }
        // Create LPB for the line
        Buf[0] = Pic.PixelLines ? 255 : (unsigned char) 256 - Pic.Lines;
        Buf[1] = Pic.VideoMode;
        Buf[2] = 10;
        Buf[3] = 52;
        // Use the 'Menu attr' address when 'Info' screen is loaded because screen data is copied there physically
        if (Block == 9) {
          Buf[4] = (y * 84) % 256;
          Buf[5] = (VideoSeg2 % 4) * 64 + (y * 84) / 256;
        }
        else {
          Buf[4] = VideoOfs % 256;
          Buf[5] = (VideoSeg % 4) * 64 + VideoOfs / 256;
        }
        if (Pic.VideoMode == 20) {
          Buf[6] = Buf[4] < 214 ? Buf[4] + 42 : Buf[4] - 214;
          Buf[7] = Buf[4] < 214 ? Buf[5] : Buf[5] + 1;
        }
        Add_LPB();
      }
      Out(0xB3, VideoSeg);
      Get_Block(40, 84, VideoOfs + 0xC000);
    }

    // Finish the LPT if all visible blocks are loaded
    if (Block == 7) {
      FillMem((unsigned int) &Buf[4], 12, 0);
      Buf[0] = Pic.YSize; Buf[1] = 18; Buf[2] = 63; Buf[3] = 0; Add_LPB();
      Buf[0] = 236; Buf[1] = 18; Buf[2] = 63; Buf[3] = 0; Add_LPB();
      Buf[0] = 252; Buf[1] = 16; Buf[2] = 6; Buf[3] = 63; Add_LPB();
      Buf[0] = 255; Buf[1] = 144; Buf[2] = 63; Buf[3] = 32; Add_LPB();
      Buf[0] = 252; Buf[1] = 18; Buf[2] = 6; Buf[3] = 63; Add_LPB();
      Buf[0] = 255; Buf[1] = 19; Buf[2] = 63; Buf[3] = 0; Add_LPB();
    }
  }
  Close_Channel(40);

  // Save header+score
  Get_Address(10, 0);
  Out(0xB2, VideoSeg);
  GetPic16B(4, 0, 0, 65, 9, VideoOfs + 0x8000);
  Out(0xB2, DataSeg1);

  // Controls
  for (i = 0; i <= 3; i++) GetPic16B(6, i * 15, 0, 15, 18, ControlIcons + i * 272);
  GetPic16B(6, 60, 0, 7, 18, NoMonstIcon);

  // Digits
  for (i = 0; i <= 9; i++) GetPic16B(8, 14 + i * 2, 114, 2, 7, Digits + i * 16);

  // Texts
  GetPic16B(8, 2, 114, 11, 7, txt_Energy);
  GetPic16B(8, 35, 114, 16, 7, txt_Exit);
  GetPic16B(8, 52, 114, 17, 7, txt_GetReady);
  GetPic16B(8, 2, 122, 17, 7, txt_GameOver);
  GetPic16B(8, 20, 122, 14, 7, txt_PhilWon);
  GetPic16B(8, 35, 122, 20, 7, txt_StuartWon);

  // Objects
  GetPic16B(8, 20, 130, 9, 45, Ladder);
  GetPic16B(8, 29, 135, 8, 40, Door);
  GetPic16B(8, 47, 150, 4, 12, Brick);
  GetPic16B(8, 74, 0, 8, 35, Plant);
  GetPic16B(8, 75, 82, 7, 32, Worm);
  GetPic16B(8, 74, 115, 8, 15, I_ate_a_plant);
  for (i = 0; i <= 4; i++) GetPic16B(8, 49, 135 + i * 3, 2, 3, SmallKeys + i * 8);
  for (i = 0; i <= 7; i++) GetPic16B(8, 2 + i * 5, 35, 5, 5, Floor + i * 27);
  for (j = 0; j <= 1; j++) {
    for (i = 0; i <= 4; i++) GetPic16B(8, 53 + i * 6, 135 + j * 13, 5, 12, Keys + i * 124 + j * 62);
    for (i = 0; i <= 9; i++) GetPic16B(8, 20 + i * 6, 40 + j * 20, 5, 19, Objects + i * 194 + j * 97);
    for (i = 0; i <= 7; i++) GetPic16B(8, 2 + i * 9, 1 + j * 79, 9, 34, Minions + i * 308 + j * 3080);
    for (i = 0; i <= 1; i++) GetPic16B(8, 2 + i * 9, 46 + j * 95, 9, 34, Minions + 2464 + i * 308 + j * 3080);
    GetPic16B(8, 38, 135 + j * 37, 4, 3, Steps + j * 14);
  }

  Box16B(6, 0, 0, 67, 18, 0);
  Box16B(8, 2, 0, 80, 175, 0);
  NickAddress = (VideoSeg3 - 0xFC) * 16384;
  Set_LPT_Address(NickAddress);
}

void Header(unsigned char Type)
// Type: 0=menu, 1/2=game, 3=Exit?, 4=Get ready!, 5=Game over, 6=Phil won, 7=Stuart won
{
  if (Type > 1) Box16B(4, 0, 0, 65, 9, 8);
  switch (Type) {
    case 2:
      PutPic16B(4, 37, 1, txt_Energy);
    case 1:
      Box16B(4, 0, 0, 37, 9, 8);
      PutPic16B(4, 8, 1, txt_Energy);
      break;
    case 0:
      Get_Address(10, 0);
      Out(0xB2, VideoSeg);
      PutPic16B(4, 0, 0, VideoOfs + 0x8000);
      break;
    case 3:
      PutPic16B(4, 34, 1, txt_Exit);
      break;
    case 4:
      PutPic16B(4, 34, 1, txt_GetReady);
      break;
    case 5:
      PutPic16B(4, 34, 1, txt_GameOver);
      break;
    case 6:
      PutPic16B(4, 35, 1, txt_PhilWon);
      break;
    case 7:
      PutPic16B(4, 32, 1, txt_StuartWon);
  }
  Out(0xB2, DataSeg1);
}

void SwitchMusic(unsigned char MusicNumber)
{
  if (MusicNumber == 1) {
    MusMapAddr = (unsigned int) &Music1Map[0];
    MusTuneAddr = (unsigned int) &Music1Tune[0];
    MusMapNum = Peek(MusMapAddr); MusMapPtr = MusMapNum + 1; // Set pointer to the end of the map, this will force to restart the music
    Envelope = 1;
  }
  if (MusicNumber == 2) {
    MusMapAddr = (unsigned int) &Music2Map[0];
    MusTuneAddr = (unsigned int) &Music2Tune[0];
    MusMapNum = Peek(MusMapAddr); MusMapPtr = MusMapNum + 1;
    Envelope = 4;
  }
  if (MusicNumber == 3) {
    MusMapAddr = (unsigned int) &Music3Map[0];
    MusTuneAddr = (unsigned int) &Music3Tune[0];
    MusMapNum = Peek(MusMapAddr); MusMapPtr = MusMapNum + 1;
    Envelope = 4;
  }
  Clear_Sound();
}

void Say_Banana()
{
  unsigned int i, a;
  unsigned char j;

  Clear_Sound();
  Timer_Wait(10);
  Out(0xB4, 170); // Disable interrupts
  Out(0xA7, 24);  // Switch left and right output to D/A converter
  a = Banana_Speach;
  for (i = 0; i < 2500; i++) {
    for (j = 0; j < 7; j++) Out(0xA8, Peek(a));
    Out(0xAC, Peek(a++));
  }
  Out(0xA7, 0);  // Switch left and right output back
  Out(0xB4, 85); // Enable interrupts
  Timer_Wait(10);
}

void SwitchScr(unsigned char NewMode)
{
  unsigned char LPTSeg, DatSeg1, DatSeg2 = 0, InfoScreen, y, i;
  unsigned int LPTStart1, LPTStart2, DatStart1, DatStart2, LPTIndex, DatIndex;

  if (Mode != 255) {
    InfoScreen = (NewMode == 3) || (Mode == 3);
    // One screen is always the menu
    Get_Address(0, 0); LPTSeg = VideoSeg; LPTStart1 = VideoOfs + 880; // 6*9 lines + 1 LPB
    Get_Address(5, 0); DatSeg1 = VideoSeg; DatStart1 = VideoOfs;
    // Is the other screen the info?
    if (InfoScreen) {
      Get_Address(2, 0); LPTStart2 = VideoOfs;
      Get_Address(9, 0); DatSeg2 = VideoSeg; DatStart2 = VideoOfs;
    }
    // ... or the game?
    else {
      Box16B(8, 2, 0, 80, 175, 0);
      Get_Address(1, 0); LPTStart2 = VideoOfs;
    }
    LPTStart1 += 0xC000; LPTStart2 += 0xC000;
    DatStart1 += 0x8000; DatStart2 += 0xC000;
    for (i = 0; i <= 179; i++) {
      if (InfoScreen) y = (i % 10) * 18 + i / 10;
      else y = i < 90 ? i * 2 : i * 2 - 179;
      LPTIndex = y * 16;
      DatIndex = y * 84;
      // Swap the LPB lines
      Out(0xB3, LPTSeg);
      CopyMem(LPTStart1 + LPTIndex, (unsigned int) &Buf[0], 16);
      CopyMem(LPTStart2 + LPTIndex, LPTStart1 + LPTIndex, 16);
      CopyMem((unsigned int) &Buf[0], LPTStart2 + LPTIndex, 16);
      // Swap the data lines if 'Info' screen is used
      if (InfoScreen) {
        Out(0xB2, DatSeg1); Out(0xB3, DatSeg2);
        CopyMem(DatStart1 + DatIndex, (unsigned int) &Buf[0], 84);
        CopyMem(DatStart2 + DatIndex, DatStart1 + DatIndex, 84);
        CopyMem((unsigned int) &Buf[0], DatStart2 + DatIndex, 84);
      }
    }
  }
  Out(0xB2, DataSeg1);
  Mode = NewMode;
}

void Term()
{
  Set_LPT_Address(SaveLPTAddr);
  Free_Segment(VideoSeg3);
  Free_Segment(VideoSeg2);
  Free_Segment(VideoSeg1);
  Free_Segment(DataSeg2);
  Free_Segment(DataSeg1);
  Set_EXOS_Var(7, 0);
  Set_EXOS_Var(8, 0);
  Set_EXOS_Var(10, 3);
}

unsigned char Init()
{
  static unsigned char Keyboard[] = { 9, 'K', 'E', 'Y', 'B', 'O', 'A', 'R', 'D', ':' };
  static unsigned char Video[] = { 6, 'V', 'I', 'D', 'E', 'O', ':' };
  static unsigned char Snd[] = { 6, 'S', 'O', 'U', 'N', 'D', ':' };
  static unsigned char Error[] = { 15, 'N', 'o', 't', ' ', 'e', 'n', 'o', 'u', 'g', 'h', ' ', 'R', 'A', 'M', '.' };

  Out(0xBF, 12);
  Out(0xB3, 255);
  SaveLPTAddr = Peek(0xFFF4) + Peek(0xFFF5) * 256 + 16384;
  if (Get_VideoSegments() == 0) {
    Print(0, (unsigned int) &Error[0]);
    GetKey(1);
    Term();
    return 0;
  }
  Out(0xB2, DataSeg1);

  Set_EXOS_Var(7, 255);  // Click
  Set_EXOS_Var(8, 255);  // Stop
  Set_EXOS_Var(10, 1);   // Key rate
  Set_EXOS_Var(13, 255); // WAIT_SND: do not wait
  Set_EXOS_Var(15, 32);  // BUF_SOUND

  Open_Channel(102, (unsigned int) &Video[0]);
  Open_Channel(105, (unsigned int) &Keyboard[0]);
  Open_Channel(103, (unsigned int) &Snd[0]);
  Set_Envelope();

  Control[0] = 0; Control[1] = 1;
  Energy[0] = 40; Energy[1] = 40;
  return 1;
}

void Draw_Object()
{
  unsigned char i, j;

  Typ = RoomData[ObjPtr];
  ObjX = RoomData[ObjPtr + 1];
  ObjY = RoomData[ObjPtr + 2];
  Atr = RoomData[ObjPtr + 3];
  switch (Typ) {
    case 1: // Floor
      for (j = 0; j < Atr; j++) {
        PutPic16B(8, ObjX + j * 5, ObjY, Floor + FloorColour * 27);
        FloorColour = (FloorColour + 1) % 8;
      }
      break;
    case 2: // Steps
      for (j = 0; j < Atr % 100; j++) {
        if (Atr < 100) PutPic16B(8, ObjX + j, ObjY + (Atr - j - 1) * 3, Steps + 14);
        else PutPic16B(8, ObjX + j, ObjY + j * 3, Steps);
      }
      break;
    case 3: // Ladder
      PutPic16B(8, ObjX, ObjY, Ladder);
      break;
    case 4: // Brick
      for (i = 0; i < Atr % 10; i++)
        for (j = 0; j < Atr / 10; j++) PutPic16B(8, ObjX + j * 4, ObjY + i * 12, Brick);
      break;
    case 5: // Keys
      PutPic16B(8, ObjX, ObjY, Keys + Atr * 124 + SecondPic * 62);
      break;
    case 6: // Worm
      PutPic16B(8, ObjX, ObjY, Worm);
      break;
    case 7: // Plant
      PutPic16B(8, ObjX, ObjY, Plant);
      break;
    case 8: // Door
      PutPic16B(8, ObjX, ObjY, Door);
      if ((Atr % 10) < 5) PutPic16B(8, ObjX + 3, ObjY + 17, SmallKeys + (Atr % 10) * 8);
      else Box16B(8, ObjX + 2, ObjY + 15, 4, 7, 5);
      break;
    case 9: // Banana
      PutPic16B(8, ObjX, ObjY, Objects + SecondPic * 97);
      break;
    case 10: // Heart
      PutPic16B(8, ObjX, ObjY, Objects + 582 + SecondPic * 97);
      break;
    case 11: // Broken heart
      PutPic16B(8, ObjX, ObjY, Objects + 776 + SecondPic * 97);
      break;
    case 12: // Bag
      PutPic16B(8, ObjX, ObjY, Objects + 970 + SecondPic * 97);
      break;
    case 13: // Stars
      PutPic16B(8, ObjX, ObjY, Objects + 1164 + SecondPic * 97);
      break;
    case 14: // Skull
      MonsterType = Typ;
      MinionX[2] = ObjX;
      MinionY[2] = ObjY;
      MonsterDirection = Atr;
      break;
    case 15: // Dog
      MonsterType = Typ;
      MinionX[2] = ObjX;
      MinionY[2] = ObjY;
      MonsterDirection = 2;
      break;
    case 16: // Fart blaster
      MonsterType = Typ;
      MinionX[2] = ObjX;
      MinionY[2] = ObjY;
      MonsterDirection = 1;
      break;
    case 17: // Room number
      Box16(8, ObjX * 2 - 1, ObjY, 9, 9, 8);
      Number(8, ObjX, ObjY + 1, 2, Room + 1);
      break;
    case 18: // Yellow banana peel
      PutPic16B(8, ObjX, ObjY, Objects + 194 + SecondPic * 97);
      break;
    case 19: // Purple banana peel
      PutPic16B(8, ObjX, ObjY, Objects + 388 + SecondPic * 97);
  }
}

void Draw_Room()
{
  static unsigned char Col[] = { 0, 96, 68, 12, 44, 12, 68, 96,      // dark purple
                                 0, 128, 16, 144, 2, 130, 18, 130,   // green
                                 0, 64, 8, 72, 1, 65, 9, 65,         // red
                                 0, 64, 192, 136, 24, 88, 17, 145,   // brown
                                 13, 97, 232, 56, 112, 148, 70, 102, // pink - blue
                                 0, 32, 224, 4, 196, 36, 228, 172,   // blue
                                 0, 192, 24, 216, 3, 195, 27, 195,   // yellow
                                 0, 96, 40, 12, 76, 108, 37, 101,    // purple
                                 0, 132, 20, 148, 6, 176, 48, 128,   // cyan
                                 7, 56, 192, 0, 64, 8, 72, 1,        // gray - dark red
                                 120, 49, 120, 56, 28, 56, 120, 49,  // gray - brown
                                 8, 136, 24, 16, 48, 160, 32, 40 };  // rainbow
  unsigned char i, j, c, Door1Floor, Door2Floor;

  // Background
  Box16B(8, 2, 0, 80, 175, 0);
  Get_Address(0, 0); VideoOfs += 0xC378;
  for (i = 0; i < 175; i++) {
    c = i / 25;
    j = i % 25;
    if ((j == 0) || (j == 4) || (j == 7) || (j == 8) || (j == 10) || (j == 11) || (j == 13) || (j == 14) ||
        (j == 16) || (j == 17) || (j == 18) || (j == 20) || (j == 21) || (j == 22) || (j == 23)) c++;
    c = c + (Room  % 12) * 8; // 12: number of colours
    Poke(VideoOfs, Col[c]);
    VideoOfs += 16;
  }

  MonsterType = 0;
  FloorColour = 0;
  ObjPtr = RoomMap[Room];
  ObjNum = RoomData[ObjPtr++];
  Door1Floor = RoomData[ObjPtr] / 10;
  Door2Floor = RoomData[ObjPtr++] % 10;
  if ((Room > PrevRoom) || !PrevRoom) {
    MinionX[0] = 10;
    MinionY[0] = Door1Floor * 45 + 6;
  }
  else {
    MinionX[0] = 65 - ((Mode == 2) && (Room != 6) && (Room != 8) && (Room != 13) && (Room != 21)) * 9;
    MinionY[0] = Door2Floor * 45 + 6;
  }
  MinionX[1] = 65;
  MinionY[1] = MinionY[0];

  LadderX1 = 0; LadderX2 = 0; LadderX3 = 0;
  for (i = 1; i <= ObjNum; i++) {
    // Ladder: save the first three position for skull-monster
    if (RoomData[ObjPtr] == 3) {
      j = RoomData[ObjPtr + 1] + 2;
      if (!LadderX1) LadderX1 = j;
      else if (!LadderX2) LadderX2 = j;
      else if (!LadderX3) LadderX3 = j;
    }
    // Disable monsters
    if ((RoomData[ObjPtr] >= 14) && (RoomData[ObjPtr] <= 16) && NoMonsters) RoomData[ObjPtr] = 0;
    Draw_Object(); ObjPtr += 4;
  }
}

unsigned char Quit()
{
  unsigned char Result;

  Set_EXOS_Var(10, 3);  // Key rate
  Set_EXOS_Var(11, 30); // Key delay
  Header(3);
  do {
    Next_Sound();
    GetKey(0);
  } while ((Key != 'y') && (Key != 'n'));
  Result = Key == 'y';
  do {
    GetKey(0);
  } while (Key);
  Header(0); // Put the half of the background back in one player mode
  if (Mode) {
    Header(Mode);
    Scores_Energy_Keys(0, 1, 1, 1);
    if (Mode == 2) Scores_Energy_Keys(1, 1, 1, 1);
    Set_EXOS_Var(10, 1);
    Set_EXOS_Var(11, 1);
  }
  return Result;
}

void Delay()
{
  Next_Sound();
  Timer_Wait(1);
}

void BigMinion(unsigned char MinXB, unsigned char MinY, unsigned char Size, unsigned int SprAddr)
{
  unsigned char x, y, c;

  for (y = 0; y <= 33; y++) {
    Next_Sound();
    if ((GameEnd == 4) || (Mode == 2)) Set_EXOS_Var(27, Rnd255());
    for (x = 0; x <= 16; x++) {
      c = Peek(SprAddr + 2 + y * 9 + x / 2);
      c = (x % 2) ? (c & 85) : ((c & 170) / 2);
      c = (c & 1) * 8 + (c & 16) / 4 + (c & 4) / 2 + (c & 64) / 64;
      Box16B(8, MinXB + x * Size / 2, MinY + y * Size, Size / 2, Size, c);
    }
  }
}

void The_End()
{
  unsigned int Ptr[4];
  unsigned char i, j, k, l;

  if (Mode == 1) {
    k = GameEnd == 4;
    Header(k ? 6 : 5);
    Ptr[0] = Minions; Ptr[1] = Minions + 2464 + (GameEnd != 4) * 308;
  }
  else {
    k = 1;
    // Phil won
    if (!Energy[1] || (Energy[0] && Energy[1] && (Points[0] >= Points[1]))) {
      Header(6);
      Ptr[0] = Minions; Ptr[1] = Minions + 2464;
      Ptr[2] = Minions + 3080; Ptr[3] = Minions + 5852;
    }
    // Stuart won
    else {
      Header(7);
      Ptr[0] = Minions + 3080; Ptr[1] = Minions + 5544;
      Ptr[2] = Minions; Ptr[3] = Minions + 2772;
    }
  }

  Timer_Wait(50);
  for (j = 0; j <= 34; j++) {
    Next_Sound();
    for (i = 0; i <= 39; i++) {
      l = k ? (i + j) % 14 + 1 : (i + j) % 3 + 4;
      Box16B(8, 2 + i * 2, j * 5, 2, 5, l);
    }
  }
  GetKey(0);
  SwitchMusic(3);
  i = 0;
  do {
    i = 1 - i;
    BigMinion(24 - (Mode == 2) * 12, 20, 4, Ptr[i]);
    if (Mode == 2) BigMinion(54, 88, 2, Ptr[i + 2]);
    GetKey(0);
  } while (!Key);
  GetKey(0); // Clear keyboard buffer after pressing Esc
}

// Check that every pixels of the minion are in the specified area, X position in bytes
unsigned char Minion_in_area(unsigned char CheckXB, unsigned char CheckY, unsigned char CheckSizeXB, unsigned char CheckSizeY)
{
  return (MinionX[Player] >= CheckXB) &&
         (MinionX[Player] + 9 <= CheckXB + CheckSizeXB) &&
         (MinionY[Player] >= CheckY) &&
         (MinionY[Player] + 34 <= CheckY + CheckSizeY);
}

// Check that any pixel of the minion contacts the specified area, X position in bytes
unsigned char Minion_contacts(unsigned char CheckXB, unsigned char CheckY, unsigned char CheckSizeXB, unsigned char CheckSizeY)
{
  return (MinionX[Player] + 9 > CheckXB) &&
         (MinionX[Player] < CheckXB + CheckSizeXB) &&
         (MinionY[Player] + 34 > CheckY) &&
         (MinionY[Player] < CheckY + CheckSizeY);
}

void Remove_Object()
{
  RoomData[ObjPtr] = 0;
  Box16B(8, ObjX, ObjY, 5, 19, 0);
}

void Swap_Control()
{
  unsigned int i;

  i = Control[0]; Control[0] = Control[1]; Control[1] = i;
}

void Sound_Effect(unsigned char Which)
{
  unsigned char Left, Right;

  Left = (MinionX[Player] < 48) * 255;
  Right = (MinionX[Player] > 21) * 255;
  if (Left && Right) Left = Right = 127;
  switch (Which) {
    case 0: // Banana
      Sound(5, 82, Left, Right, 0, 2, 25, 0); break;
    case 1: // Ta-da
      Sound(6, 74, Left, Right, 0, 2, 30, 0); break;
    case 2: // Noise
      Sound(7, 74, Left, Right, 48, 2, 25, 0); break;
    case 3: // High
      Sound(8, 114, Left, Right, 0, 2, 25, 0); break;
    default: // Bad sound
      Sound(9, 90, Left, Right, 16, 2, 25, 0);
  }
}

void Check_Objects()
{
  unsigned char i, j, DistanceX, DistanceY;

  On_ladder = 0;
  ObjPtr = RoomMap[Room] + 2;
  for (i = 0; i < ObjNum; i++) {
    ObjX = RoomData[ObjPtr + 1];
    ObjY = RoomData[ObjPtr + 2];
    // Speed up the checking: check only if the object is close to the minion
    DistanceX = ObjX > MinionX[Player] ? ObjX - MinionX[Player]: MinionX[Player] - ObjX;
    DistanceY = ObjY > MinionY[Player] ? ObjY - MinionY[Player]: MinionY[Player] - ObjY;
    if (((DistanceX < 10) && (DistanceY < 40)) || (RoomData[ObjPtr] > 17) || (RoomData[ObjPtr] == 4)) {
      Atr = RoomData[ObjPtr + 3];
      switch (RoomData[ObjPtr]) {
        case 3: // Ladder
          if (Minion_in_area(ObjX, ObjY - 34, 9, 79)) On_ladder = 1;
          if ((ControlDirection == 4) || (ControlDirection == 8)) { // Down-up
            if ((ControlDirection == 4) && Minion_in_area(ObjX, ObjY - 34, 9, 76)) MinionDirection = ControlDirection; // Accept move if possible
            if ((ControlDirection == 8) && Minion_in_area(ObjX, ObjY - 31, 9, 76)) MinionDirection = ControlDirection;
            if (Minion_in_area(ObjX + 1, ObjY - 34, 12, 79)) MinionDirection = 2; // Help move left
            if (Minion_in_area(ObjX - 4, ObjY - 34, 12, 79)) MinionDirection = 1; // Help move right
          }
          if ((ControlDirection == 1) || (ControlDirection == 2)) { // Right-left
            if (Minion_contacts(ObjX, ObjY, 9, 5)) MinionDirection = 0; // Prohibit move if impossible
            if (Minion_in_area(ObjX, ObjY - 15, 9, 59)) MinionDirection = 4; // Help move down
            if (Minion_in_area(ObjX, ObjY - 31, 9, 50)) MinionDirection = 8; // Help move up
          }
          // Refresh the ladder if the minion climbed on, this avoids to fall down
          if (Minion_in_area(ObjX, ObjY - 34, 9, 34)) PutPic16B(8, ObjX, ObjY, Ladder);
          break;
        case 4: // Brick
          if (((ControlDirection == 2) && Minion_contacts(ObjX + 1, ObjY, 4 * (Atr / 10), 12 * (Atr % 10))) || // Left
              ((ControlDirection == 1) && Minion_contacts(ObjX - 1, ObjY, 4 * (Atr / 10), 12 * (Atr % 10))))   // Right
            MinionDirection = 0;
          break;
        case 5: // Keys
          if (Minion_contacts(ObjX, ObjY, 5, 12)) {
            Sound_Effect(3);
            KeysHave[Player * 5 + Atr] = 1;
            Scores_Energy_Keys(Player, 0, 0, 1);
            Remove_Object();
          }
          break;
        case 6: // Worm
          if ((ControlDirection == 1) && Minion_contacts(ObjX - 1, ObjY, 8, 32)) {
            if (RoomData[RoomMap[Atr] + 10]) {
              PutPic16B(8, ObjX - 2, ObjY - 17, I_ate_a_plant);
              Sound_Effect(1);
            }
            MinionDirection = 0;
            RoomData[RoomMap[Atr] + 10] = 0; // +10: eatable plant is always the 3rd object in the room
            // Delete plant if it is in the actual room
            if (Room == Atr) Box16B(8, RoomData[RoomMap[Atr] + 11], RoomData[RoomMap[Atr] + 12], 8, 35, 0);
          }
          break;
        case 7: // Plant
          if (((ControlDirection == 2) && Minion_contacts(ObjX + 1, ObjY, 8, 35)) || // Left
              ((ControlDirection == 1) && Minion_contacts(ObjX - 1, ObjY, 8, 35)))   // Right
            MinionDirection = 0;
          break;
        case 8: // Door
          if (((ControlDirection == 2) && Minion_contacts(ObjX + 1, ObjY, 8, 40)) || // Left
              ((ControlDirection == 1) && Minion_contacts(ObjX - 1, ObjY, 8, 40))) { // Right
            MinionDirection = 0;
            // Atr / 10: room number, Atr % 10: key colour (5 = open door)
            j = Atr % 10;
            if (j == 5) {
              Room = Atr / 10; GameEnd = 3;
              // Animate door open
              for (j = 0; j < 4; j++) {
                Sound(255, 100 + j * 6, 127, 127, 32, 2, 5, 0);
                GetPic16B(8, ObjX + 2, ObjY + 8, 3, 32, DoorAnim);
                PutPic16B(8, ObjX + 3, ObjY + 8, DoorAnim);
                Box16B(8, ObjX + 2, ObjY + 8, 1, 32, 0);
                Next_Sound();
                Timer_Wait(5);
              }
            }
            else {
              if (KeysHave[j + Player * 5]) {
                KeysHave[j + Player * 5] = 0;
                RoomData[ObjPtr + 3] = (Atr / 10) * 10 + 5;
                Scores_Energy_Keys(Player, 0, 0, 1);
              }
            }
          }
          break;
        case 9: // Banana
          if (Minion_contacts(ObjX, ObjY, 5, 19)) {
            Sound_Effect(0);
            Points[Player]++; Bananas--;
            Scores_Energy_Keys(Player, 1, 0, 0);
            Remove_Object();
            // All bananas are eaten
            if (!Bananas) GameEnd = 4;
          }
          break;
        case 10: // Heart
          if (Minion_contacts(ObjX, ObjY, 5, 19)) {
            Sound_Effect(1);
            if (Energy[0] < 41) {
              Energy[Player] = (Energy[Player] < 20) ? (Energy[Player] + 20) : 40;
              Scores_Energy_Keys(Player, 0, 1, 0);
            }
            Remove_Object();
          }
          break;
        case 11: // Broken heart
          if (Minion_contacts(ObjX, ObjY, 5, 19)) {
            Sound_Effect(4);
            if (Energy[0] < 41) {
              Energy[Player] = (Energy[Player] > 12) ? (Energy[Player] - 12) : 0;
              Scores_Energy_Keys(Player, 0, 1, 0);
            }
            Remove_Object();
          }
          break;
        case 12: // Bag
          if (Minion_contacts(ObjX, ObjY, 5, 19))
            switch (Rnd255() % 4) {
              case 0: RoomData[ObjPtr] = 10; break;
              case 1: RoomData[ObjPtr] = 11; break;
              case 2: RoomData[ObjPtr] = 13; break;
              case 3: KeysHave[Rnd255() % 5 + Player * 5] = 1; Scores_Energy_Keys(Player, 0, 0, 1);
            }
          break;
        case 13: // Stars
          if (Minion_contacts(ObjX, ObjY, 5, 19)) {
            Sound_Effect(2);
            if ((Mode == 2) && !Swap) Swap_Control();
            Swap = 255;
            Set_EXOS_Var(27, 0);
            Remove_Object();
          }
          break;
        case 18: // Yellow banana peel
        case 19: // Purple banana peel
          // Countdown when player one is checked
          if (!Player) {
            RoomData[ObjPtr + 3] -= 1;
            if (!RoomData[ObjPtr + 3]) Remove_Object();
          }
          // Block the other minion
          if (RoomData[ObjPtr] == 19 - Player)
            if (((ControlDirection == 2) && Minion_contacts(ObjX + 1, ObjY, 6, 19)) || // Left
                ((ControlDirection == 1) && Minion_contacts(ObjX - 1, ObjY, 6, 19)))   // Right
              MinionDirection = 0;
      }
    }
    ObjPtr += 4;
  }
}

void Move_Monster()
{
  unsigned char Banana_peel, i;

  // Check the banana peels
  i = Player; Player = 2; // Use the same subroutine as for the minions
  Banana_peel = 0;
  if (RoomData[RoomMap[Room] + 2]) Banana_peel = Minion_contacts(RoomData[RoomMap[Room] + 3], RoomData[RoomMap[Room] + 4], 5, 19);
  if (RoomData[RoomMap[Room] + 6]) Banana_peel = Banana_peel || Minion_contacts(RoomData[RoomMap[Room] + 7], RoomData[RoomMap[Room] + 8], 5, 19);
  Player = i; // Restore the value to catch real minion below when direction is changed

  // Check that the monster can move in that way
  switch(MonsterType) {
    case 14: // Skull
      // Change direction randomly
      if (!Banana_peel) {
        // Change where there is more ways to choose
        i = Rnd255() % 3;
        if ((MonsterDirection < 3) && ((MinionX[2] == LadderX1) || (MinionX[2] == LadderX2) || (MinionX[2] == LadderX3))) {
          if ((i == 0) || (MinionY[Player] < MinionY[2])) {
            if (MinionY[2] > 19) MonsterDirection = 8; else MonsterDirection = 4;
          }
          else if ((i == 1) || (MinionY[Player] > MinionY[2])) {
            if (MinionY[2] < 154) MonsterDirection = 4; else MonsterDirection = 8;
          }
        }
        else if ((MonsterDirection > 3) && ((MinionY[2] == 19) || (MinionY[2] == 64) || (MinionY[2] == 109) || (MinionY[2] == 154))) {
          if ((i == 0) || (MinionX[Player] < MinionX[2])) {
            if (MinionX[2] > LadderX1) MonsterDirection = 2; else MonsterDirection = 1;
          }
          else if ((i == 1) || (MinionX[Player] > MinionX[2])) {
            if (MinionX[2] < LadderX2) MonsterDirection = 1; else MonsterDirection = 2;
          }
        }
        // Turn back (Crazy Ivan)
        i = Rnd255() % 100;
        if (!i) {
          if (MonsterDirection < 3) MonsterDirection = 3 - MonsterDirection;
          else MonsterDirection = 12 - MonsterDirection;
        }
      }
      // Skull valid floors: 19, 64, 109, 154, valid horizontal range: LadderX1 - LadderX2, possibibility to change: LadderX3
      if ((MonsterDirection == 2) && ((MinionX[2] <= LadderX1) || Banana_peel)) MonsterDirection = 1;
      else if ((MonsterDirection == 1) && ((MinionX[2] >= LadderX2) || Banana_peel)) MonsterDirection = 2;
      else if ((MonsterDirection == 8) && ((MinionY[2] <= 19) || Banana_peel)) MonsterDirection = 4;
      else if ((MonsterDirection == 4) && ((MinionY[2] >= 154) || Banana_peel)) MonsterDirection = 8;
      break;
    case 15: // Dog
      if ((MinionX[2] < 4) || Banana_peel) {
        Box16B(8, MinionX[2], MinionY[2], 5, 19, 0);
        MinionX[2] = 75;
      }
      break;
    case 16: // Fart blaster
      if ((MinionX[2] > 75) || Banana_peel) {
        Box16B(8, MinionX[2], MinionY[2], 5, 19, 0);
        MinionX[2] = 4;
      }
  }

  // Move the monster
  switch(MonsterDirection) {
    case 1: // Right
      Box16B(8, MinionX[2], MinionY[2], 1, 19, 0);
      MinionX[2]++;
      break;
    case 2: // Left
      Box16B(8, MinionX[2] + 4, MinionY[2], 1, 19, 0);
      MinionX[2]--;
      break;
    case 4: // Down
      Box16B(8, MinionX[2], MinionY[2], 5, 3, 0);
      MinionY[2] += 3;
      break;
    case 8: // Up
      Box16B(8, MinionX[2], MinionY[2] + 16, 5, 3, 0);
      MinionY[2] -= 3;
  }
  PutPic16B(8, MinionX[2], MinionY[2], Objects + MonsterType * 194 - 1358 + ((SpriteCounter / 8) % 2) * 97);
}

void Game()
{
  unsigned char i, j, DrawObj = 0;

  //    Mode == 0: menu
  //            1: one player mode
  //            2: two players mode
  //            3: info screen
  //
  // GameEnd == 0: no end
  //            1: Phil died
  //            2: Stuart died
  //            3: next room
  //            4: end of the game, all bananas are eaten
  //            5: exit, Esc is pressed or end of the game

  Room = 0; PrevRoom = Room;
  Points[0] = 0; Points[1] = 0; Bananas = 112; Swap = 0;
  for (i = 0; i < 10; i++) KeysHave[i] = 0;
  if (Energy[0] < 41) {
    Energy[0] = 40; Energy[1] = 40;
  }
  Set_EXOS_Var(11, 1); // Key delay
  CopyMem((unsigned int) &RoomOrig, (unsigned int) &RoomData, 2444);
  Draw_Room();

  // Get ready!
  Header(4);
  for (i = 0; i < 4; i++) {
    for (j = 0; j < Mode; j++) Box16B(8, MinionX[j], MinionY[j], 9, 34, 0);
    if (i) for (j = 0; j < 8; j++) Delay();
    for (j = 0; j < Mode; j++) PutPic16B(8, MinionX[j], MinionY[j], Minions + j * 3080);
    for (j = 0; j < 8; j++) Delay();
  }
  Say_Banana();
  SwitchMusic(2);
  Header(0);
  Header(Mode);

  do {
    for (i = 0; i < Mode; i++) Scores_Energy_Keys(i, 1, 1, 1);
    GameEnd = 0;
    if (Room != PrevRoom) {
      Draw_Room();
      PrevRoom = Room;
      GetKey(0);
    }
    do {
      SpriteCounter = (SpriteCounter + 1) % 128;
      GetKey(0);
      if ((Key == 27) || (Key == 3)) if (Quit()) GameEnd = 5;

      // Re-draw one object
      DrawObj++;
      if (DrawObj >= ObjNum) { DrawObj = 0; SecondPic = 1 - SecondPic; }
      ObjPtr = RoomMap[Room] + 2 + DrawObj * 4;
      Typ = RoomData[ObjPtr];
      if ((Typ == 3) || (Typ == 5) || (Typ == 9) || (Typ == 10) || (Typ == 11) || (Typ == 12) || (Typ == 13) || (Typ == 18) || (Typ == 19))
        Draw_Object();

      // Swap minions
      if (Swap) {
        Swap--;
        if (Swap < 40) Set_EXOS_Var(27, ((Swap / 4) % 2) * 12);
        if (!Swap && (Mode == 2)) Swap_Control();
      }

      // Move the monster
      if (MonsterType) Move_Monster();

      // Handle the players
      for (Player = 0; (Player < Mode) && (GameEnd == 0); Player++)
        if (!GameEnd) {
          // Check the keyboard
          ControlDirection = 0; Fire = 0;
          if (((Control[Player] != 3) && (Joy[Control[Player]] & 1)) || ((Control[Player] == 3) && (Key_Matrix1 == 251)))
            ControlDirection = 1; // Right
          if (((Control[Player] != 3) && (Joy[Control[Player]] & 2)) || ((Control[Player] == 3) && (Key_Matrix1 == 247)))
            ControlDirection = 2; // Left
          if (((Control[Player] != 3) && (Joy[Control[Player]] & 4)) || ((Control[Player] == 3) && (Key_Matrix1 == 239)))
            ControlDirection = 4; // Down
          if (((Control[Player] != 3) && (Joy[Control[Player]] & 8)) || ((Control[Player] == 3) && (Key_Matrix2 == 247)))
            ControlDirection = 8; // Up
          if (((Control[Player] != 3) && (Joy[Control[Player]] & 16)) || ((Control[Player] == 3) && (Key_Matrix2 == 253)))
            Fire = 1; // Fire

          // Check that the minion can move in that way
          if (((ControlDirection == 1) && (MinionX[Player] > 72)) ||
              ((ControlDirection == 2) && (MinionX[Player] < 3)) ||
              ((ControlDirection == 4) && (MinionY[Player] > 140)) ||
              ((ControlDirection == 8) && (MinionY[Player] < 7))) ControlDirection = 0;
          MinionDirection = ControlDirection;

          // Do not allow up-down direction, allow only later if the ladder is used
          if ((MinionDirection == 4) || (MinionDirection == 8)) MinionDirection = 0;

          // Steps (direction down is handled by the gravity)
          On_steps = (((ControlDirection == 2) && (GetPixel16(8, MinionX[Player] * 2 - 1, MinionY[Player] + 31) == 21)) || // Left
                     ((ControlDirection == 1) && (GetPixel16(8, MinionX[Player] * 2 + 18, MinionY[Player] + 31) == 42)));  // Right
          if (On_steps) {
            Box16B(8, MinionX[Player], MinionY[Player] + 31, 9, 3, 0);
            MinionY[Player] -= 3;
          }

          // Check objects which can block the minion or minion can catch it
          Check_Objects();

          // Check collision with the monster
          if (MonsterType)
            if (Minion_contacts(MinionX[2], MinionY[2], 5, 19)) {
              if (Energy[Player] > 0) {
                Sound(255, 104, 255, 255, 32, 2, 2, 0);
                if (Energy[0] < 41) {
                  Energy[Player]--;
                  if ((MonsterType == 16) && (Energy[Player])) Energy[Player]--;
                  if ((MonsterType == 14) && (Energy[Player] > 1)) Energy[Player] -= 2;
                }
                Scores_Energy_Keys(Player, 0, 1, 0);
              }
            }      
          if (!Energy[Player]) GameEnd = Player + 1;

          // Gravity (42 and 21 are colour 14 on the high and low part of the byte)
          Falling = 0;
          if (!On_ladder && !On_steps &&
              (GetPixel16(8, (MinionX[Player] - (MinionDirection == 2)) * 2, MinionY[Player] + 34) != 42) &&
              (GetPixel16(8, (MinionX[Player] + (MinionDirection == 1)) * 2 + 17, MinionY[Player] + 34) != 21)) {
            Box16B(8, MinionX[Player], MinionY[Player], 9, 3, 0);
            MinionY[Player] += 3;
            Falling = 1; // Force display of the minion
            // Do not allow to move if minion is blocked (cyan colour is found)
            if ((MinionDirection == 1) || (MinionDirection == 2)) {
              j = MinionX[Player] * 2 - 2 + (MinionDirection == 1) * 20;
              for (i = 0; (i <= 36) && MinionDirection; i += 3) if (GetPixel16(8, j, MinionY[Player] + 34 - i) == 42) MinionDirection = 0;
            }
          }

          // Move and display the appropriate minion sprite
          switch (MinionDirection) {
            case 1: // Right
              SpritePtr = Minions + 1232;
              Box16B(8, MinionX[Player], MinionY[Player], 1, 34, 0);
              MinionX[Player]++;
              break;
            case 2: // Left
              SpritePtr = Minions + 616;
              Box16B(8, MinionX[Player] + 8, MinionY[Player], 1, 34, 0);
              MinionX[Player]--;
              break;
            case 4: // Climb down
              SpritePtr = Minions + 1848;
              Box16B(8, MinionX[Player], MinionY[Player], 9, 3, 0);
              MinionY[Player] += 3;
              break;
            case 8: // Climb up
              SpritePtr = Minions + 1848;
              Box16B(8, MinionX[Player], MinionY[Player] + 31, 9, 3, 0);
              MinionY[Player] -= 3;
              break;
            default: // Stand, gravity
              SpritePtr = Minions;
          }
          if (MinionDirection || ((SpriteCounter % 8) == 0) || Falling) {
            if ((SpriteCounter == Player * 64 + 32) && !MinionDirection) SpritePtr = Minions + 2464;
            else if ((SpriteCounter / 8) % 2) SpritePtr += 308;
            if (Player || ((Mode == 1) && Swap)) SpritePtr += 3080;
            PutPic16B(8, MinionX[Player], MinionY[Player], SpritePtr);
          }

          // Drop banana peel
          if (Fire && !Falling && !On_ladder && Points[Player] && !RoomData[RoomMap[Room] + Player * 4 + 2]) {
            RoomData[RoomMap[Room] + Player * 4 + 2] = 18 + Player;
            RoomData[RoomMap[Room] + Player * 4 + 3] = MinionX[Player] + 2;
            RoomData[RoomMap[Room] + Player * 4 + 4] = MinionY[Player] + 15;
            RoomData[RoomMap[Room] + Player * 4 + 5] = 255;
            Points[Player]--;
            Scores_Energy_Keys(Player, 1, 0, 0);
            DrawObj = 12; // This sets the pointer around the half of the object number (not zero because the minion covers it), this forces to display the banana peel quickly
          }
        }
      Next_Sound();
      if (Mode == 1) Timer_Wait(1);
    } while (!GameEnd);

  } while (GameEnd == 3);
  if (Swap && (Mode == 2)) Swap_Control();

  if (GameEnd != 5) The_End();
  Set_EXOS_Var(11, 30);
  Set_EXOS_Var(27, 0);
  Header(0);
  SwitchMusic(1);
}

void Menu()
{
  unsigned char i, j, k, l;

  Timer_Init();
  SwitchScr(0);
  SwitchMusic(1);
  do {
    Set_EXOS_Var(27, 0);
    Out(0xB2, DataSeg1);
    if (!Mode) {
      for (i = 0; i <= 1; i++) PutPic16B(6, 8 + i * 53, 0, ControlIcons + Control[i] * 272);
      if (NoMonsters) PutPic16B(6, 38, 0, NoMonstIcon);
      else Box16B(6, 38, 0, 7, 18, 0);
    }
    Get_Address(0, 0);
    do {
      // Scroll the colours of the menu text
      if (!Mode) {
        k = Peek(95 * 16 + 8 + Peek(MenuColours) + 0xC000);
        i = 0;
        do {
          j = i + 1;
          while ((j < 89) && (Peek(MenuColours + j) == 255)) j++;
          l = j < 89 ? Peek((95 + j) * 16 + 8 + Peek(MenuColours + j) + 0xC000) : k;
          Poke((95 + i) * 16 + 8 + Peek(MenuColours + i) + 0xC000, l);
          i = j;
        } while (i < 89);
      }
      Delay(); Delay();
      GetKey(0);
    } while (!Key);
    if (!Mode) {
      if (Key == '1') {
        SwitchScr(1);
        Game();
        SwitchScr(0);
      }
      if (Key == '2') {
        SwitchScr(2);
        Game();
        SwitchScr(0);
      }
      if (Key == '3') {
        do {
          Control[0] = (Control[0] + 1) % 4;
        } while (Control[0] == Control[1]);
      }
      if (Key == '4') {
        do {
          Control[1] = (Control[1] + 1) % 4;
        } while (Control[1] == Control[0]);
      }
    }
    if ((Key == '5') || Mode) SwitchScr(Mode ? 0 : 3);
    if (Key == '6') NoMonsters = !NoMonsters;
    if ((Key == 27) || (Key == 3)) Key = Quit() ? 27 : 0;
    if ((Key == 'e') && (Mode == 0)) {
      Set_EXOS_Var(27, 219);
      Energy[0] = 41; Energy[1] = 41;
      Delay();
      Set_EXOS_Var(27, 0);
    }
  } while (Key != 27);
}

void main()
{
  if (Init()) {
    Load_Pic();
    Menu();
    Term();
  }
}

// -----------------------------------------------------------
//
// Screen parts:
//
// LPT1:       28*9*16 =  4144 (28*9 + 7 sync lines)
// LPT2:       20*9*16 =  2880
// LPT3:       20*9*16 =  2880
// Header:      5*9*84 =  3780
// Head+score:  1*9*84 =   756
// Menu attr:  18*9*84 = 13608
// Menu 16:     2*9*84 =  1512
// Footer:      2*9*84 =  1512
// Info:       20*9*84 = 15120
// Game track: 20*9*84 = 15120
//                       -----
//                       61312 bytes
//
// -----------------------------------------------------------
//
// Memory map:
//
// VideoSeg1: 0..15119: Game track
// (252)  15120..15706: Head+score save (65*9+2=587)
// VideoSeg2: 0..13607: Menu attr
// (253)  13608..15119: Menu 16
//        15120..15875: Head+score
// VideoSeg3: 0.. 4143: LPT1 (menu)
// (254)   4144.. 7023: LPT2 (game)
//         7024.. 9903: LPT3 (info, to swap)
//         9904..13683: Header
//        13684..15195: Footer
// DataSeg1:  0..xxxxx: Images, WAV
// DataSeg2:  0..15119: Info (swap Info and Menu screens)
// (Segment 255 is not used.)
//
// -----------------------------------------------------------
//
// .EPI structure, blocks and memory addresses:
//
// ID   Block#  Part    Lines    Variable    Seg/Ofs      Size
// -----------------------------------------------------------
// 12 bytes .EPI header
//  0   -       LPT1 (menu)      VideoSeg3   254/0        4144
//  1   -       LPT2 (game)      VideoSeg3   254/4144     2880
//  2   -       LPT3 (info)      VideoSeg3   254/7024     2880
//  3   3       Header           VideoSeg3   254/9904     3780
//  4   4       Head+score       VideoSeg2   253/15120     756
//  5   5       Menu attr        VideoSeg2   253/0       13608
//  6   6       Menu 16          VideoSeg2   253/13608    1512
//  7   7       Footer           VideoSeg3   254/13684    1512
// Skip 12 bytes .EPI header
//  8   8       Game track       VideoSeg1   252/0       15120
// Skip 12 bytes .EPI header
//  9   9       Info             DataSeg2    ???/0       15120
// 10   -       Head+score save  VideoSeg1   252/15120     587
//
// -----------------------------------------------------------
//
// Compile: c:\z88dk\bin\zcc +enterprise -create-app -compiler sdcc Banana2.c -o Banana2.com
//
// copy /b BANSOUND.DAT + Image1.epi + Image2.epi + Image3.epi + Image4.epi Banana2.epi
