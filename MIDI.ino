
void getConfig( byte *data, unsigned int mlength) {
  if ((data[0] == 0xF0) && (data[1] == 0x01) && (data[2] == 0x25) && (data[3] == 0x0F) && (data[mlength-1] == 0xF7)) {
    midiChannel = data[8];
    midiClock = data[10];
    MIDIout.disconnectCallbackFromType(SystemExclusive);                    // disconnect function
    MIDIout.setHandleSystemExclusive(OnSysExOut);                           // attach real callback
  }
}

void clockTick() {
  if (!(clockCounter % 24)) {
    if (midiClock) {
      if (digitalRead(CLOCK_LED)) digitalWrite(CLOCK_LED, LOW);
      else analogWrite(CLOCK_LED, 150);
    }
    clockCounter = 0;
  }
  clockCounter++;
}



//
// USB communication
//
void OnUsbNoteOn(byte channel, byte note, byte velocity) {                  // input from the USB port, trigger LED and forward to the din input
  MIDIin.sendNoteOn(note, velocity, channel);
  if (channel == midiChannel) {
    if (velocity) {
      midiLastNote = note;
      analogWrite(NOTE_LED, velocity<<1);
    } else if (note == midiLastNote) analogWrite(NOTE_LED, LOW);    
  }
}

  
void OnUsbNoteOff(byte channel, byte note, byte velocity) {                 // input from the USB port, trigger LED and forward to the din input
  MIDIin.sendNoteOff(note, velocity, channel);
  if ((channel == midiChannel) && (note == midiLastNote)) digitalWrite(NOTE_LED, LOW);
}

  
void OnUsbPitchBend(byte channel, int pitch) {
  MIDIin.sendPitchBend(pitch, channel);  
}

void OnUsbControlChange(byte channel, byte control, byte value) {
  MIDIin.sendControlChange(control, value, channel);
}


void OnUsbAfterTouchCh(byte channel, byte value) {
  MIDIin.sendAfterTouch(value, channel);
}


void OnUsbProgramChange(byte channel, byte number) {
  MIDIin.sendProgramChange(number, channel);  
}


void OnUsbRealTimeSystem(byte realtimebyte) { 
  if (realtimebyte == Clock) {
    MIDIin.sendRealTime(Clock);
    clockTick();
  }
  else if (realtimebyte == Start) MIDIin.sendRealTime(Start);
  else if (realtimebyte == Continue) MIDIin.sendRealTime(Continue);
  else if (realtimebyte == Stop) MIDIin.sendRealTime(Stop);
  else if (realtimebyte == ActiveSensing) MIDIin.sendRealTime(ActiveSensing);
  else if (realtimebyte == SystemReset) MIDIin.sendRealTime(SystemReset);  
}

  
void OnUsbTimeCode(uint16_t value) {
  MIDIin.sendTimeCodeQuarterFrame(value);  
}
  

void OnUsbSysEx(const uint8_t *data, uint16_t mlength,  bool complete) {
  if (sysExLength + mlength < 2048) {
    memcpy(sysExData + sysExLength, data, mlength);
    sysExLength += mlength;
    if (complete) {        // [0] will be 0xF0; [sys_ex_length-1] will be 0xF7
      MIDIin.sendSysEx(sysExLength, sysExData, true);
      sysExLength = 0;      
    }
  } else {
    // SysEx message is largre than buffer
    // ... not implemented yet
  }
}



//
// MIDI IN
//
void OnNoteOnIn(byte channel, byte note, byte velocity) {                    // input from the DIN input, trigger LED
  if (channel == midiChannel) {
    if (velocity) {
      midiLastNote = note;
      analogWrite(NOTE_LED, velocity<<1);
    } else if (note == midiLastNote) analogWrite(NOTE_LED, LOW);    
  }
}


void OnNoteOffIn(byte channel, byte note, byte velocity) {                    // input from the DIN input, trigger LED
  if ((channel == midiChannel) && (note == midiLastNote)) digitalWrite(NOTE_LED, LOW);
}


void OnClockIn() {
  if ((midiClock == 2) || (midiClock == 3)) clockTick();  
}
  


//
// MIDI OUT
//
void OnControlChangeOut(byte channel, byte control, byte value) {             // output on DIN, forward to USB
  if ((control == 99) && (value == 3)) {                                      // is a new MIDI channel or clock set on the Mopho?
    nrpn.channel = channel;
    nrpn.message[0] = value;                         
  } else if ((nrpn.channel == channel) && (nrpn.message[0] == 3) && (control == 98) && (value == 2)) nrpn.message[1] = value;
  else if ((nrpn.channel == channel) && (nrpn.message[0] == 3) && (nrpn.message[1] == 2) && (control == 6) && (value == 0)) nrpn.message[2] = value;
  else if ((nrpn.channel == channel) && (nrpn.message[0] == 3) && (nrpn.message[1] == 2) && (nrpn.message[2] == 0) && (control == 38)) midiChannel = value;    // new midi channel: 99-3, 98-2, 6-0, 38-channel

  else if ((channel == midiChannel) && (control == 99) && (value == 3)) nrpn.message[0] = value;               
  else if ((channel == midiChannel) && (nrpn.message[0] == 3) && (control == 98) && (value == 4)) nrpn.message[1] = value;
  else if ((channel == midiChannel) && (nrpn.message[0] == 3) && (nrpn.message[1] == 4) && (control == 6) && (value == 0)) nrpn.message[2] = value;
  else if ((channel == midiChannel) && (nrpn.message[0] == 3) && (nrpn.message[1] == 4) && (nrpn.message[2] == 0) && (control == 38)) midiClock = value;       // new midi clock setting: 99-3, 98-4, 6-0, 38-clock
  else {
    nrpn.channel = 0;
    memset(nrpn.message, 0, sizeof(nrpn.message));
  }
  usbMIDI.sendControlChange(control, value, channel);
}


void OnProgramChangeOut(byte channel, byte number) {                          // output on DIN, forward to USB
  usbMIDI.sendProgramChange(number, channel);  
}


static void OnSysExOut( byte *data, unsigned int mlength) {
  usbMIDI.sendSysEx(mlength, data);
}

void OnClockOut() {
  if (midiClock == 1) clockTick();  
}
