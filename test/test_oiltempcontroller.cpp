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

/* Unit tests for OilTempController to verify operation when idle, running and
 * at extremes of operation */

#include "oiltempcontroller.h"
#include "params.h"
#include "test.h"

class OilTempControllerTest : public UnitTest
{
public:
   OilTempControllerTest(const std::list<VoidFunction>* cases) : UnitTest(cases)
   {
   }

   void TestCaseSetup() override;
};

void OilTempControllerTest::TestCaseSetup()
{
   Param::LoadDefaults();

   // Configurable parameters for oil temp controller operation
   Param::SetInt(Param::tmpoilhigh, 60);
   Param::SetInt(Param::tmpoillow, 30);
   Param::SetInt(Param::pumpspeed, 55);
   Param::SetInt(Param::pumpspeedidle, 20);

   // Spot values
   Param::SetInt(Param::tmpoil, 25);
}

static void StartupHasZeroPumpSpeed()
{
   OilTempController otc;

   ASSERT(otc.PumpSpeed() == 0);
}

static void IdleSpeedIsLow()
{
   OilTempController otc;

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 20);
}

static void NormalOperatingSpeedWhenInRunMode()
{
   OilTempController otc;
   Param::SetInt(Param::opmode, MOD_RUN);

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 55);
}

static void MaxOperatingSpeedWhenHotInRunMode()
{
   OilTempController otc;
   Param::SetInt(Param::opmode, MOD_RUN);
   Param::SetInt(Param::tmpoil, 100);

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 255);
}

static void MaxOperatingSpeedWhenHotButSwitchedOff()
{
   OilTempController otc;
   Param::SetInt(Param::tmpoil, 100);

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 255);
}

static void MidOperatingSpeedWhenMiddleTempAndRunning()
{
   OilTempController otc;
   Param::SetInt(Param::opmode, MOD_RUN);
   Param::SetInt(Param::tmpoil, 45);

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 155);
}

static void MaxOperatingSpeedWhenTempLimitsConfiguredWrong()
{
   OilTempController otc;
   Param::SetInt(Param::tmpoil, 45);
   Param::SetInt(Param::tmpoilhigh, 30);
   Param::SetInt(Param::tmpoillow, 60);

   otc.Ms100Task();

   ASSERT(otc.PumpSpeed() == 255);
}

REGISTER_TEST(
   OilTempControllerTest,
   StartupHasZeroPumpSpeed,
   IdleSpeedIsLow,
   NormalOperatingSpeedWhenInRunMode,
   MaxOperatingSpeedWhenHotInRunMode,
   MaxOperatingSpeedWhenHotButSwitchedOff,
   MidOperatingSpeedWhenMiddleTempAndRunning,
   MaxOperatingSpeedWhenTempLimitsConfiguredWrong);
