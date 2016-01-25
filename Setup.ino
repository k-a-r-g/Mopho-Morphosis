void setup() {
  pinMode(CLOCK_LED, OUTPUT);
  pinMode(NOTE_LED, OUTPUT);
  analogWrite(CLOCK_LED, 150);
  digitalWrite(NOTE_LED, LOW);
  delay(200);
  digitalWrite(CLOCK_LED, LOW);
  analogWrite(NOTE_LED, 150);
  delay(200);
  analogWrite(CLOCK_LED, 150);
  digitalWrite(NOTE_LED, LOW);
  delay(200);
  digitalWrite(CLOCK_LED, LOW);
  
  // Midi messages for the Mopho to respond to
  usbMIDI.setHandleNoteOn(OnUsbNoteOn);
  usbMIDI.setHandleNoteOff(OnUsbNoteOff);
  usbMIDI.setHandlePitchChange(OnUsbPitchBend);
  usbMIDI.setHandleControlChange(OnUsbControlChange);
  usbMIDI.setHandleAfterTouch(OnUsbAfterTouchCh);   
  //usbMIDI.setHandleAfterTouchPoly(OnUsbAfterTouchPoly);     // not yet supported by USB MIDI library
  usbMIDI.setHandleProgramChange(OnUsbProgramChange);
  //usbMIDI.setHandleSongPosition(OnUsbSongPosition);         // not yet supported by USB MIDI library
  //usbMIDI.setHandleSongSelect(OnUsbSongSelect);             // not yet supported by USB MIDI library
  //usbMIDI.setHandleTuneRequest(OnUsbTuneRequest);           // not yet supported by USB MIDI library
  usbMIDI.setHandleRealTimeSystem(OnUsbRealTimeSystem);
  usbMIDI.setHandleTimeCodeQuarterFrame(OnUsbTimeCode);
  usbMIDI.setHandleSysEx(OnUsbSysEx);
   
  MIDIin.begin(MIDI_CHANNEL_OMNI);
  MIDIin.turnThruOff();
  MIDIin.setHandleNoteOn(OnNoteOnIn);
  MIDIin.setHandleNoteOff(OnNoteOffIn);
  MIDIin.setHandleClock(OnClockIn);

  // MIDI messages from the Mopho to forward to USB
  MIDIout.begin(MIDI_CHANNEL_OMNI);
  MIDIout.turnThruOff();
  MIDIout.setHandleControlChange(OnControlChangeOut);
  MIDIout.setHandleProgramChange(OnProgramChangeOut);
  MIDIout.setHandleClock(OnClockOut);

  
  // request gobal parameter dump
  MIDIout.setHandleSystemExclusive(getConfig);                      // attach config function
  byte getGlobalParameterDump[] = {0xF0, 0x01, 0x25, 0x0E, 0xF7 };  // construct SysEx message to get global parameters
  MIDIin.sendSysEx(5, getGlobalParameterDump, true);                // send it
}


