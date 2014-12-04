// Copyright (C) 2012 Mark R. Stevens
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#if (ARDUINO >= 100)
  #include "Arduino.h"
#endif
#include "Encoders.h"


        static volatile long counts1 = 0;
        static volatile long counts2 = 0;

        static volatile long last1 = 0;
        static volatile long last2 = 0;
        extern int direccionI;
        extern int direccionD;
        // Interrupt based on wheel encoder
        //
        ISR(PCINT0_vect)
        {

          // Get the value at the pins
          //
          const long m1 = (PINB >> 3) & 1;
          const long m2 = (PINB >> 2) & 1;

          // Determine which pin to update
          //
          if (m1 ^ last1)
          {
            if (direccionI == 1) 
              counts1 += 1;
            else if (direccionI == 2)
              counts1 -= 1;
          }
          if (m2 ^ last2)
          {
            if (direccionD == 1)
              counts2 += 1;
            else if (direccionD == 2)
              counts2-=1;
          }

          // reset
          //
          last1 = m1;
          last2 = m2;
        }

        ISR(PCINT1_vect,ISR_ALIASOF(PCINT0_vect));
        ISR(PCINT2_vect,ISR_ALIASOF(PCINT0_vect));

        long Encoders::getCountsAndResetM1()
        {
          noInterrupts();
          const long tmp = counts1;
          counts1 = 0;
          interrupts();
          return tmp;
        }

        long Encoders::getCountsAndResetM2()
        {
          noInterrupts();
          const long tmp = counts2;
          counts2 = 0;
          interrupts();
          return tmp;
        }

        long Encoders::getCountsM1(){
          return counts1;
        }

        long Encoders::getCountsM2(){
          return counts2;
        }

        void Encoders::resetM1(){
          counts1 = 0;
        }

        void Encoders::resetM2(){
          counts2 = 0;
        }

        float convert(long count)
        {
          static const float ppr = 20.0f;
          static const float pi = 3.1416f;
          static const float diameter = 0.0635f;
          static const float circumference = pi * diameter;
          return circumference * count / ppr;
        }
      
      // Encoders::Encoders()
      // {
      //   this->setup();
      // }
      void Encoders::setup()
      {

        // Make sure the pin is set for input
        //
        ::pinMode(LeftWheelEncoder, INPUT);
        ::pinMode(RightWheelEncoder, INPUT);

        // Enable the pull-up resistor
        //
        ::digitalWrite(LeftWheelEncoder, HIGH);
        ::digitalWrite(RightWheelEncoder, HIGH);

        // Prevent interrupts while initializing
        //
        noInterrupts();

        // Pin 50 for left wheel
        //
        PCICR |= 1 << PCIE0;
        DDRB &= ~(1 << (3));
        PCMSK0 |= 1 << (3);

        // Pin 51 for right wheel
        //
        PCICR |= 1 << PCIE0;
        DDRB &= ~(1 << (2));
        PCMSK0 |= 1 << (2);

        // Clear the interrupt flags in case they were set before for any reason.
        // On the AVR, interrupt flags are cleared by writing a logical 1
        // to them.
        //
        PCIFR |= (1 << PCIF0) | (1 << PCIF1) | (1 << PCIF2);

        // re-enable interrupts
        //
        interrupts();
      }
      float Encoders::left()
      {
        return convert(getCountsAndResetM1());
      }
      float Encoders::right()
      {
        return convert(getCountsAndResetM2());
      }

