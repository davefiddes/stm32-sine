/*
 * This file is part of the Zombieverter VCU project.
 *
 * Copyright (C) 2018 Johannes Huebner <dev@johanneshuebner.com>
 * Copyright (C) 2025 Damien Maguire <info@evbmw.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Controls the 12V electric oil pump in the Tesla Model 3 Drive unit and reads back information.
 *
 * LIN bus speed is 19.2kbps and uses protocol 2.1.
 * Message interval is 20ms from Master (inverter) to Slave (pump) and vice versa.
 * Message ID 0x0A from Inverter to Pump 8 bits 0-255 commands pump speed. 2 bytes. Speed command in byte 0.
 *
 * Pump responds on 0x2A
 * Oil temp on byte 3 offset -40 in degC.
 * Pump responds on 0x30
 * Pump supply voltage in byte 0 multiply by 0.1. VDC.
 * Pump motor RPM possible in bytes 4 and 5. 5=msb.
 */

 #ifndef MODEL3LIN_H
 #define MODEL3LIN_H

 #include <stdint.h>
 #include "linbus.h"
 #include "digio.h"
 #include "params.h"

 class Model3Lin
{
   public:
      void SetSpeed(uint8_t pumpSpeed);
      void SetLinInterface(LinBus* l);

   private:
      LinBus* lin;

};

 #endif // MODEL3LIN_H
