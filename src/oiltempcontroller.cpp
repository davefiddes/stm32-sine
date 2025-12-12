/*
 * This file is part of the stm32-sine project.
 *
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
 */

#include "oiltempcontroller.h"
#include "my_math.h"
#include "params.h"

//! \brief Maximum permitted oil pump speed
static const uint8_t MaxPumpSpeed = 255;

/**
 * \brief Initialise the oil temperature controller
 */
OilTempController::OilTempController() : m_currentSpeed(0)
{
}

/**
 * \brief Return the current pump speed
 */
uint8_t OilTempController::PumpSpeed() const
{
   return m_currentSpeed;
}

/**
 * \brief Update the oil temperature controller state
 */
void OilTempController::Ms100Task()
{
   int16_t tmpoil = Param::GetInt(Param::tmpoil);
   int16_t tmpoilhigh = Param::GetInt(Param::tmpoilhigh);
   int16_t tmpoillow = Param::GetInt(Param::tmpoillow);
   uint8_t pumpspeed = Param::GetInt(Param::pumpspeed);
   uint8_t pumpspeedidle = Param::GetInt(Param::pumpspeedidle);

   // Fix possible misconfiguration of temperature limits in such a way that
   // the pump runs faster
   tmpoillow = MIN(tmpoillow, tmpoilhigh);

   // Determine desired pump speed based on oil temperature and operation mode
   if (Param::GetInt(Param::opmode) == MOD_OFF)
   {
      m_currentSpeed =
         ComputePumpSpeed(tmpoil, tmpoillow, tmpoilhigh, pumpspeedidle);
   }
   else
   {
      m_currentSpeed =
         ComputePumpSpeed(tmpoil, tmpoillow, tmpoilhigh, pumpspeed);
   }
}

/**
 * \brief Compute the desired pump speed based on oil temperature
 *
 * \param tmpoil Current oil temperature
 * \param tmpoillow Lower temperature threshold
 * \param tmpoilhigh Upper temperature threshold
 * \param pumpspeedlower Pump speed at lower temperature threshold
 *
 * \return Desired pump speed
 */
uint8_t OilTempController::ComputePumpSpeed(
   int16_t tmpoil,
   int16_t tmpoillow,
   int16_t tmpoilhigh,
   uint8_t pumpspeedlower)
{
   if (tmpoil >= tmpoilhigh)
   {
      return MaxPumpSpeed;
   }
   else if (tmpoil <= tmpoillow)
   {
      return pumpspeedlower;
   }
   else
   {
      // Linear interpolation between lower and upper speed
      float ratio = float(tmpoil - tmpoillow) / float(tmpoilhigh - tmpoillow);
      return pumpspeedlower + uint8_t(ratio * (MaxPumpSpeed - pumpspeedlower));
   }
}
