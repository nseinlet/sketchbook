/*Copyright (C) 2015  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/
 
#include <PinChangeInt.h>
#include <Servo.h>

// Entrées
#define ACCEL_IN_PIN 5
#define VIRE_IN_PIN 4
#define LEVAGE_IN_PIN 3

// Sorties
#define CHDR_OUT_PIN 6
#define CHGA_OUT_PIN 7
#define LEVDR_OUT_PIN 8
#define LEVGA_OUT_PIN 9

// Servos
Servo chenilleDroite;
Servo chenilleGauche;
Servo levageDroite;
Servo levageGauche;

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define ACCEL_FLAG 1
#define VIRE_FLAG 2
#define LEV_FLAG 4

// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the 
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t unThrottleInShared;
volatile uint16_t unSteeringInShared;
volatile uint16_t unAuxInShared;

// These are used to record the rising edge of a pulse in the calcInput functions
// They do not need to be volatile as they are only used in the ISR. If we wanted
// to refer to these in loop and the ISR then they would need to be declared volatile
uint32_t ulThrottleStart;
uint32_t ulSteeringStart;
uint32_t ulAuxStart;

void setup()
{
  // attach servo objects, these will generate the correct 
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 
  chenilleDroite.attach(CHDR_OUT_PIN);
  chenilleGauche.attach(CHGA_OUT_PIN);
  levageDroite.attach(LEVDR_OUT_PIN);
  levageGauche.attach(LEVGA_OUT_PIN);

  // using the PinChangeInt library, attach the interrupts
  // used to read the channels
  PCintPort::attachInterrupt(ACCEL_IN_PIN, calcThrottle,CHANGE); 
  PCintPort::attachInterrupt(VIRE_IN_PIN, calcSteering,CHANGE); 
  PCintPort::attachInterrupt(LEVAGE_IN_PIN, calcAux,CHANGE); 
}

void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained 
  // between calls to loop.
  static uint16_t unThrottleIn;
  static uint16_t unSteeringIn;
  static uint16_t unAuxIn;
  // local copy of update flags
  static uint8_t bUpdateFlags;

  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;
    
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
    
    if(bUpdateFlags & ACCEL_FLAG)
    {
      unThrottleIn = unThrottleInShared;
    }
    
    if(bUpdateFlags & VIRE_FLAG)
    {
      unSteeringIn = unSteeringInShared;
    }
    
    if(bUpdateFlags & LEV_FLAG)
    {
      unAuxIn = unAuxInShared;
    }
     
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the 
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }
  
  // do any processing from here onwards
  // only use the local values unAuxIn, unThrottleIn and unSteeringIn, the shared
  // variables unAuxInShared, unThrottleInShared, unSteeringInShared are always owned by 
  // the interrupt routines and should not be used in loop
  
  // the following code provides simple pass through 
  // this is a good initial test, the Arduino will pass through
  // receiver input as if the Arduino is not there.
  // This should be used to confirm the circuit and power
  // before attempting any custom processing in a project.
  
  // we are checking to see if the channel value has changed, this is indicated  
  // by the flags. For the simple pass through we don't really need this check,
  // but for a more complex project where a new signal requires significant processing
  // this allows us to only calculate new values when we have new inputs, rather than
  // on every cycle.
  if(bUpdateFlags & ACCEL_FLAG)
  {
    if(chenilleDroite.readMicroseconds() != unThrottleIn)
    {
      chenilleDroite.writeMicroseconds(unThrottleIn);
    }
  }
  
  if(bUpdateFlags & VIRE_FLAG)
  {
    if(chenilleGauche.readMicroseconds() != unSteeringIn)
    {
      chenilleGauche.writeMicroseconds(unSteeringIn);
    }
  }
  
  if(bUpdateFlags & LEV_FLAG)
  {
    if(levageDroite.readMicroseconds() != unAuxIn)
    {
      levageDroite.writeMicroseconds(unAuxIn);
    }
  }
  
  bUpdateFlags = 0;
}


// simple interrupt service routine
void calcThrottle()
{
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(ACCEL_IN_PIN) == HIGH)
  { 
    ulThrottleStart = micros();
  }
  else
  {
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    unThrottleInShared = (uint16_t)(micros() - ulThrottleStart);
    // use set the throttle flag to indicate that a new throttle signal has been received
    bUpdateFlagsShared |= ACCEL_FLAG;
  }
}

void calcSteering()
{
  if(digitalRead(VIRE_IN_PIN) == HIGH)
  { 
    ulSteeringStart = micros();
  }
  else
  {
    unSteeringInShared = (uint16_t)(micros() - ulSteeringStart);
    bUpdateFlagsShared |= VIRE_FLAG;
  }
}

void calcAux()
{
  if(digitalRead(LEVAGE_IN_PIN) == HIGH)
  { 
    ulAuxStart = micros();
  }
  else
  {
    unAuxInShared = (uint16_t)(micros() - ulAuxStart);
    bUpdateFlagsShared |= LEV_FLAG;
  }
}
