// Mopho Morphosis by Karg (Timm Schlegelmilch)
// DSI Mopho Tweak
//
// Detailed instructions are published on:
// http://karg-music.blogspot.de
//
// Licenced under Creative Commons Attribution-ShareAlike 4.0
// http://creativecommons.org/licenses/by-sa/4.0/

// To change device name, go to (Arduino)/Contents/Java/hardware/teensy/avr/cores/teensy3/usb_desc.h
//  and change: 
//    #define VENDOR_ID   0x16C0
//    #define PRODUCT_ID    0x0485
//    #define MANUFACTURER_NAME {'T','e','e','n','s','y','d','u','i','n','o'}
//    #define MANUFACTURER_NAME_LEN 11
//    #define PRODUCT_NAME    {'T','e','e','n','s','y',' ','M','I','D','I'}
//    #define PRODUCT_NAME_LEN  11
//
//  to:
//    #define VENDOR_ID   0x16C0
//    #define PRODUCT_ID    0x03E9
//    #define MANUFACTURER_NAME {'D','a','v','e','S','m','i','t','h','I','n','s','t','r','u','m','e','n','t','s'}
//    #define MANUFACTURER_NAME_LEN 20
//    #define PRODUCT_NAME    {'M','o','p','h','o'}
//    #define PRODUCT_NAME_LEN  5


#include <MIDI.h>

using namespace midi;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIin);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDIout);



// pin definitions
#define      CLOCK_LED          8
#define      NOTE_LED           7

unsigned int clockCounter     = 0;
byte         midiChannel      = 0;        // MIDI channel set on the Mopho
byte         midiClock        = 0;        // use internal or external clock
byte         midiThru         = 0;        // pass on all MIDI data?
byte         midiLastNote     = 0;

int          sysExLength      = 0;
byte         sysExData[2048];

struct NRPN {
  byte       channel;
  byte       message[4];
};
NRPN         nrpn;

