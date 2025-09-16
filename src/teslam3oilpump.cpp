/*
 * This file is part of the stm32-sine project.
 *
 * Copyright (C) 2018 Johannes Huebner <dev@johanneshuebner.com>
 * Copyright (C) 2025 Damien Maguire <info@evbmw.com>
 * Copyright (C) 2025 David J. Fiddes <D.J@fiddes>
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
 * Controls the 12V electric oil pump in the Tesla Model 3 Drive unit and reads
 * back information.
 *
 * LIN bus speed is 19.2kbps and uses protocol 2.1.
 * Message interval is 20ms from Master (inverter) to Slave (pump) and vice
 * versa. Message ID 0x0A from Inverter to Pump 8 bits 0-255 commands pump
 * speed. 2 bytes. Speed command in byte 0.
 *
 * Pump responds on 0x2A
 * Oil temp on byte 3 offset -40 in degC.
 * Pump responds on 0x30
 * Pump supply voltage in byte 0 multiply by 0.1. VDC.
 * Pump motor RPM possible in bytes 4 and 5. 5=msb.
 */

#include "teslam3oilpump.h"
#include "digio.h"
#include "params.h"
#include "errormessage.h"
#include <libopencm3/stm32/usart.h>

// LIN protocol PID definitions

static const uint8_t SpeedRequestPID = 0x0A;
static const uint8_t SpeedRequestLen = 2;

static const uint8_t FlowPressureTempStatusPID = 0x2A;
static const uint8_t FlowPressureTempStatusLen = 8;

static const uint8_t VoltageSpeedStatusPID = 0x30;
static const uint8_t VoltageSpeedStatusLen = 8;

//! \brief Aim for a 100ms loop duration
static const uint8_t MaxLoopTicks = 10;

//! \brief Maximum time we are prepared to wait for a valid response from the
//! pump in 10ms ticks
static const uint16_t StatusTimeout = 500;

/**
 * \brief Initialise the oil pump controller
 */
TeslaM3OilPump::TeslaM3OilPump()
: lin(nullptr), tickCount(0), ticksSinceLastResponse(0)
{
}

/**
 * \brief Configure the LIN interface
 */
void TeslaM3OilPump::SetLinInterface(LinBus* l)
{
   lin = l;

   // Initialise the LIN UART
   lin->Init(UART4, 19200);

   // Turn on the LIN transceiver
   DigIo::lin_wake.Clear();
   DigIo::lin_nslp.Set();
}

/**
 * \brief Run the LIN state machine to send commands to the pump and retrieve
 * its status
 */
void TeslaM3OilPump::Ms10Task()
{
   ProcessStatusResponse();

   // Statically schedule requests. A 10ms tick is sufficient for a max length
   // LIN request to be processed
   switch (tickCount)
   {
   case 0:
      SendSpeedRequest();
      break;

   case 1:
      lin->Request(FlowPressureTempStatusPID, 0, 0);
      break;

   case 2:
      lin->Request(VoltageSpeedStatusPID, 0, 0);
      break;

   default:
      break;
   }

   tickCount++;
   if (tickCount >= MaxLoopTicks)
      tickCount = 0;

   CheckForFaults();
}

/**
 * \brief Send the currently configured static pump speed to the pump
 */
void TeslaM3OilPump::SendSpeedRequest()
{
   uint8_t lindata[SpeedRequestLen];
   lindata[0] = 0xFF;
   lindata[1] = Param::GetInt(Param::pumpspeed);
   lin->Request(SpeedRequestPID, lindata, sizeof(lindata));
}

/**
 * \brief Process status responses from the pump updating spot values as
 * required
 */
void TeslaM3OilPump::ProcessStatusResponse()
{
   if (lin->HasReceived(FlowPressureTempStatusPID, FlowPressureTempStatusLen))
   {
      uint8_t* data = lin->GetReceivedBytes();

      Param::SetInt(Param::tmpoil, data[3] - 40); // Motor oil temperature
      Param::SetFloat(
         Param::oilpres,
         (data[2] * 2) * 0.14503); // Motor oil pressure in psi
      ticksSinceLastResponse = 0;
   }
   else if (lin->HasReceived(VoltageSpeedStatusPID, VoltageSpeedStatusLen))
   {
      uint8_t* data = lin->GetReceivedBytes();

      Param::SetFloat(
         Param::upmp, data[0] * 0.1); // Oil pump 12V supply Voltage.
      Param::SetInt(Param::pmprev, (data[5] << 8) | (data[4])); // Oil pump RPM
      ticksSinceLastResponse = 0;
   }
}

/**
 * \brief Check to see if we are receiving timely status responses
 */
void TeslaM3OilPump::CheckForFaults()
{
   if (ticksSinceLastResponse > StatusTimeout)
   {
      ErrorMessage::Post(ERR_OILPUMPFAULT);

      // Set default values to indicate a fault condition
      Param::SetInt(Param::tmpoil, 0);
      Param::SetFloat(Param::oilpres, 0);
      Param::SetFloat(Param::upmp, 0);
      Param::SetInt(Param::pmprev, 0);
   }
   else
   {
      ticksSinceLastResponse++;
   }
}
