/*** INCLUDES ***/

#include <TFT_eSPI.h>       // Graphics and font library for ILI9341 driver chip
TFT_eSPI tft = TFT_eSPI();  // Invoke Display library

#include <Seeed_Arduino_FS.h>

/*** DEFINES ***/

// DEBUG:

//#define DEBUGIO

// PROGRAM:

//#define Dscb_mode
//#define Dscb_rand

#define scb_uint8 unsigned char
#define scb_bool _Bool
#define scb_ProgramExit 9
#define scb_TftMaxLines 15
#define scb_TftMaxLineChars 27

#define scb_ByteValueMax 255
#define scb_ByteValueMaxP1 256
#define scb_unsBlockSize 256
#define scb_unsBlockSizeMask (scb_unsBlockSize - 1)
#define scb_sBlockSize (4*scb_unsBlockSize)
#define scb_sBlockSizeMask (scb_sBlockSize - 1)
#define scb_IndexWordSize 256
#define scb_WordIndexMax (scb_IndexWordSize - 1)

//FILE:

//#define Dscbf_processFiles

#define scbf_FileEnter 40
#define scbf_FileSkip 41
#define scbf_FileExit 49

//OVERWRITE

//#define Dscbo_overwiteFile

// SWITHCH:

//#define Dscb5_5WSwitch

#define scb5_5WayDelay 150  // 0.15 second debounce.

// EDITOR:

//#define Dscbe_CharChange
//#define Dscbe_LinePress

#define scbe_ModeIndexEnter 10
#define scbe_ModeIndex 11
#define scbe_ModeChar 21
#define scbe_ModeLine 31

#define scbe_incrementX 12
#define scbe_incrementY 16
#define scbe_maxX (26 * scbe_incrementX)

#define scbe_CharVMax 3
#define scbe_CharHMax 25
#define scbe_BlankChr 248
#define scbe_InsertChr 175
#define scbe_BackspaceChr 174
#define scbe_NewLineChr 165

//DIGIT10:

//#define Dscbd_KeyChange;

#define scbd_ModeDigit10Enter 60
#define scbd_ModeDigit10 61
#define scbd_ModeDigit10M 62
#define scbd_ModeDigit10E 63
#define scbd_ModeDigit10Q 64
#define scb_digit10Min 1000                       // At least 4 digits!
#define scb_digit10MaxD10 ((LONG_MAX / 10) - 10)  // Pervent overflow-error!
#define scbd_KcSize 4
#define scbd_KxStart 94
#define scbd_KyStart 40
#define scbd_DigitsxStart 72
#define scbd_Line2Start 40
#define scbd_KxInc 32
#define scbd_KyInc 40

//SCRAMBLE/UNSCRAMBLE

//#define DtestRand
//#define DCheckSCBFiles
//#define Doutput256Randoms
//#define DoutputscbBytes
//#define Dscbs_shuffleBytes
//#define Dsave_randoms
//#define Dscbs_rand
//#define Dscbs_xorUnsBlock
//#define Dscbs_clearHistograms()
//#define Dscbs_unsBlockHistogram
//#define Dscbs_randomFillSBlock
//#define Dscbs_sort
//#define Dscbs_selectUnsToSValuesUsingHistograms
//#define Dscbs_randomLoadSBlock
//#define Dscbs_scrambleBlock
//#define Dscbs_unloadSBlock
//#define Dscbs_unscrambleBlock
//#define Dscbs_overwiteFile
//#define Dscbs_scrambleFile
//#define Dscbs_unscrambleFile
//#define Dscbs_Mutate

#define scbs_ModeScramble 70

#define scbs_ByteValueMax 255
#define scbs_ByteValueMaxP1 256
#define scbs_sBlockUsedBitsSize (scb_sBlockSize >> 4)  // 16 bits/byte
#define scbs_sBlockSeparationSizeMin 0
#define scbs_sBlockSeparationSizeMax 31

#define scbs_dummyModulo 0x7FFFFFFF

/*** GLOBALS ***/

// DEBUG:
#ifdef DEBUGIO
char debug_output[81] = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
#endif

// PROGRAM:

int scb_mode;
int scb_newMode;
unsigned long scb_digit10 = 0;  // max: 4,294,967,295

File scb_inDir;
File scb_inFile;
File scb_outFile;
char scb_tempName[80];

unsigned int scb_randSeed;
scb_uint8 scb_unsBlock[scb_unsBlockSize+1];             // position-index.
scb_uint8 scb_sBlock[scb_sBlockSize];                 // position-index.

// Make program unique by changing code at "PICK ONE!" locations.
// Make mutating-SDCARD unique by changing SDCARD file ".scbs_Bytes" with "DoutputscbBytes".
// Make mutating-code unique by changing these with data from "Doutput256Randoms".
const unsigned int scbs_WordDef[scb_IndexWordSize] = {
  1959343845, 952892145, 980800944, 994524332, 
  1984964630, 1914432163, 1616832001, 549763289, 
  1710265904, 2067177542, 1451314741, 1124982376, 
  1623119224, 1832759079, 1340589880, 137314985, 
  282945380, 1665428353, 1103282115, 1743850082, 
  1870580421, 484787007, 454682351, 2081538168, 
  1096464834, 202771335, 1473821326, 477633360, 
  277226891, 261835511, 811999545, 1199293682, 
  1633571785, 425366700, 609678375, 368397810, 
  1671875031, 8114183, 574329027, 2077545230, 
  1615502770, 1069126958, 174612987, 340780438, 
  572538032, 1602249298, 1318680920, 968249998, 
  461422578, 1283262901, 1828789202, 710802581, 
  121748122, 1500113002, 124866144, 2015732867, 
  1175619004, 767251930, 1457548881, 93238483, 
  937174549, 1603980297, 979939677, 1292771270, 
  1354003324, 2051103901, 578999673, 908333123, 
  714349083, 809558681, 414941292, 889909632, 
  1376761181, 2042388716, 573832997, 2000487777, 
  266591397, 1584357806, 1067300042, 1535201699, 
  1849962948, 742663715, 1204864400, 1988065207, 
  971860263, 1503201158, 1607306573, 1654285166, 
  1064989650, 1394920583, 1294547405, 476200025,
  77359888, 1986655120, 1709532391, 723332589, 
  1030077926, 2013853130, 1536333389, 1414997608, 
  1033384011, 1978774242, 577623334, 482915446, 
  632683544, 1973659018, 880362845, 1413799826, 
  206950336, 239946945, 1901044918, 1845355309, 
  1669978815, 2133483986, 1488119717, 1691421840, 
  1509554131, 1911633693, 1651884509, 1808373634, 
  2013307117, 684657560, 823150250, 700076456, 
  962269412, 1485000466, 1635734782, 1110351022, 
  1642876654, 1866071869, 906977101, 1762798377, 
  1329806286, 102032745, 1571927832, 1134541881, 
  96704716, 1075053840, 398284873, 1407652337, 
  973223016, 1404880661, 1917476932, 833199735, 
  367254734, 27286986, 1714985145, 1309567978, 
  2044689415, 921770422, 234951226, 73195270, 
  2051223028, 454471443, 960670352, 904825992, 
  1487683578, 1247904793, 463054540, 848825682, 
  951208572, 2084377596, 1479689503, 752402765, 
  1376078860, 1428233910, 691419753, 2046929169, 
  1558095456, 628234628, 1651857299, 1588805190, 
  2071277830, 1394631728, 286854812, 654452423, 
  1606703574, 809940756, 1993539444, 1249060491, 
  1814439722, 2099727833, 554079371, 1555945539, 
  279986127, 1454922498, 1546488615, 158765250, 
  618926265, 1352611884, 1119319928, 1560221984, 
  2085736569, 2072697618, 685010028, 403606429, 
  2021559406, 396163921, 593135427, 1349526342, 
  1431126973, 844268783, 1797677793, 490275416, 
  1932189697, 1899681693, 1546045158, 244653926, 
  1540190145, 1200493238, 1359165053, 850060604, 
  1128662693, 1493334542, 436039415, 1701195137, 
  1171648359, 1208004460, 1346241046, 754582046, 
  547792778, 797378771, 1163649066, 1641636194, 
  658832588, 163312516, 1312751068, 1666655457, 
  889133148, 1107238850, 716027340, 395098911, 
  1504022474, 859433306, 1212518876, 2009597936, 
  60778433, 1380887777, 571387977, 1758471068, 
  1681996919, 1138167226, 2068471934, 521224388, 
  1372014559, 520172766, 927777703, 1324316936, 
  1679973795, 463226970, 201405703, 1757444196, 
  241705172, 1809029014, 1376391689, 564955997
};
unsigned int scb_Word[scb_IndexWordSize];

// Make mutating-code unique by changing these with data from "Doutput256Randoms".
const scb_uint8 scb_WordIndexDef[scb_IndexWordSize] = {
  27, 132, 180, 86, 45, 44, 233, 185, 140, 232, 198, 215, 184, 255, 65, 49, 
  168, 161, 41, 126, 82, 172, 31, 253, 2, 254, 70, 81, 116, 155, 18, 23, 
  170, 100, 147, 217, 61, 241, 182, 17, 89, 153, 234, 248, 10, 193, 6, 192, 
  98, 181, 47, 40, 117, 93, 121, 122, 104, 106, 71, 151, 4, 223, 160, 29, 
  247, 20, 105, 72, 143, 228, 94, 250, 36, 152, 236, 129, 77, 39, 145, 190, 
  177, 238, 119, 149, 107, 92, 60, 173, 231, 58, 154, 1, 30, 68, 0, 163, 
  207, 183, 96, 34, 203, 230, 67, 159, 206, 128, 109, 63, 24, 130, 158, 73, 
  8, 110, 169, 103, 48, 54, 220, 175, 59, 35, 218, 142, 46, 213, 225, 74, 
  124, 78, 249, 191, 127, 13, 84, 200, 221, 224, 120, 52, 111, 216, 243, 37, 
  80, 237, 5, 205, 165, 66, 38, 108, 239, 99, 79, 75, 212, 87, 240, 252, 
  32, 26, 222, 144, 83, 56, 115, 131, 204, 50, 88, 186, 194, 188, 164, 12, 
  15, 138, 135, 179, 64, 202, 196, 139, 197, 244, 251, 242, 199, 118, 235, 219, 
  171, 113, 97, 112, 22, 136, 33, 178, 150, 156, 114, 227, 9, 53, 229, 101, 
  21, 148, 25, 167, 214, 11, 141, 245, 195, 76, 69, 102, 174, 57, 51, 16, 
  3, 43, 187, 226, 201, 7, 62, 210, 208, 166, 133, 42, 134, 19, 211, 14, 
  146, 85, 55, 95, 176, 91, 123, 157, 246, 137, 125, 90, 209, 189, 28, 162
};
scb_uint8 scb_WordIndex[scb_IndexWordSize];

int scb_current_WordIndex = 0;


//FILE:

bool scbf_siFiles;  // Else uoFiles!
int scbf_NFCount = 1;

//OVERWRITE

int scbo_blockCount = 0;

// SWITCH:

int scb5_verticalCount = 0;
int scb5_verticalCountMax = 0;
int scb5_horizontalCount = 0;
int scb5_horizontalCountMax = 0;
int scb5_pressCount = 0;
int scb3_key3CCount = 0;
int scb5_selectionV = 0;
int scb5_selectionH = 0;
int scb5_selectionVPrior = 0;
int scb5_selectionHPrior = 0;

// EDITOR:

int scbe_currentIndex = 0;
int scbe_currentX = 0;
int scbe_currentY = 0;
char scbe_currentChar = ' ';
char scbe_sellectedChar = ' ';


const char scbe_char[(scbe_CharVMax + 1)][(scbe_CharHMax + 1)] = {
  { scbe_InsertChr, scbe_BackspaceChr, scbe_NewLineChr, scbe_BlankChr,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '"', '\'', '-', '?', ':', '!', ';', 0, 0, 0 },
  { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' },
  { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' },
  { '&', '_', '`', '~', '@', '#', '$', '%', '(', ')', '=', '+', '-', '*', '/', '^', '<', '>', '[', ']', '{', '}', '\\', '|', 0, 0 }
};


//DIGIT10:


//SCRAMBLE/UNSCRAMBLE


union scbs_Integer {
  scb_uint8 bytes[4];
  uint32_t integer;
};
union scbs_Integer scbs_fileSize;
bool scbs_firstBlock;
int scbs_blockCount = 0;

scb_uint8 scbs_sBlockSeparationSize;
scb_uint8 scbs_unsBlockHistogram[scbs_ByteValueMaxP1];  // value-index.
scb_uint8 scbs_sBlockHistogram[scbs_ByteValueMaxP1];    // value-index.
scb_uint8 scbs_unsBlockValues[scbs_ByteValueMaxP1];     // value-index.
scb_uint8 scbs_sBlockValues[scbs_ByteValueMaxP1];       // value-index.
int scbs_sBlockUsedBits[scbs_sBlockUsedBitsSize];         // position-index-by-bit.
scb_uint8 scbs_translationValues[scbs_ByteValueMaxP1];  // uns-to-s-value-index & s-to-s-value-index.

const int scbs_SetBitMask[16] = { 0X0001, 0X0002, 0X0004, 0X0008,
                                  0X0010, 0X0020, 0X0040, 0X0080,
                                  0X0100, 0X0200, 0X0400, 0X0800,
                                  0X1000, 0X2000, 0X4000, 0X8000 };

int scbs_current_WordIndex = 0;

unsigned int scbs_xorSeed;
unsigned int scbs_expandSeed;
unsigned int scbs_loadSeed;
unsigned int scbs_sepSeed;
int scbs_xor_WordIndex;
int scbs_expand_WordIndex;
int scbs_load_WordIndex;
int scbs_sep_WordIndex;

/*** BEGIN GLOBAL CODE ...***/

/* Reentrant random function from POSIX.1c.
	 Copyright (C) 1996-2020 Free Software Foundation, Inc.
	 This file is part of the GNU C Library.
	 Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

	 The GNU C Library is free software; you can redistribute it and/or
	 modify it under the terms of the GNU Lesser General Public
	 License as published by the Free Software Foundation; either
	 version 2.1 of the License, or (at your option) any later version.

	 The GNU C Library is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
	 Lesser General Public License for more details.

	 You should have received a copy of the GNU Lesser General Public
	 License along with the GNU C Library; if not, see
	 <https://www.gnu.org/licenses/>.	 */
/* This algorithm is mentioned in the ISO C standard, here extended
	 for 32 bits.	 */
/* Modified by RTC for scb_rand()!	*/
int scb_rand(const unsigned int mask) {
  unsigned int next = scb_randSeed;
  int result;

  //RTC Begin Mutation:
  next ^= (unsigned int ) scb_Word[scb_WordIndex[scb_current_WordIndex]];  //index];
  --scb_current_WordIndex;
  if (scb_current_WordIndex < 0) { scb_current_WordIndex = scb_WordIndexMax; }
#ifdef DEBUGIO
#ifdef Dscb_rand
  Serial.println("scb_rand():");
  strcpy(debug_output, "index, scb_current_WordIndex =:");
  itoa(index, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scb_current_WordIndex, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  //RTC End Mutation:

  next *= 1103515245;
  next += 12345;
//RTC  result = (unsigned int) (next / 65536) % 2048;
  result = (unsigned int) (next >> 16) & 0x0007FF;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
//RTC  result ^= (unsigned int) (next / 65536) % 1024;
  result ^= (unsigned int) (next >> 16) & 0x0003FF;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
//RTC  result ^= (unsigned int) (next / 65536) % 1024;
  result ^= (unsigned int) (next >> 16) & 0x0003FF;

  scb_randSeed = next;

 //RTC return result;
  return (result &= mask);  // RTC
}

/*** ... END SGLOBAL CODE ***/

/*** BEGIN SCB SWITCH CODE ...***/

void scb5_Reset() {

  scb5_verticalCount = 0;
  scb5_horizontalCount = 0;
  scb5_pressCount = 0;
  scb3_key3CCount = 0;
  scb5_selectionV = 0;
  scb5_selectionH = 0;

}

void scb5_run5WaySwitch() {  // Also includes 3-keys

  scb_mode = scb_newMode;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "run5WaySwitch A: scb_mode, scb_newMode = ");
  itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  while (scb_newMode == scb_mode) {

    if (digitalRead(WIO_5S_UP) == LOW) {
      scb5_verticalCount--;  // Display rowtated 180.
    } else if (digitalRead(WIO_5S_DOWN) == LOW) {
      scb5_verticalCount++;  // Display rowtated 180.
    } else if (digitalRead(WIO_5S_LEFT) == LOW) {
      scb5_horizontalCount--;
    } else if (digitalRead(WIO_5S_RIGHT) == LOW) {
      scb5_horizontalCount++;
    } else if (digitalRead(WIO_5S_PRESS) == LOW) {
      scb5_pressCount++;
    } else if (digitalRead(WIO_KEY_C) == LOW) {
      scb3_key3CCount++;
    }
    delay(scb5_5WayDelay);

    if (scb5_verticalCount < 0) {
      if (scb_mode == scbe_ModeIndex) {
        scb5_selectionH -= 10;
        if (scb5_selectionH < 0) { scb5_selectionH = 0; }
      } else if (scb_mode == scbe_ModeChar) {
        if (scb5_selectionV > 0) { scb5_selectionV--; }
        while (scbe_char[scb5_selectionV][scb5_selectionH] == 0) { scb5_selectionH--; }
      } else if (scb_mode == scbe_ModeLine) {
        if (scb5_selectionV > 1) { scb5_selectionV--; }
      } else {
        if (scb5_selectionV > 0) { scb5_selectionV--; }
      }
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb5_verticalCount, scb5_selectionH, scb5_selectionV  = ");
      itoa(scb5_verticalCount, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      scb5_verticalCount = 0;
    }

    if (scb5_verticalCount > 0) {
      if (scb_mode == scbe_ModeIndex) {
        scb5_selectionH += 10;
        if (scb5_selectionH > scb5_horizontalCountMax) { scb5_selectionH = scb5_horizontalCountMax; }
      } else if (scb_mode == scbe_ModeChar) {
        if (scb5_selectionV < scb5_verticalCountMax) { scb5_selectionV++; }
        while (scbe_char[scb5_selectionV][scb5_selectionH] == 0) { scb5_selectionH--; }
      } else {
        if (scb5_selectionV < scb5_verticalCountMax) { scb5_selectionV++; }
      }
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb5_verticalCount, scb5_selectionH, scb5_selectionV  = ");
      itoa(scb5_verticalCount, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      scb5_verticalCount = 0;
    }

    if (scb5_horizontalCount < 0) {
      if (scb5_selectionH > 0) { scb5_selectionH--; };
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb5_horizontalCount, scb5_selectionH, scb5_selectionV  = ");
      itoa(scb5_horizontalCount, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      scb5_horizontalCount = 0;
    }

    if (scb5_horizontalCount > 0) {
      if (scb5_selectionH < scb5_horizontalCountMax) { scb5_selectionH++; }
      if (scb_mode == scbe_ModeChar) {
        while (scbe_char[scb5_selectionV][scb5_selectionH] == 0) { scb5_selectionH--; }
      }
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb5_horizontalCount, scb5_selectionH, scb5_selectionV  = ");
      itoa(scb5_horizontalCount, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
      strcat(debug_output, " , ");
      itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      scb5_horizontalCount = 0;
    }

    if (scb5_pressCount > 0) {
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb5_pressCount = ");
      itoa(scb5_pressCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif

#ifdef DEBUGIO
#ifdef Dscb_mode
      strcpy(debug_output, "run5WaySwitch Press: scb_mode = ");
      itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      switch (scb_mode) {

        case scbe_ModeLine:
          scbe_LinePress();
          break;

        case scbe_ModeIndex:
          scbe_IndexPress();
          break;

        case scbe_ModeChar:
          scbe_CharPress();
          break;

        case scbd_ModeDigit10:
          if (scbd_pressKeyAction()) {
            return;  // Exit scb5_run5WaySwitch()!
          }
          break;

#ifdef DEBUGIO
        default:
          strcpy(debug_output, "run5WaySwitch B: scb_mode, scb_newMode = ");
          itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
          strcat(debug_output, " , ");
          itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
          Serial.println(debug_output);
          Serial.println("scb_mode mode error 1!");
          while(1){}
#endif
      }


      scb5_horizontalCount = 0;
      scb5_verticalCount = 0;
      scb5_pressCount = 0;
      delay(scb5_5WayDelay);

    }
    if (scb3_key3CCount > 0) {
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
      strcpy(debug_output, "scb3_key3CCount = ");
      itoa(scb3_key3CCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif

#ifdef DEBUGIO
#ifdef Dscb_mode
      strcpy(debug_output, "run5WaySwitch key3C: scb_mode = ");
      itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      switch (scb_mode) {

        case scbe_ModeIndex:
          scbe_Indexkey3CPress();
          break;
      }

      scb3_key3CCount = 0;
      delay(scb5_5WayDelay);

    }

    switch (scb_mode) {

      case scbe_ModeIndex:
        if (scb5_selectionH != scb5_selectionHPrior) {
          scbe_IndexChange();
        }
        break;

      case scbe_ModeChar:
        if ((scb5_selectionV != scb5_selectionVPrior) | (scb5_selectionH != scb5_selectionHPrior)) {
          scbe_CharChange();
        }
        break;

      case scbe_ModeLine:
        if (scb5_selectionV != scb5_selectionVPrior) {
          scbe_LineChange();
        }
        break;

      case scbd_ModeDigit10:
        if ((scb5_selectionV != scb5_selectionVPrior) | (scb5_selectionH != scb5_selectionHPrior)) {
          scbd_KeyChange();
        }
        break;

      default:
        break;

    }
  }

}

/*** ... END SCB SWITCH CODE ***/

/*** BEGIN SCB FILE CODE ...***/

void scbf_processFiles() {
  int i;

#ifdef DEBUGIO
#ifdef Dscbf_processFiles
  Serial.println("Edit/View ScrambleIn/UnScrambleOut files:");
#endif
#endif
  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);
  tft.setTextSize(2); // 16x8 non-mono-spaced font. Numbers are all the same size!
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  if (scbf_siFiles) {
    tft.println("   Edit a file:   ");
    scb_inDir = SD.open("ScrambleIn", FILE_READ);
  } else {
    tft.println("   View a file:   ");
    scb_inDir = SD.open("UnScrambleOut", FILE_READ);
  }
  if (scb_inDir.isDirectory()) {
    i = 1;
    scb_inFile = scb_inDir.openNextFile(FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    while ((scb_inFile.available()) & (i < scb_TftMaxLines-2)) {
      strcpy(scb_tempName, strrchr(scb_inFile.name(), '/'));  // Strip "0:/.../"!
      strcpy((char*)&scb_sBlock[i*80], &scb_tempName[1]);
      if (strlen(scb_tempName) > scb_TftMaxLineChars) {
        scb_tempName[scb_TftMaxLineChars-1] = 0;
      }
      tft.println(&scb_tempName[1]);
      scb_inFile.close();
      scb_inFile = scb_inDir.openNextFile(FA_READ | FA_OPEN_EXISTING);
      i++;
    }
  }
  scb_inDir.close();
  strcpy((char*)&scb_sBlock[i*80], "New_File?");
  tft.println("New_File?");
  i++;
  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  tft.println("Skip?");
  strcpy((char*)&scb_sBlock[i*80], "Skip?");

  scb_newMode = scbe_ModeLine;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "_IndexPress B: scb_newMode = ");
  itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  scb5_horizontalCountMax = 0;
  scb5_verticalCountMax = i;
  scb5_selectionV = scb5_verticalCountMax;
  scb5_selectionVPrior = scb5_selectionV;
  scb5_run5WaySwitch();
}

/*** ... END SCB FILE CODE ***/

/*** BEGIN SCB OVERWRITE FILES CODE ...***/

void scbo_OverwriteFiles() {

  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0); 
  tft.setTextSize(2); // 16x8 non-mono-spaced font. Numbers are all the same size!
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.println(" Overwrite-Deleted:");
#ifdef DEBUGIO
  Serial.println("Overwite-Deleted UnScrambleOut files:");
#endif
  scb_inDir = SD.open("UnScrambleOut", FILE_READ);
  if (scb_inDir.isDirectory()) {
    scb_inFile = scb_inDir.openNextFile(FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    while (scb_inFile.available()) {

      scbo_overwiteFile(scb_inFile);
      scb_inFile.close();
      //remove(scb_inFile.name());  // Not working using f_unlink()!
      f_unlink(scb_inFile.name());
      scb_inFile.rewindDirectory();  // Appartently needed because file was over-written!
      tft.fillRect(0, 40, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, 40);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.println(scbo_blockCount);
      delay(500);

#ifdef DEBUGIO
      strcpy(debug_output, scb_inFile.name());
      strcat(debug_output, " - ");
      itoa(scbo_blockCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif

      scb_inFile = scb_inDir.openNextFile(FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    }
  }
  scb_inDir.close();
}

void scbo_overwiteFile(File file) {
  int i;
  long count;

#ifdef DEBUGIO
#ifdef Dscbo_overwiteFile
  strcpy(debug_output, "scbo_overwiteFile(): ");
  strcat(debug_output, file.name());
  Serial.println(debug_output);
#endif
#endif

  // Make scb_rand() unknown.
  file.read(&scb_unsBlock[0], scb_unsBlockSize);
  scb_randSeed = (millis() * scb_unsBlock[0] * scb_unsBlock[1]) & 0X7FFFFFFF;
#ifdef DEBUGIO
#ifdef Dscbo_overwiteFile
  strcpy(debug_output, "scb_randSeed = ");
  itoa(scb_randSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  count = file.size();
#ifdef DEBUGIO
#ifdef Dscbo_overwiteFile
  strcpy(debug_output, "count = ");
  itoa(count, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  file.seek(0);
  scbo_blockCount = 0;
  while (count > 0) {
    i = 0;
    while (i < scb_unsBlockSize) {
     scb_unsBlock[i] = scb_rand(scb_ByteValueMax);
      i++;
    }
    file.write(&scb_unsBlock[0], scb_unsBlockSize);
    count -= scb_unsBlockSize;
    scbo_blockCount++;
    if ((scbo_blockCount % 100) == 0) {
      tft.fillRect(0, 40, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, 40);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.print(scbo_blockCount);
      tft.println(" ...");
    }
  }
  file.flush();  // Needed?

#ifdef DEBUGIO
#ifdef Dscbo_overwiteFile
  Serial.println(" last scb_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif
}

/*** ... END SCB OVERWRITE FILE CODE ***/

/*** BEGIN SCB DIGIT10 CODE ...***/

void scbd_Reset() {
  scb5_verticalCount = 0;
  scb5_horizontalCount = 0;
  scb5_pressCount = 0;
  scb5_selectionV = 1;
  scb5_selectionH = 1;
  scb5_selectionVPrior = 1;
  scb5_selectionHPrior = 1;
  scb_digit10 = 0;
}

void scbd_DrawKeyPad() {

  int32_t scbd_Kx = scbd_KxStart;
  int32_t scbd_Ky = scbd_KyStart;

  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);  
  tft.setTextSize(2);  // 16x8 non-mono-spaced font. Numbers are all the same size!
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.print(" KEY: ");
  tft.setTextColor(TFT_RED, TFT_DARKGREY);
  tft.println("XXXXXXXXXX");

  tft.drawChar(scbd_Kx, scbd_Ky, '1', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '2', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '3', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '<', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx = scbd_KxStart;
  scbd_Ky += scbd_KyInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '4', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '5', TFT_GREEN, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '6', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '>', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx = scbd_KxStart;
  scbd_Ky += scbd_KyInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '7', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '8', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '9', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '0', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx = scbd_KxStart;
  scbd_Ky += scbd_KyInc;
  tft.drawChar(scbd_Kx, scbd_Ky, 'R', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, 'M', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, 'E', TFT_WHITE, TFT_BLACK, scbd_KcSize);
  scbd_Kx += scbd_KxInc;
  tft.drawChar(scbd_Kx, scbd_Ky, '?', TFT_WHITE, TFT_BLACK, scbd_KcSize);
}

void scbd_changeKey(int vert, int horz, char character, uint32_t color) {
#ifdef DEBUGIO
#ifdef Dscbd_changeKey
  strcpy(debug_output, "vert & horz = ");
  itoa(vert, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(horz, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  tft.drawChar((scbd_KxStart + (horz * scbd_KxInc)), (scbd_KyStart + (vert * scbd_KyInc)), character, color, TFT_BLACK, scbd_KcSize);
}

void scbd_KeyChange() {

#ifdef DEBUGIO
#ifdef Dscbd_KeyChange
  strcpy(debug_output, "scb5_selectionVPrior & scb5_selectionHPrior = ");
  itoa(scb5_selectionVPrior, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(scb5_selectionHPrior, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
  strcpy(debug_output, "scb5_selectionV & scb5_selectionH = ");
  itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  switch (scb5_selectionVPrior) {

    case 0:  //1 2 3 <
      switch (scb5_selectionHPrior) {
        case 0:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '1', TFT_WHITE);
          break;
        case 1:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '2', TFT_WHITE);
          break;
        case 2:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '3', TFT_WHITE);
          break;
        case 3:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '<', TFT_WHITE);
          break;
      }
      break;

    case 1:  //4 5 6 >
      switch (scb5_selectionHPrior) {
        case 0:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '4', TFT_WHITE);
          break;
        case 1:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '5', TFT_WHITE);
          break;
        case 2:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '6', TFT_WHITE);
          break;
        case 3:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '>', TFT_WHITE);
          break;
      }
      break;

    case 2:  //7 8 9 0
      switch (scb5_selectionHPrior) {
        case 0:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '7', TFT_WHITE);
          break;
        case 1:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '8', TFT_WHITE);
          break;
        case 2:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '9', TFT_WHITE);
          break;
        case 3:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '0', TFT_WHITE);
          break;
      }
      break;
      
    case 3:  //R K M ?
      switch (scb5_selectionHPrior) {
        case 0:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, 'R', TFT_WHITE);
          break;
        case 1:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, 'M', TFT_WHITE);
          break;
        case 2:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, 'E', TFT_WHITE);
          break;
        case 3:
          scbd_changeKey(scb5_selectionVPrior, scb5_selectionHPrior, '?', TFT_WHITE);
          break;
      }
      break;
      
  }

  switch (scb5_selectionV) {

    case 0:  //1 2 3  <
      switch (scb5_selectionH) {
        case 0:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '1', TFT_GREEN);
          break;
        case 1:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '2', TFT_GREEN);
          break;
        case 2:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '3', TFT_GREEN);
          break;
        case 3:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '<', TFT_GREEN);
          break;
      }
      break;

    case 1:  //4 5 6 >
      switch (scb5_selectionH) {
        case 0:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '4', TFT_GREEN);
          break;
        case 1:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '5', TFT_GREEN);
          break;
        case 2:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '6', TFT_GREEN);
          break;
        case 3:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '>', TFT_GREEN);
          break;
      }
      break;

    case 2:  //7 8 9 0
      switch (scb5_selectionH) {
        case 0:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '7', TFT_GREEN);
          break;
        case 1:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '8', TFT_GREEN);
          break;
        case 2:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '9', TFT_GREEN);
          break;
        case 3:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '0', TFT_GREEN);
          break;
      }
      break;

    case 3:  //R M E ?
      switch (scb5_selectionH) {
        case 0:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, 'R', TFT_GREEN);
          break;
        case 1:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, 'M', TFT_GREEN);
          break;
        case 2:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, 'E', TFT_GREEN);
          break;
        case 3:
          scbd_changeKey(scb5_selectionV, scb5_selectionH, '?', TFT_GREEN);
          break;
      }
      break;
      
  }

  scb5_selectionVPrior = scb5_selectionV;
  scb5_selectionHPrior = scb5_selectionH;
}

bool scbd_pressKeyAction() {

  int i;
  int j;
  char scbd_digits[11];

  switch (scb5_selectionV) {

    case 0:  //1 2 3  <
      switch (scb5_selectionH) {
        case 0:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 1;
          }
          break;
        case 1:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 2;
          }
          break;
        case 2:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 3;
          }
          break;
        case 3:
          scb_digit10 = scb_digit10 / 10;
          break;
      }
      break;

    case 1:  //4 5 6 >
      switch (scb5_selectionH) {
        case 0:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 4;
          }
          break;
        case 1:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 5;
          }
          break;
        case 2:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 6;
          }
          break;
        case 3:
          if (scb_digit10 < scb_digit10Min) {
            tft.setCursor(scbd_DigitsxStart, 0);
            tft.setTextColor(TFT_DARKGREY, TFT_DARKGREY);
            tft.println("              ");
            tft.setCursor(scbd_DigitsxStart, 0);
            tft.setTextColor(TFT_RED, TFT_DARKGREY);
            tft.println(" TO SMALL ");
            delay(500);
          } else {
            scb_newMode = scbs_ModeScramble;
            scb_mode = scb_newMode;
            return true;  // scb5_run5WaySwitch
          }
          break;
      }
      break;

    case 2:  //7 8 9 0
      switch (scb5_selectionH) {
        case 0:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 7;
          }
          break;
        case 1:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 8;
          }
          break;
        case 2:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
            scb_digit10 += 9;
          }
          break;
        case 3:
          if (scb_digit10 < scb_digit10MaxD10) {
            scb_digit10 = scb_digit10 * 10;
          }
          break;
      }
      break;

    case 3:  //R M E ?
      switch (scb5_selectionH) {
        case 0:
          scb_newMode = scbd_ModeDigit10Enter; // Reset.
          scb_mode = scb_newMode;
          return true;  // Exit scb5_run5WaySwitch()!
         break;
        case 1:
          scb_newMode = scbd_ModeDigit10M; // Mutate:
          scb_mode = scb_newMode;
          return true;  // Exit scb5_run5WaySwitch()!
          break;
        case 2:
          scb_newMode = scbd_ModeDigit10E; // Exit
          scb_mode = scb_newMode;
          return true;  // Exit scb5_run5WaySwitch()!
          break;
        case 3:
          scb_newMode = scbd_ModeDigit10Q; // ?
          scb_mode = scb_newMode;
          // extra process here?
          return true;  // Exit scb5_run5WaySwitch()!
          break;
      }
#ifdef DEBUGIO
#ifdef Dscb_mode
      strcpy(debug_output, "run5WaySwitch key3C: scb_mode = ");
      itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
#endif
      break;
      
  }

  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  itoa(scb_digit10, &scbd_digits[0], 10);
  i = 0;
  j = 10 - strlen(scbd_digits);
  while (i < j) { scbd_digits[i++] = '0'; }
  itoa(scb_digit10, &scbd_digits[i], 10);
  tft.setCursor(scbd_DigitsxStart, 0);
  tft.println(scbd_digits);

  return false;
}


/*** ... END SCB DIGIT10 CODE ***/

/*** BEGIN SCB SCRAMBLE/UNSCRAMBLE CODE ...***/

#ifdef DEBUGIO
#ifdef Doutput256Randoms
// Used to create initial values for scbs_Byte & scb_WordIndex.
void output256Randoms() {
  int i;
  int j;

  Serial.println("output256Randoms:");

  scb_randSeed = 3156848170;                            // 1 thru 4,294,967,295 PICK ONE!
  scbs_current_WordIndex = 0X000000FF & scb_Word[123];  // 0 thru 255 PICK ONE!

  Serial.println("256RandomWords = :");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_rand(scbs_dummyModulo), &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }

  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scb_unsBlockSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

  // Include all byte-values:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      j = scbs_rand(scb_unsBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(j >> 4)] & scbs_SetBitMask[(j & 0X000F)]) > 0);
    scb_unsBlock[j] = i;
    ++scbs_sBlockHistogram[i];
    scbs_sBlockUsedBits[(j >> 4)] |= scbs_SetBitMask[(j & 0X000F)];
    i++;
  }

  Serial.println("256RandomBytes = :");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }

}
#endif
#endif

#ifdef DEBUGIO
#ifdef DoutputscbBytes
// Used to create ".scbs_Bytes".
void outputscbBytes() {
  int i;
  int j;
  struct stat fs;

  Serial.println("outputscbByte:");

  scb_randSeed = 1703156848;                               // 1 thru 4,294,967,295 PICK ONE!
  scbs_current_WordIndex = 0X000000FF & scb_Word[234];  // 0 thru 255 PICK ONE!

  f_chmod(".scbs_Bytes", 0B010010010, 0);  // Make file deleteable.
  f_unlink(".scbs_Bytes");                 //  Delete file.
  scb_inFile = SD.open(".scbs_Bytes", FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS);

  Serial.println("scbWords = :");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    scb_Word[i] = scb_rand(scbs_dummyModulo);
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
   }
  }
  scb_inFile.write((uint8_t*)scb_Word, sizeof(scb_Word));

  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scb_unsBlockSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

  // Include all byte-values:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      j = scbs_rand(scb_unsBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(j >> 4)] & scbs_SetBitMask[(j & 0X000F)]) > 0);
    scb_unsBlock[j] = i;
    ++scbs_sBlockHistogram[i];
    scbs_sBlockUsedBits[(j >> 4)] |= scbs_SetBitMask[(j & 0X000F)];
    i++;
  }
  scb_inFile.write(&scb_unsBlock[0], scb_unsBlockSize);

  Serial.println("scbBytes = :");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }

  scb_inFile.close();
  f_chmod(".scbs_Bytes", 0B100100100, 0);  // Make file just readable.
}
#endif
#endif


#ifdef DEBUGIO
#ifdef DCheckSCBFiles
// Used to randomness of *.SCB files.
void CheckSCBFiles() {
  int i;
  long count;

#ifdef DEBUGIO
  Serial.println("Check .SCB Files:");
#endif
  scb_inDir = SD.open("ScrambleOut", FILE_READ);
  if (scb_inDir.isDirectory()) {
    scb_inFile = scb_inDir.openNextFile(FA_READ | FA_OPEN_EXISTING);
    while (scb_inFile.available()) {

      // Clear histogram
      i = 0;
      while (i < scbs_ByteValueMaxP1) {
        scbs_unsBlockHistogram[i] = 0;
        i++;
      }

      count = scb_inFile.size();
      scb_inFile.seek(0);  // Needed?
      while (count > 0) {
        scb_inFile.read(&scb_unsBlock[0], scb_unsBlockSize);
        i = 0;
        while (i < scb_unsBlockSize) {
          scbs_unsBlockHistogram[scb_unsBlock[i]]++;
          i++;
        }
        count -= scb_unsBlockSize;
      }
      
      scb_inFile.close();

      Serial.println(scb_inFile.name());
      Serial.println(" .SCB Histogram =:");
      Serial.print(" ");
      debug_output[0] = 0;
      i = 0;
      while (i < scbs_ByteValueMaxP1) {
        itoa(scbs_unsBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
        strcat(debug_output, ", ");
        i++;
        if ((i % 16) == 0) {
          Serial.println(debug_output);
          Serial.print(" ");
          debug_output[0] = 0;
        }
      }
      Serial.println(debug_output);

      scb_inFile = scb_inDir.openNextFile(FA_READ | FA_OPEN_EXISTING);
    }
  }
  scb_inDir.close();
}
#endif
#endif

void scbs_Setup() {

  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
    tft.setCursor(0, 0);  // 16x8 non-mono-spaced font. Numbers are all the same size!
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    tft.print(" SDCARD initialization    failed! ");
    while (1) { delay(1000); }
  }

#ifdef DEBUGIO
  Serial.println("SDCARD initialized.");
#endif
}

// Used ".scbs_Bytes" to xor scb_Word & to shuffle scb_WordIndex.
void scbs_shuffleBytes() {
  int i;
  int iI1;
  int iI2;
  scb_uint8 temp;
  union scbs_UnsInteger {
    scb_uint8 bytes[4];
    unsigned int integer;
  };
  union scbs_Integer tempword;

  scb_randSeed = scb_digit10;
  scbs_current_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[(scb_digit10 & 0X000000FF)]];

  scb_inFile = SD.open(".scbs_Bytes", FA_READ);

  if (!scb_inFile.available()) {
    tft.setCursor(0, 0);  // 16x8 non-mono-spaced font. Numbers are all the same size!
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    tft.print(" No .scbs_Bytes file on SDCARD! ");
    while (1);
  }

#ifdef DEBUGIO
#ifdef Dscbs_shuffleBytes
  Serial.println("scbs_shuffleBytes() of scb_Word = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
  Serial.println("scbs_shuffleBytes() of scb_WordIndex = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_WordIndex[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  //Xor scb_Word:
  i = 0;
  while (i < scb_unsBlockSize) {
    if (scb_inFile.readBytes(&tempword.bytes[0], 4) != 4) {
      tft.setCursor(0, 0, 2);  // 16x8 non-mono-spaced font. Numbers are all the same size!
      tft.setTextSize(2);
      tft.setTextColor(TFT_RED, TFT_DARKGREY);
      // scb_unsBlockSize bytes missing from .scbs_Bytes file on SDCARD!
      tft.print(" Bad Words on .scbs_Bytes  file on SDCARD! ");
      while (1);

    }
    scb_Word[i] ^= tempword.integer;
    i++;
  }
  
  if (scb_inFile.readBytes(&scb_unsBlock[0], scb_unsBlockSize) != scb_unsBlockSize) {
    tft.setCursor(0, 0, 2);  // 16x8 non-mono-spaced font. Numbers are all the same size!
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    // scb_unsBlockSize bytes missing from .scbs_Bytes file on SDCARD!
    tft.print(" Bad Bytes on .scbs_Bytes  file on SDCARD! ");
    while (1);
  }

  // Shuffle scb_WordIndex:
  i = 0;
  while (i < (scb_unsBlockSize >> 1)) {
    iI1 = scb_unsBlock[i];
    iI2 = scb_unsBlock[i + 1];
    temp = scb_WordIndex[iI1];
    scb_WordIndex[iI1] = scb_WordIndex[iI2];
    scb_WordIndex[iI2] = temp;
    i += 2;
  }

#ifdef DEBUGIO
#ifdef Dscbs_shuffleBytes
  Serial.println("scbs_shuffleBytes() of scb_Word = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
  Serial.println("scbs_shuffleBytes() of scb_WordIndex = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_WordIndex[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif
}

void scbs_randomControls() {
  int i;
  unsigned int filenameint;
  int length;

  // Setup random controls:
  scbs_xor_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[172]];     // 0 thru 255 PICK ONE!
  scbs_expand_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[183]];  // 0 thru 255 PICK ONE!
  scbs_load_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[30]];     // 0 thru 255 PICK ONE!
  scbs_sep_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[148]];     // 0 thru 255 PICK ONE!

#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "xor/expand/load/sep DigitIndex's = ");
  itoa(scbs_xor_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_expand_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_load_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sep_WordIndex, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Ramdomize with file name:
  strcpy(scb_tempName, strrchr(scb_inFile.name(), '/'));  // Strip "0:/.../"!
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "scb_tempName = ");
  strcat(debug_output, scb_tempName);
  Serial.println(debug_output);
#endif
#endif
  length = strlen(scb_tempName);
  if ((scb_tempName[length - 4] == '.') && (scb_tempName[length - 3] == 'S') && (scb_tempName[length - 2] == 'C') && (scb_tempName[length - 1] == 'B')) {
    scb_tempName[(strlen(scb_tempName) - 4)] = 0;  // Strip ".SCB"
  }
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "scb_tempName = ");
  strcat(debug_output, scb_tempName);
  Serial.println(debug_output);
#endif
#endif
  filenameint = scb_WordIndex[176];  // 0 thru 255 PICK ONE!
  i = 0;
  while (i < ((int)strlen(scb_tempName) - 1)) {
    if (filenameint > (UINT_MAX >> 7)) {
      filenameint = filenameint >> 7;
    }
    filenameint += (uint)scb_tempName[i];
    i++;
  }
  filenameint = abs(filenameint);
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "filenameint = ");
  itoa(filenameint, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Setup file seeds:
#ifdef DEBUGIO
#ifdef Dsave_randoms
        strcpy(debug_output, "scb_digit10 = ");
        itoa(scb_digit10, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
  scb_randSeed = scb_digit10 ^ filenameint;
#ifdef DEBUGIO
#ifdef Dsave_randoms
        strcpy(debug_output, "scb_randSeed = ");
        itoa(scb_randSeed, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
  scbs_current_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[(scb_randSeed & 0x000000FF)]];
#ifdef DEBUGIO
#ifdef Dsave_randoms
        strcpy(debug_output, "scbs_current_WordIndex = ");
        itoa(scbs_current_WordIndex, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
  i = 0;
  while (i < (int)(0X000000FF & (scb_Word[scb_WordIndex[110]] >> 17))) {  // 0 thru 255 PICK ONE! & shift 0 thru 24 PICK ONE!
    scbs_rand(scbs_dummyModulo);
    i++;
  }
  scbs_xorSeed = scbs_rand(scbs_dummyModulo);
  i = 0;
  while (i < (int)(0X000000FF & (scb_Word[scb_WordIndex[63]] >> 9))) {  /// 0 thru 255 PICK ONE! & shift 0 thru 24 PICK ONE!
    scbs_rand(scbs_dummyModulo);
    i++;
  }
  scbs_expandSeed = scbs_rand(scbs_dummyModulo);
  i = 0;
  while (i < (int)(0X000000FF & (scb_Word[scb_WordIndex[66]] >> 2))) {  // 0 thru 255 PICK ONE! & shift 0 thru 24 PICK ONE!
    scbs_rand(scbs_dummyModulo);
    i++;
  }
  scbs_loadSeed = scbs_rand(scbs_dummyModulo);
  i = 0;
  while (i < (int)(0X000000FF & (scb_Word[scb_WordIndex[239]] >> 14))) {  // 0 thru 255 PICK ONE! & shift 0 thru 24 PICK ONE!
    scbs_rand(scbs_dummyModulo);
    i++;
  }
  scbs_sepSeed = scbs_rand(scbs_dummyModulo);

#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "xor/expand/load/sep randoms = ");
  itoa(scbs_xorSeed, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_expandSeed, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_loadSeed, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sepSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

#ifdef DEBUGIO
#ifdef Dsave_randoms
  debug_output[0] = 0;
  Serial.println(debug_output);
  Serial.println("scb_WordIndex = 0X....:");
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_WordIndex[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  debug_output[0] = 0;
  Serial.println(debug_output);
  Serial.println("scb_Byte = 0X....:");
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

}

/* Reentrant random function from POSIX.1c.
	 Copyright (C) 1996-2020 Free Software Foundation, Inc.
	 This file is part of the GNU C Library.
	 Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

	 The GNU C Library is free software; you can redistribute it and/or
	 modify it under the terms of the GNU Lesser General Public
	 License as published by the Free Software Foundation; either
	 version 2.1 of the License, or (at your option) any later version.

	 The GNU C Library is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
	 Lesser General Public License for more details.

	 You should have received a copy of the GNU Lesser General Public
	 License along with the GNU C Library; if not, see
	 <https://www.gnu.org/licenses/>.	 */
/* This algorithm is mentioned in the ISO C standard, here extended
	 for 32 bits.	 */
/* Modified by RTC for scbs_rand()!	*/
int scbs_rand(const unsigned int mask) {
  unsigned int next = scb_randSeed;
  int result;

  //RTC Begin Mutation:
  next ^= (unsigned int ) scb_Word[scb_WordIndex[scbs_current_WordIndex]];  //index];
  --scbs_current_WordIndex;
  if (scbs_current_WordIndex < 0) { scbs_current_WordIndex = scb_WordIndexMax; }
#ifdef DEBUGIO
#ifdef Dscbs_rand
  Serial.println("scbs_rand():");
  strcpy(debug_output, "index, scbs_current_WordIndex =:");
  itoa(index, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_current_WordIndex, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  //RTC End Mutation:

  next *= 1103515245;
  next += 12345;
//RTC  result = (unsigned int) (next / 65536) % 2048;
  result = (unsigned int) (next >> 16) & 0x0007FF;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
//RTC  result ^= (unsigned int) (next / 65536) % 1024;
  result ^= (unsigned int) (next >> 16) & 0x0003FF;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
//RTC  result ^= (unsigned int) (next / 65536) % 1024;
  result ^= (unsigned int) (next >> 16) & 0x0003FF;

  scb_randSeed = next;

 //RTC return result;
  return (result &= mask);  // RTC
}

void scbs_clearHistograms() {
  int i;

  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    scbs_unsBlockHistogram[i] = 0;
    scbs_sBlockHistogram[i] = 0;
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_clearHistograms()

  Serial.println("scbs_clearHistograms():");

  Serial.println("scbs_unsBlockHistogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_unsBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockHistogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_sBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

#endif
#endif
}

void scbs_xorUnsBlock() {
  int i;

#ifdef DEBUGIO
#ifdef Dscbs_xorUnsBlock
  Serial.println("scbs_xorUnsBlock():");
  Serial.println("scbs_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  // Get xor randoms:
  scb_randSeed = scbs_xorSeed;
  scbs_current_WordIndex = scbs_xor_WordIndex;

  scbs_rand(scbs_dummyModulo);
  i = 0;
  while (i < (int)(0X000000FF & (scb_Word[scb_WordIndex[94]] >> 23))) {  // 0 thru 255 PICK ONE! & shift 0 thru 24 PICK ONE!
    scbs_rand(scbs_dummyModulo);
    i++;
  }

  i = 0;
  while (i < scb_unsBlockSize) {
    scb_unsBlock[i] ^= scbs_rand(scbs_ByteValueMax);
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_xorUnsBlock
  Serial.println("scbs_xorUnsBlock() of scbs_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  // Save xor randoms:
  scbs_xorSeed = scbs_rand(scbs_dummyModulo);
  scbs_xor_WordIndex = scb_WordIndex[scbs_xor_WordIndex];
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "xor randoms = ");
  itoa(scbs_xor_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_xorSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbs_histogramUnsBlock() {
  int i;

  // Histogram:
  i = 0;
  while (i < scb_unsBlockSize) {
    ++scbs_unsBlockHistogram[scb_unsBlock[i]];
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_unsBlockHistogram
  Serial.println("scbs_histogramUnsBlock():");
  Serial.println("scbs_unsBlockHistogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_unsBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif
}

void scbs_randomFillSBlock() {  // using all byte-values & filling scbs_sHistogram.
  int i;
  int j;

  // Make scbs_rand() unknown.
  scbs_current_WordIndex = 0X000000FF & scb_Word[scbs_current_WordIndex];
  scb_randSeed = (millis() + scb_Word[scbs_current_WordIndex]) & 0X3FFFFFFF;


  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_randomFillSBlock

  Serial.println("scbs_randomFillSBlock():");

  Serial.println("Clear scbs_sBlockUsedBits:");

  Serial.println("scbs_sBlockUsedBits = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    itoa(scbs_sBlockUsedBits[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 8) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

#endif
#endif

  // Include all byte-values:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      j = scbs_rand(scb_sBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(j >> 4)] & scbs_SetBitMask[(j & 0X000F)]) > 0);
    scb_sBlock[j] = i;
    ++scbs_sBlockHistogram[i];
    scbs_sBlockUsedBits[(j >> 4)] |= scbs_SetBitMask[(j & 0X000F)];
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_randomFillSBlock

  Serial.println("Include all byte-values:");

  Serial.println("scbs_sBlock = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_sBlockSize) {
    itoa(scb_sBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockUsedBits = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    itoa(scbs_sBlockUsedBits[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 8) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockHistogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_sBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

#endif
#endif

  // Random fill byte-values:
  i = 0;
  while (i < scb_sBlockSize) {
    if (!((scbs_sBlockUsedBits[(i >> 4)] & scbs_SetBitMask[(i & 0X000F)]) > 0)) {
      scb_sBlock[i] = scbs_rand(scbs_ByteValueMax);
      ++scbs_sBlockHistogram[scb_sBlock[i]];
      scbs_sBlockUsedBits[(i >> 4)] |= scbs_SetBitMask[(i & 0X000F)];
    }
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_randomFillSBlock

  Serial.println("Random fill byte-values:");

  Serial.println("scbs_sBlock = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_sBlockSize) {
    itoa(scb_sBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockUsedBits = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    itoa(scbs_sBlockUsedBits[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 8) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockHistogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_sBlockHistogram[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

#endif
#endif
}


// Iterative implementation
int scbs_binarySearch(scb_uint8 a[], int item, int low, int high) {
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (item == a[mid])
      return mid + 1;
    else if (item > a[mid])
      low = mid + 1;
    else
      high = mid - 1;
  }

  return low;
}

// Function to sort an array a[] of size 'n'
void scbs_sort(scb_uint8 a[], scb_uint8 b[], int n) {
  int i, loc, j, aselected, bselected;

#ifdef DEBUGIO
#ifdef Dscbs_sort
  Serial.println("histogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < n) {
    itoa(a[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println("value = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < n) {
    itoa(b[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
#endif
#endif

  for (i = 1; i < n; ++i) {
    j = i - 1;
    aselected = a[i];
    bselected = b[i];

    // Find location where selected should be inserted
    loc = scbs_binarySearch(a, aselected, 0, j);

    // Move all elements after location to create space
    while (j >= loc) {
      a[j + 1] = a[j];
      b[j + 1] = b[j];
      j--;
    }
    a[j + 1] = aselected;
    b[j + 1] = bselected;
  }

#ifdef DEBUGIO
#ifdef Dscbs_sort
  Serial.println("histogram =:");
  debug_output[0] = 0;
  i = 0;
  while (i < n) {
    itoa(a[i], &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println("value = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < n) {
    itoa(b[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
#endif
#endif
}

void scbs_selectUnsToSValuesUsingHistograms() {
  int i;
  int j;

  // Setup values:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    scbs_unsBlockValues[i] = i;
    scbs_sBlockValues[i] = i;
    i++;
  }

  // Order scbs_unsHistogram with scbs_unsBlockValues:
  scbs_sort(&scbs_unsBlockHistogram[0], &scbs_unsBlockValues[0], scbs_ByteValueMaxP1);

  // Order scbs_sHistogram with scbs_sBlockValues:
  scbs_sort(&scbs_sBlockHistogram[0], &scbs_sBlockValues[0], scbs_ByteValueMaxP1);

  // Fill scbs_translationValues:
  i = 0;
  j = scbs_ByteValueMax;
  while (i < scbs_ByteValueMaxP1) {
    scbs_translationValues[scbs_unsBlockValues[i]] = scbs_sBlockValues[j];
    i++;
    j--;
  }

#ifdef DEBUGIO
#ifdef Dscbs_selectUnsToSValuesUsingHistograms

  Serial.println("scbs_selectUnsToSValuesUsingHistograms():");

  Serial.println("scbs_unsBlockValues = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_unsBlockValues[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_sBlockValues = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_sBlockValues[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

  Serial.println("scbs_translationValues = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);

#endif
#endif
}

void scbs_randomLoadSBlock() {
  int i;
  int r;

  // Get load/unload randoms:
  scb_randSeed = scbs_loadSeed;
  scbs_current_WordIndex = scbs_load_WordIndex;

#ifdef DEBUGIO
#ifdef Dscbs_randomLoadSBlock
  Serial.println("scbs_randomLoadSBlock():");
#endif
#endif

  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

  // Load scbs_fileSize in frist block:
  if (scbs_firstBlock) {
    i = 0;
    while (i < 4) {
      scbs_fileSize.bytes[i] ^= scbs_rand(scbs_ByteValueMax);
      i++;
    }
    i = 0;
    while (i < 4) {
      do {
        r = scbs_rand(scb_sBlockSizeMask);
      } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
      scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
      scb_sBlock[r] = scbs_fileSize.bytes[i];
      i++;
    }
    scbs_firstBlock = false;
#ifdef DEBUGIO
    strcpy(debug_output, "xored file size = ");
    itoa(scbs_fileSize.integer, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
  }

  // Load scbs_translationValues:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      r = scbs_rand(scb_sBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
    scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
    scb_sBlock[r] = scbs_translationValues[i];
    i++;
  }
#ifdef DEBUGIO
#ifdef Dscbs_randomLoadSBlock
  strcpy(debug_output, "scbs_translationValues[i], i, r = ");
  itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
  strcat(debug_output, ", ");
  itoa(i, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(r, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Translate & load scbs_sBlock:
  i = 0;
  while (i < scb_unsBlockSize) {
    do {
      r = scbs_rand(scb_sBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
    scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
    scb_sBlock[r] = scbs_translationValues[scb_unsBlock[i]];
    i++;
  }
#ifdef DEBUGIO
#ifdef Dscbs_randomLoadSBlock
  strcpy(debug_output, "scb_unsBlock[i], i, r = ");
  itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
  strcat(debug_output, ", ");
  itoa(i, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(r, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Save load/unload randoms:
  scbs_loadSeed = scbs_rand(scbs_dummyModulo);
  scbs_load_WordIndex = scb_WordIndex[scbs_load_WordIndex];
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "load/unload randoms = ");
  itoa(scbs_load_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_loadSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbs_scrambleBlock() {
#ifdef DEBUGIO
  int i;
#endif

#ifdef DEBUGIO
#ifdef Dscbs_scrambleBlock
  Serial.println("scbs_scrambleBlock():");
  Serial.println("scbs_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  scbs_xorUnsBlock();

  scbs_clearHistograms();

  scbs_histogramUnsBlock();

  scbs_randomFillSBlock();

  scbs_selectUnsToSValuesUsingHistograms();

  scbs_randomLoadSBlock();
}

void scbs_unloadSBlock() {
  int i;
  int r;
  scb_uint8 tempbytes[4];

  // Get load/unload randoms:
  scb_randSeed = scbs_loadSeed;
  scbs_current_WordIndex = scbs_load_WordIndex;

#ifdef DEBUGIO
#ifdef Dscbs_unloadSBlock
  Serial.println("scbs_unloadSBlock():");
#endif
#endif

  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

  // UnLoad scbs_fileSize:
  if (scbs_firstBlock) {
    i = 0;
    while (i < 4) {
      tempbytes[i] = scbs_rand(scbs_ByteValueMax);
      i++;
    }
    i = 0;
    while (i < 4) {
      do {
        r = scbs_rand(scb_sBlockSizeMask);
      } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
      scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
      scbs_fileSize.bytes[i] = scb_sBlock[r];
      //TEMPscbs_fileSize.bytes[i] ^= tempbytes[i];
      i++;
    }
#ifdef DEBUGIO
    strcpy(debug_output, "xored file size = ");
    itoa(scbs_fileSize.integer, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
    i = 0;
    while (i < 4) {
      scbs_fileSize.bytes[i] ^= tempbytes[i];
      i++;
    }
    scbs_firstBlock = false;
#ifdef DEBUGIO
    strcpy(debug_output, "file size = ");
    itoa(scbs_fileSize.integer, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
  }

  // Unload scbs_translationValues:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      r = scbs_rand(scb_sBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
    scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
    scbs_translationValues[i] = scb_sBlock[r];
    i++;
  }
#ifdef DEBUGIO
#ifdef Dscbs_unloadSBlock
  strcpy(debug_output, "scbs_translationValues[i], i, r = ");
  itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
  strcat(debug_output, ", ");
  itoa(i, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(r, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

#ifdef DEBUGIO
#ifdef Dscbs_selectUnsToSValuesUsingHistograms
  Serial.println("scbs_translationValues = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  // Change scbs_translationValues to un-scbs_translationValues ...
  //   Tempoaraly using scbs_unsBlock:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    scb_unsBlock[scbs_translationValues[i]] = i;
    i++;
  }
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    scbs_translationValues[i] = scb_unsBlock[i];
    i++;
  }


  // UnLoad scbs_sBlock & translate:
  i = 0;
  while (i < scb_unsBlockSize) {
    do {
      r = scbs_rand(scb_sBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(r >> 4)] & scbs_SetBitMask[(r & 0X000F)]) > 0);
    scbs_sBlockUsedBits[(r >> 4)] |= scbs_SetBitMask[(r & 0X000F)];
    scb_unsBlock[i] = scbs_translationValues[scb_sBlock[r]];
#ifdef DEBUGIO
#ifdef Dscbs_unloadSBlock
    strcpy(debug_output, "scb_unsBlock[i], i, r = ");
    itoa(scbs_translationValues[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    itoa(i, &debug_output[strlen(debug_output)], 10);
    strcat(debug_output, ", ");
    itoa(r, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
#endif
    i++;
  }

  // Save load/unload randoms:
  scbs_loadSeed = scbs_rand(scbs_dummyModulo);
  scbs_load_WordIndex = scb_WordIndex[scbs_load_WordIndex];
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "load/unload randoms = ");
  itoa(scbs_load_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_loadSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbs_unscrambleBlock() {
#ifdef DEBUGIO
  int i;
#endif

  scbs_unloadSBlock();

  scbs_xorUnsBlock();

#ifdef DEBUGIO
#ifdef Dscbs_unscrambleBlock
  Serial.println("scbs_unscrambleBlock():");
  Serial.println("scbs_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif
}

void scbs_overwiteFile(File file) {
  int i;
  long count;

#ifdef DEBUGIO
#ifdef Dscbs_overwiteFile
  strcpy(debug_output, "scbs_overwiteFile(): ");
  strcat(debug_output, file.name());
  Serial.println(debug_output);
#endif
#endif

  count = file.size();
  // Make scbs_rand() unknown.
  scbs_current_WordIndex = count & 0X000000FF;
  scb_randSeed = (millis() + scb_Word[scbs_current_WordIndex]) & 0X3FFFFFFF;

  file.seek(0);  // Needed?
  scbs_blockCount = 0;
  while (count > 0) {
    i = 0;
    while (i < scb_unsBlockSize) {
      scb_unsBlock[i] = scbs_rand(scbs_ByteValueMax);
      i++;
    }
    file.write(&scb_unsBlock[0], scb_unsBlockSize);
    count -= scb_unsBlockSize;
    scbs_blockCount++;
    if ((scbs_blockCount % 100) == 0) {
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.print(scbs_blockCount);
      tft.println(" ...");
    }
  }
  file.flush();  // Needed?

#ifdef DEBUGIO
#ifdef Dscbs_overwiteFile
  Serial.println(" last scbs_UnsBlock() = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_unsBlock[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 16) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif
}

void scbs_randomLoadSBlockSep() {
  int i;
  int size;

  // Get sep randoms:
  scbs_current_WordIndex = scbs_sep_WordIndex;
  scb_randSeed = scbs_sepSeed;
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "sepation randoms = ");
  itoa(scbs_sep_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sepSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Create & output block separation:
  size = (0X000000FF & scb_Word[scb_WordIndex[scbs_current_WordIndex]]) + 1;
  i = 0;
  while (i < size) {
    scb_unsBlock[i] = scbs_rand(scbs_ByteValueMax);
    i++;
  }
  scb_outFile.write(&scb_unsBlock[0], size);

  // Save sep randoms:
  scbs_sepSeed = scbs_rand(scbs_dummyModulo);
  scbs_sep_WordIndex = scb_WordIndex[scbs_sep_WordIndex];
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "sepation randoms = ");
  itoa(scbs_sep_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sepSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbs_randomSkipSBlockSep() {
  int i;
  int size;

  // Get sep randoms:
  scbs_current_WordIndex = scbs_sep_WordIndex;
  scb_randSeed = scbs_sepSeed;
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "sepation randoms = ");
  itoa(scbs_sep_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sepSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  // Skip block separation:
  size = (0X000000FF & scb_Word[scb_WordIndex[scbs_current_WordIndex]]) + 1;
  i = 0;
  while (i < size) {
    scbs_rand(scbs_ByteValueMax);
    i++;
  }
  scb_inFile.read(&scb_sBlock[0], size);

  // Save sep randoms:
  scbs_sepSeed = scbs_rand(scbs_dummyModulo);
  scbs_sep_WordIndex = scb_WordIndex[scbs_sep_WordIndex];
#ifdef DEBUGIO
#ifdef Dsave_randoms
  strcpy(debug_output, "sepation randoms = ");
  itoa(scbs_sep_WordIndex, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, ", ");
  itoa(scbs_sepSeed, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbs_scrambleFile() {
  int i;
  int count;

#ifdef DEBUGIO
#ifdef Dscbs_scrambleFile
  strcpy(debug_output, "scbs_scrambleFile(): ");
  strcat(debug_output, scb_inFile.name());
  Serial.println(debug_output);
#endif
#endif

  // Setup random controls:
  scbs_randomControls();

  // Get file size.
  scbs_firstBlock = true;
  scbs_fileSize.integer = scb_inFile.size();
#ifdef DEBUGIO
  strcpy(debug_output, "file size = ");
  itoa(scbs_fileSize.integer, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif

  // Process file:
  count = scb_inFile.readBytes(&scb_unsBlock[0], scb_unsBlockSize);
  scbs_blockCount = 0;
  while (count > 0) {
    // If nessessary, fill with 0's to complete last block.
    if (count < scb_unsBlockSize) {
      i = count;
      while (i < scb_unsBlockSize) {
        scb_unsBlock[i] = 0;
        i++;
      }
    }
    scbs_scrambleBlock();
    scbs_randomLoadSBlockSep();
    scb_outFile.write(&scb_sBlock[0], scb_sBlockSize);
    count = scb_inFile.readBytes(&scb_unsBlock[0], scb_unsBlockSize);
    scbs_blockCount++;
    if ((scbs_blockCount % 25) == 0) {
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.print(scbs_blockCount);
      tft.println(" ...");
    }

  }
}

void scbs_unscrambleFile() {
#ifdef DEBUGIO
  int i;
#endif
  int count;

#ifdef DEBUGIO
#ifdef Dscbs_unscrambleFile
  strcpy(debug_output, "scbs_unscrambleFile()): ");
  strcat(debug_output, scb_inFile.name());
  Serial.println(debug_output);
#endif
#endif

  // Setup random controls:
  scbs_randomControls();

  // Get file size from first block.
  scbs_firstBlock = true;

  scbs_blockCount = 0;
  scbs_randomSkipSBlockSep();
  count = scb_inFile.readBytes(&scb_sBlock[0], scb_sBlockSize);
  while (count > 0) {
    // Process file:
    scbs_unscrambleBlock();
    scbs_blockCount++;
    if ((scbs_blockCount % 100) == 0) {
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.print(scbs_blockCount);
      tft.println(" ...");
    }

    if (scbs_fileSize.integer < scb_unsBlockSize) {
      scb_outFile.write(&scb_unsBlock[0], scbs_fileSize.integer);
      break;
    } else {
      scb_outFile.write(&scb_unsBlock[0], scb_unsBlockSize);
      scbs_fileSize.integer -= scb_unsBlockSize;
    }
    if (scb_inFile.available()) {
      scbs_randomSkipSBlockSep();
      count = scb_inFile.readBytes(&scb_sBlock[0], scb_sBlockSize);
    } else {
      count = 0;
    }
  }
}

void scbs_OverwriteFiles() {

  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);  // 16x8 non-mono-spaced font. Numbers are all the same size!
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.println(" Overwrite-Deleted:");
#ifdef DEBUGIO
  Serial.println("Overwite-Deleted UnScrambleOut files:");
#endif
  scb_inDir = SD.open("UnScrambleOut", FILE_READ);
  if (scb_inDir.isDirectory()) {
    scb_inFile = scb_inDir.openNextFile(FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    while (scb_inFile.available()) {

      scbs_overwiteFile(scb_inFile);
      scb_inFile.close();
      //remove(scb_inFile.name());  // Not working using f_unlink()!
      f_unlink(scb_inFile.name());
      scb_inFile.rewindDirectory();  // Appartently needed because file was over-written!
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.println(scbs_blockCount);
      delay(500);

#ifdef DEBUGIO
      strcpy(debug_output, scb_inFile.name());
      strcat(debug_output, " - ");
      itoa(scbs_blockCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif

      scb_inFile = scb_inDir.openNextFile(FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    }
  }
  scb_inDir.close();
}

void scbs_scramble() {

  // Mutate Code using scb_digit10 & ".scbs_Bytes" file on SDCARD:
  scbs_shuffleBytes();

  // Process files to be scrambled:
  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.println(" Scramble:");
#ifdef DEBUGIO
  Serial.println("Scramble ScrambleIn files:");
#endif
  scb_inDir = SD.open("ScrambleIn", FILE_READ);
  if (scb_inDir.isDirectory()) {
    scb_inFile = scb_inDir.openNextFile(FILE_READ);
    while (scb_inFile.available()) {
      strcpy(scb_tempName, "ScrambleOut");
      strcat(scb_tempName, strrchr(scb_inFile.name(), '/'));  // Strip "0:/.../"!
      strcat(scb_tempName, ".SCB");
      scb_outFile = SD.open(scb_tempName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS);
      scbs_scrambleFile();
      scb_inFile.close();
      scb_outFile.close();
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.println(scbs_blockCount);
      delay(500);
#ifdef DEBUGIO
      strcpy(debug_output, scb_inFile.name());
      strcat(debug_output, " - ");
      itoa(scbs_blockCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
      scbs_overwiteFile(scb_inFile);
      //remove(scb_inFile.name());  // Not working using f_unlink()!
      f_unlink(scb_inFile.name());
      scb_inFile = scb_inDir.openNextFile(FILE_READ);
    }
  }
  scb_inDir.close();

  // Process files to be unscrambled:
  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.println(" Unscramble:");
#ifdef DEBUGIO
  Serial.println("Unscramble UnScrambleIn files:");
#endif
  scb_inDir = SD.open("UnScrambleIn", FILE_READ);
  if (scb_inDir.isDirectory()) {
    scb_inFile = scb_inDir.openNextFile(FILE_READ);
    while (scb_inFile.available()) {
      strcpy(scb_tempName, "UnScrambleOut");
      strcat(scb_tempName, strrchr(scb_inFile.name(), '/'));                     // Strip "0:/.../"!
      if (strcmp(&scb_tempName[(strlen(scb_tempName) - 4)], ".SCB")) { break; }  // Only *.SCB files!
      scb_tempName[(strlen(scb_tempName) - 4)] = 0;                              // Strip ".SCB"
#ifdef DEBUGIO
      strcpy(debug_output, "scb_tempName = ");
      strcat(debug_output, scb_tempName);
      Serial.println(debug_output);
#endif
      scb_outFile = SD.open(scb_tempName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS);
      scbs_unscrambleFile();
      scb_inFile.close();
      scb_outFile.close();
      tft.fillRect(0, scbd_KyInc, 320, 240, TFT_DARKGREY);
      tft.setCursor(0, scbd_KyInc);
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
      tft.print(scb_inFile.name());
      tft.print(" - ");
      tft.println(scbs_blockCount);
      delay(500);
#ifdef DEBUGIO
      strcpy(debug_output, scb_inFile.name());
      strcat(debug_output, " - ");
      itoa(scbs_blockCount, &debug_output[strlen(debug_output)], 10);
      Serial.println(debug_output);
#endif
      //remove(scb_inFile.name());  // Not working using f_unlink()!
      f_unlink(scb_inFile.name());
      scb_inFile = scb_inDir.openNextFile(FILE_READ);
    }
  }
  scb_inDir.close();
}

void scbs_Reset() {
  int i;

  scb_digit10 = 0;
  scbs_current_WordIndex = 0;
  i = 0;
  while (i < scb_IndexWordSize) {
    scb_WordIndex[i] = scb_WordIndexDef[i];
    scb_Word[i] = scbs_WordDef[i];
    i++;
  }

  
}

void scbs_Mutate() {
  int i ;
  int j;
  int iI1;
  int iI2;
  scb_uint8 temp;

#ifdef DEBUGIO
  Serial.println("Mutate Code:");
#endif

  scb_randSeed = scb_digit10;
  scbs_current_WordIndex = 0X000000FF & scb_Word[scb_WordIndex[(scb_digit10 & 0x000000FF)]];

#ifdef DEBUGIO
#ifdef Dscbs_Mutate
  Serial.println("scbs_Word = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_IndexWordSize) {
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

  // Clear scbs_sBlockUsedBits:
  i = 0;
  while (i < scbs_sBlockUsedBitsSize) {
    scbs_sBlockUsedBits[i] = 0;
    i++;
  }

  // Include all byte-values:
  i = 0;
  while (i < scbs_ByteValueMaxP1) {
    do {
      j = scbs_rand(scb_unsBlockSizeMask);
    } while ((scbs_sBlockUsedBits[(j >> 4)] & scbs_SetBitMask[(j & 0X000F)]) > 0);
    scb_unsBlock[j] = i;
    scbs_sBlockUsedBits[(j >> 4)] |= scbs_SetBitMask[(j & 0X000F)];
    i++;
  }

  // Shuffle scbs_Byte:
  i = 0;
  while (i < (scb_unsBlockSize >> 1)) {
    iI1 = scb_unsBlock[i];
    iI2 = scb_unsBlock[i + 1];
    temp = scb_Word[iI1];
    scb_Word[iI1] = scb_Word[iI2];
    scb_Word[iI2] = temp;
    i++;
  }

#ifdef DEBUGIO
#ifdef Dscbs_Mutate
  Serial.println("scbs_Byte = 0X....:");
  debug_output[0] = 0;
  i = 0;
  while (i < scb_unsBlockSize) {
    itoa(scb_Word[i], &debug_output[strlen(debug_output)], 16);
    strcat(debug_output, ", ");
    i++;
    if ((i % 4) == 0) {
      Serial.println(debug_output);
      debug_output[0] = 0;
    }
  }
  Serial.println(debug_output);
#endif
#endif

}
 

/*** ... END SCB SCRAMBLE/UNSCRAMBLE CODE ***/

/*** BEGIN SCB EDITOR CODE ...***/

void scbe_Reset() {

  scbe_currentIndex = 0;
  scbe_currentX = 0;
  scbe_currentY = 0;
  scbe_currentChar = 0;

}

void scbe_preProsBlock() {
  int i;

  i = 0;
  while (i < scb_unsBlockSize) {
    if (((scb_unsBlock[i] < ' ') & (scb_unsBlock[i] != '\n')) | \
       (scb_unsBlock[i] > '~')) {
      scb_unsBlock[i] = ' ';
    }
    if (scb_unsBlock[i] == '\n') { scb_unsBlock[i] = scbe_NewLineChr; }
    if (scb_unsBlock[i] == ' ') { scb_unsBlock[i] = scbe_BlankChr; }
    i++;
  }
}

void scbe_postProsBlock() {
  int i;

  // Replace new-lines:
  i = 0;
  while (i < scb_unsBlockSize) {
    if (scb_unsBlock[i] == scbe_NewLineChr) { scb_unsBlock[i] = '\n'; }
    if (scb_unsBlock[i] == scbe_BlankChr) { scb_unsBlock[i] = ' ';}
  i++;
  }

  // Strip trailing blanks and new-lines & fill with 0's:
  i = scb_unsBlockSize-1;
  while (i >= 0) {
    if ((scb_unsBlock[i] == ' ') | (scb_unsBlock[i] == '\n')) {
      scb_unsBlock[i] = 0;
    i--; 
    } else {
      break;
    }
  
  }

}

void scbe_displayBlock() {
  int i;

  tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
  tft.setCursor(0, 0);
  tft.setTextSize(2); // 16x8 non-mono-spaced font. Numbers are all the same size!
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  i = 0;
  while (i < scb_unsBlockSize) {
    tft.print((char)scb_unsBlock[i]);
    i++;
  }

  if (scbe_currentIndex == 0) {
    scbe_currentChar = scb_unsBlock[0];
    tft.setCursor(scbe_currentX, scbe_currentY);
    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    tft.print(scbe_currentChar);
  }
}

void scbe_IndexChange() {

  if (scbe_currentIndex != scb_unsBlockSize) {
    tft.setCursor(scbe_currentX, scbe_currentY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.print(scbe_currentChar);
  }

  scbe_currentIndex = scb5_selectionH;

  if (scbe_currentIndex != scb_unsBlockSize) {
    scbe_currentChar = scb_unsBlock[scbe_currentIndex];

    scbe_currentX = scbe_currentIndex * scbe_incrementX;
    scbe_currentY = (scbe_currentX / scbe_maxX) * scbe_incrementY;
    scbe_currentX %= scbe_maxX;

    tft.setCursor(scbe_currentX, scbe_currentY);
    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    tft.print(scbe_currentChar);
  }

  scb5_selectionHPrior = scb5_selectionH;
}

void scbe_IndexPress() {

#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
  Serial.println("Switch to char mode.");
#endif
#endif
  // start with 'e'.
  scbe_sellectedChar = 'e';
  scb5_selectionV = 1;
  scb5_selectionVPrior = 1;
  scb5_selectionH = 4;
  scb5_selectionHPrior = 4;
  tft.setCursor(scbe_currentX, scbe_currentY);
  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  tft.print(scbe_sellectedChar);
  scb5_horizontalCountMax = scbe_CharHMax;
  scb5_verticalCountMax = scbe_CharVMax;
  scb_newMode = scbe_ModeChar;
  scb_mode = scb_newMode;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "_IndexPress B: scb_mode = ");
  itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbe_CharChange() {

#ifdef DEBUGIO
#ifdef Dscbe_CharChange
  strcpy(debug_output, "scb5_selectionV & scb5_selectionH = ");
  itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  scbe_sellectedChar = scbe_char[scb5_selectionV][scb5_selectionH];
  tft.setCursor(scbe_currentX, scbe_currentY);
  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  tft.print(scbe_sellectedChar);
  scb5_selectionVPrior = scb5_selectionV;
  scb5_selectionHPrior = scb5_selectionH;
}

void scbe_CharPress() {
  int i;

  if (scbe_sellectedChar == scbe_BackspaceChr) {
    if (scbe_currentIndex != 0) {
      i = scbe_currentIndex;
      while (i < scb_unsBlockSize) {
        scb_unsBlock[i - 1] = scb_unsBlock[i];
        i++;
      }
      scb_unsBlock[(scb_unsBlockSize-1)] = scbe_BlankChr;
      scbe_currentIndex--;
      scbe_currentX = scbe_currentIndex * scbe_incrementX;
      scbe_currentY = (scbe_currentX / scbe_maxX) * scbe_incrementY;
      scbe_currentX %= scbe_maxX;
    }
    scb5_selectionHPrior = scb5_selectionH;
    scb5_selectionVPrior = scb5_selectionV;
    scbe_displayBlock();

  } else if (scbe_sellectedChar == scbe_InsertChr) {
    i = scb_unsBlockSize-1;
    while (i >= scbe_currentIndex) {
      scb_unsBlock[i] = scb_unsBlock[i - 1];
      i--;
    }

    scb_unsBlock[scbe_currentIndex] = scbe_BlankChr;
    scbe_currentChar = scb_unsBlock[scbe_currentIndex];
    scb5_selectionHPrior = scb5_selectionH;
    scb5_selectionVPrior = scb5_selectionV;
    scbe_displayBlock();

  } else {
    scbe_currentChar = scbe_sellectedChar;
    scb_unsBlock[scbe_currentIndex] = scbe_currentChar;
  }

  tft.setCursor(scbe_currentX, scbe_currentY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.print(scbe_currentChar);
  // Switch to index mode.
#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
  Serial.println("Switch to index mode.");
#endif
#endif
  scb5_selectionH = scbe_currentIndex;
  scb5_selectionHPrior = scbe_currentIndex;
  scb5_horizontalCountMax = scb_unsBlockSize - 1;
  scb5_verticalCountMax = 0;
  scb_newMode = scbe_ModeIndex;
  scb_mode = scb_newMode;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "CharPress: scb_mode = ");
  itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
}

void scbe_LineChange() {

#ifdef DEBUGIO
#ifdef Dscbe_CharChange
  strcpy(debug_output, "scb5_selectionH = ");
  itoa(scb5_selectionH, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setCursor(0, (scb5_selectionVPrior*scbe_incrementY));
  strcpy(&scb_tempName[0], (char*)&scb_sBlock[scb5_selectionVPrior*80]);
  if (strlen(scb_tempName) > scb_TftMaxLineChars) {
    scb_tempName[scb_TftMaxLineChars-1] = 0;
  }
  tft.print(scb_tempName);
  tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
  tft.setCursor(0, (scb5_selectionV*scbe_incrementY));
  strcpy(&scb_tempName[0], (char*)&scb_sBlock[scb5_selectionV*80]);
  if (strlen(scb_tempName) > scb_TftMaxLineChars) {
    scb_tempName[scb_TftMaxLineChars-1] = 0;
  }
  tft.print(scb_tempName);
  scb5_selectionVPrior = scb5_selectionV;
  
}

void scbe_LinePress() {
  
int i;
int indexCount = 0;

#ifdef DEBUGIO
#ifdef Dscbe_LinePress
  strcpy(debug_output, "scb5_selectionV & scb5_verticalCountMax = ");
  itoa(scb5_selectionV, &debug_output[strlen(debug_output)], 10);
  strcat(debug_output, " , ");
  itoa(scb5_verticalCountMax, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

  if (scb5_selectionV == scb5_verticalCountMax) {
    scb_mode = scbf_FileSkip;
#ifdef DEBUGIO
#ifdef Dscb_mode
    strcpy(debug_output, "LinePress A: scb_mode = ");
    itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
#endif
  } else if (scb5_selectionV == (scb5_verticalCountMax-1)) {  //New_File.txt"
    if (scbf_siFiles) {
      strcpy(scb_tempName, "ScrambleIn/");
    } else {
      strcpy(scb_tempName, "UnScrambleOut/");
    }
    strcat(scb_tempName, "New_File");
    itoa(scbf_NFCount, &scb_tempName[strlen(scb_tempName)], 10);
    scbf_NFCount++;
    strcat(scb_tempName, ".txt");
    scb_inFile = SD.open(scb_tempName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    i = 0;
    while (i < scb_unsBlockSize) {
      scb_unsBlock[i] = 0;
      i++;
    }
    scb5_selectionV = 0;
    scb_mode = scbe_ModeIndexEnter;
#ifdef DEBUGIO
#ifdef Dscb_mode
    strcpy(debug_output, "LinePress B: scb_mode = ");
    itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
#endif
  } else {
  if (scbf_siFiles) {
      strcpy(scb_tempName, "ScrambleIn/");
    } else {
      strcpy(scb_tempName, "UnScrambleOut/");
    }
    strcat(scb_tempName, (char*)&scb_sBlock[scb5_selectionV*80]);
    scb_inFile = SD.open(scb_tempName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
#ifdef DEBUGIO
  if (!scb_inFile.available()) {
      Serial.println("FileSI open error!");
      while(1){}
    }
#endif
    indexCount = scb_inFile.read(&scb_unsBlock[0], scb_unsBlockSize);
    scb_unsBlock[scb_unsBlockSize] = 0;  //ending null for strlen().
    if (indexCount < scb_unsBlockSize) {
    i = indexCount;
      while (i < scb_unsBlockSize) {
        scb_unsBlock[i] = 0;
        i++;
      }
    }
#ifdef DEBUGIO
  if (indexCount < 0) {
      Serial.println("FileSI read error!");
      while(1){}
    }
#endif
    scb5_selectionV = 0;
    scb_mode = scbe_ModeIndexEnter;
#ifdef DEBUGIO
#ifdef Dscb_mode
    strcpy(debug_output, "LinePress B: scb_mode = ");
    itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
#endif
  }

}

void scbe_Indexkey3CPress() {

#ifdef DEBUGIO
#ifdef Dscb5_5WSwitch
  Serial.println("Index exited.");
#endif
#endif
  scbe_postProsBlock();
  scb_inFile.seek(0);
  scb_inFile.write(&scb_unsBlock[0], strlen((const char*)scb_unsBlock));
  scb_inFile.close();
  scb_mode = scbf_FileExit;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "scbe_Indexkey3CPress A: scb_mode = ");
  itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif
  
}
  
/*** ... END SCB EDITOR CODE ***/

void setup() {

#ifdef DEBUGIO
  // Monitor I/O:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect.
  }
  delay(1000);
#endif

  // Display Setup:
  tft.init();
  tft.setRotation(3);

  // SD CARD Setup:
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
    tft.setCursor(0, 0);
    tft.setTextSize(2);  // 16x8 non-mono-spaced font. Numbers are all the same size!
    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    tft.print(" SDCARD initialization    failed! ");
    while (1) { delay(1000); }
  }

#ifdef DEBUGIO
#ifdef Doutput256Randoms
output256Randoms();
while(1);
#endif
#endif

#ifdef DEBUGIO
#ifdef DoutputscbBytes
outputscbBytes();
while(1);
#endif
#endif

// 5-way-switch setup:
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  // 3-keys setup:
  //pinMode(WIO_KEY_A, INPUT_PULLUP);
  //pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);



  // Initial Mode:
  scb_mode = scbf_FileEnter;
  scbf_siFiles = true;
#ifdef DEBUGIO
#ifdef Dscb_mode
  strcpy(debug_output, "setup: scb_mode = ");
  itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
  Serial.println(debug_output);
#endif
#endif

}

void loop() {

  // Overwrite and delete all files in UnScrambleOut:
  scbo_OverwriteFiles();
  delay(500);

  while (1) {  // Until scb_ProgramExit.

#ifdef DEBUGIO
#ifdef Dscb_mode
    strcpy(debug_output, "loop: scb_mode = ");
    itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
    Serial.println(debug_output);
#endif
#endif
    switch (scb_mode) {

      case scbf_FileEnter:
        scb5_Reset();
        scbf_processFiles();
        scbe_preProsBlock();
        if (scb_mode != scbf_FileSkip) { //Avoid sperious display on scbf_FileSkip!
          scbe_displayBlock();
        }

#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "FileSIEnter: scb_mode = ");
        itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbe_ModeIndexEnter:
        scbe_Reset();
        scbe_currentChar = scb_unsBlock[0];
        scb_mode = scbe_ModeIndex;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "modeIndexEnter: scb_mode = ");
        itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbe_ModeIndex:
        scb5_horizontalCountMax = scb_unsBlockSize - 1;
        scb5_verticalCountMax = 0;
        scb_newMode = scb_mode;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "modeIndex: scb_newMode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        scb5_run5WaySwitch();
        break;

      case scbf_FileExit:
          scb_mode = scbf_FileEnter;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "FileSIEnter: scb_newMode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbf_FileSkip:
        if (scbf_siFiles) {
          // Needed for some reason so that "SD.open()" in the next mode will return from it's call!
          SD.end();
          SD.begin(SDCARD_SS_PIN, SDCARD_SPI);
          // end-of-Needed...
          scb_mode = scbd_ModeDigit10Enter;
        } else {
          scb_mode = scb_ProgramExit;
        }
        scb_newMode = scb_mode;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "FileSkip: scb_newMode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbd_ModeDigit10Enter:
        scb_digit10 = 0;
        scbd_Reset();
        scbs_Reset();
        scb_mode = scbd_ModeDigit10;
        scb_newMode = scb_mode;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "scbd_ModeDigit10Enter: scb_mode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbd_ModeDigit10:
        scbd_DrawKeyPad();
        scb5_verticalCountMax = 3;
        scb5_horizontalCountMax = 3;
        scb5_selectionV = 1;
        scb5_selectionVPrior = scb5_selectionV;
        scb5_selectionH = 1;
        scb5_selectionVPrior = scb5_selectionH;
        scb5_run5WaySwitch();
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "scbd_ModeDigit10: scb_mode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbd_ModeDigit10M:
#ifdef DEBUGIO
        strcpy(debug_output, "scb_digit10 = ");
        itoa(scb_digit10, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
        scbs_Mutate();
        scb_digit10 = 0;
        scbd_Reset();
        scb_mode = scbd_ModeDigit10;
        scb_newMode = scb_mode;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "scbd_ModeDigit10M: scb_mode = ");
        itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbd_ModeDigit10E:
        scb_mode = scbf_FileEnter;
        scbf_siFiles = false;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "scbd_ModeDigit10E: scb_newMode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbd_ModeDigit10Q:
        scb_mode = scbf_FileEnter;
        scbf_siFiles = false;
#ifdef DEBUGIO
#ifdef Dscb_mode
        strcpy(debug_output, "scbd_ModeDigit10Q: scb_newMode = ");
        itoa(scb_newMode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
#endif
        break;

      case scbs_ModeScramble:
#ifdef DEBUGIO
        strcpy(debug_output, "scb_digit10 = ");
        itoa(scb_digit10, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
#endif
        scbs_scramble();
        scb_mode = scbf_FileEnter;
        scbf_siFiles = false;
        break;

      case scb_ProgramExit:
#ifdef DEBUGIO
        Serial.println(" *** Finished! *** ");
#endif

        tft.fillRect(0, 0, 320, 240, TFT_DARKGREY);
        tft.setCursor(0, 0);
        tft.setTextSize(2); // 16x8 non-mono-spaced font. Numbers are all the same size!
        tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
        tft.println(" *** Finished! *** ");
        while (1) {delay(10);}
        break;

#ifdef DEBUGIO
      default:
        Serial.println("Loop Error!");
        strcpy(debug_output, "scb_mode = ");
        itoa(scb_mode, &debug_output[strlen(debug_output)], 10);
        Serial.println(debug_output);
        while (1) {}
#endif

    }

  }

}