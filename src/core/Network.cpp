
/**
 * @file Network.cpp
 * @author Matthias Hertel (https://www.mathertel.de)
 *
 * @brief The Network class implements the connection of a device to different Access Points (networks).
 *
 * @copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 * This work is licensed under a BSD 3-Clause style license, see https://www.mathertel.de/License.aspx
 *
 * Changelog: see Network.h
 */

#include <Arduino.h>
#include <HomeDing.h>
#include <core/Network.h>


Network::NETSTATE Network::state = NETSTATE::NONET;

// End.
