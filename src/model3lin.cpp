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

#include "model3lin.h"
uint8_t TenCount=0;
bool read = true;
uint8_t readalt = 0;

 void Model3Lin::SetLinInterface(LinBus* l)
 {
    lin = l;
    DigIo::lin_wake.Clear();//Not used on TJA1027
    DigIo::lin_nslp.Set();//Wakes the device
    //Johannes for president!

 }

 void Model3Lin::SetSpeed(uint8_t pumpSpeed)
 {
    TenCount++;
    if(TenCount==1)//slow down to 20ms as this is called in 10ms task.
    {
    TenCount=0;


   if (lin->HasReceived(42, 8))//0x2A hex address
   {
      uint8_t* data = lin->GetReceivedBytes();

      Param::SetInt(Param::tmpoil, data[3]-40);//Motor oil temperature
      Param::SetFloat(Param::oilpres, (data[2]*2)*0.14503);//Motor oil pressure in psi

   }
   else if (lin->HasReceived(48, 8))//0x30 hex address
   {
      uint8_t* data = lin->GetReceivedBytes();

      Param::SetFloat(Param::upmp, data[0]*0.1);//Oil pump 12V supply Voltage.
      Param::SetInt(Param::pmprev, (data[5]<<8)|(data[4]));//Oil pump RPM

   }

   if (read)
   {
      if (readalt==10)  lin->Request(42, 0, 0);
      if (readalt==20)  lin->Request(48, 0, 0);
   }
   else
   {
      uint8_t lindata[2];
      lindata[0] = 0x11;
      lindata[1] = pumpSpeed;//send pump speed command.
      lin->Request(10, lindata, sizeof(lindata));//0x0A hex address for Pump speed command
   }

   read = !read;//ping - pong read and send.

   if(readalt>31) readalt=0;
   readalt++;


   }

 }
