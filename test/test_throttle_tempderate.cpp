/*
 * This file is part of the tumanako_vc project.
 *
 * Copyright (C) 2010 Johannes Huebner <contact@johanneshuebner.com>
 * Copyright (C) 2010 Edward Cheeseman <cheesemanedward@gmail.com>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

/* Unit tests for Throttle::TemperatureDerate to show how the soft limiting
 of the throttle and regen works as temperatures approach the configured
 limits. */

#include "test.h"
#include "throttle.h"

class ThrottleDerateTest : public UnitTest
{
public:
   ThrottleDerateTest(const std::list<VoidFunction>* cases) : UnitTest(cases)
   {
   }
};

static void TempDerateAtNormalTempLeavesThrottleSetting()
{
   float throttlePercent = 75.0f;

   bool derate = Throttle::TemperatureDerate(40.0f, 99.0f, throttlePercent);

   ASSERT(throttlePercent == 75.0f);
   ASSERT(derate == false);
}

static void TempDerateAtNormalTempLeavesRegenSetting()
{
   float finalSpnt = -50.0f;

   bool derate = Throttle::TemperatureDerate(0.0f, 20.0f, finalSpnt);

   ASSERT(finalSpnt == -50.0f);
   ASSERT(derate == false);
}

static void OutOfRangeThrottlePercentageClamps()
{
   // This should never happen
   float finalSpnt = 150.0f;

   bool derate = Throttle::TemperatureDerate(90.0f, 100.0f, finalSpnt);

   ASSERT(finalSpnt == 100.0f);
   ASSERT(derate == false);
}

static void TempAtOrAboveTempMax_ClampsThrottleToZero()
{
   float finalSpnt = 25.0f;

   bool derate = Throttle::TemperatureDerate(100.0f, 100.0f, finalSpnt);

   ASSERT(finalSpnt == 0.0f);
   ASSERT(derate == true);
}

static void TempAtOrAboveTempMax_ClampsRegenToZero()
{
   float finalSpnt = -30.0f;

   bool derate = Throttle::TemperatureDerate(101.0f, 100.0f, finalSpnt);

   ASSERT(finalSpnt == 0.0f);
   ASSERT(derate == true);
}

static void TempApproachingLimitReducesThrottle()
{
   float throttlePercent = 80.0f;

   bool derate = Throttle::TemperatureDerate(98.0f, 99.0f, throttlePercent);

   ASSERT(throttlePercent == 10.0f);
   ASSERT(derate == true);
}

static void TempApproachingLimitReducesRegen()
{
   float finalSpnt = -80.0f;

   bool derate = Throttle::TemperatureDerate(95.0f, 100.0f, finalSpnt);

   ASSERT(finalSpnt == -50.0f);
   ASSERT(derate == true);
}

REGISTER_TEST(
   ThrottleDerateTest,
   TempDerateAtNormalTempLeavesThrottleSetting,
   TempDerateAtNormalTempLeavesRegenSetting,
   OutOfRangeThrottlePercentageClamps,
   TempAtOrAboveTempMax_ClampsThrottleToZero,
   TempAtOrAboveTempMax_ClampsRegenToZero,
   TempApproachingLimitReducesThrottle,
   TempApproachingLimitReducesRegen);
