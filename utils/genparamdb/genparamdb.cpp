/*
 * This file is part of the stm32-sine project.
 *
 * Copyright (C) 2022 David J. Fiddes <D.J@fiddes.net>
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
#include "my_string.h"
#include "param_prj.h"
#include <iostream>

static bool firstParam = true;

//
//! Format an openinverter value parameter as JSON. These are runtime only
//! values.
//
void PrintValue(const char* name, const char* unit, uint32_t id)
{
    if (firstParam)
    {
        firstParam = false;
    }
    else
    {
        std::cout << ",\n";
    }

    std::cout << "\t\"" << name << "\" : {"
              << "\"unit\":\"" << unit << "\","
              << "\"isparam\":false,"
              << "\"id\":\"" << id << "\"}";
}

//
//! Format an openinverter modifiable parameter as JSON. These parameters can be
//! saved and have default, min and max settings in addition to the basic
//! details of a value. The category is optional.
//
void PrintParam(
    const char* name,
    const char* unit,
    float       min,
    float       max,
    float       def,
    const char* category,
    uint32_t    id)
{
    if (firstParam)
    {
        firstParam = false;
    }
    else
    {
        std::cout << ",\n";
    }

    std::cout << "\t\"" << name << "\" : {"
              << "\"unit\":\"" << unit << "\","
              << "\"minimum\":\"" << min << "\","
              << "\"maximum\":\"" << max << "\","
              << "\"default\":\"" << def << "\","
              << "\"isparam\":true,"
              << "\"category\":\"" << category << "\","
              << "\"id\":\"" << id << "\"}";
}

//
//! Empty error string list to keep the linker happy
//
const char* errorListString = "Empty";

int main()
{
// Macro to add a parameter entry to the list
#define PARAM_ENTRY(category, name, unit, min, max, def, id)                   \
    PrintParam(#name, unit, min, max, def, category, id);

// Macro to add a value entry to the list
#define VALUE_ENTRY(name, unit, id) PrintValue(#name, unit, id);

    std::cout << "{\n";

    // Instantiate the list of parameters which will through pre-processor magic
    // put them all in the categories and values containers
    PARAM_LIST

    std::cout << "\n}" << std::endl;
}