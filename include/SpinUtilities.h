/*  YAP - Yet another PWA toolkit
    Copyright 2015, Technische Universitaet Muenchen,
    Authors: Daniel Greenwald, Johannes Rauch

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef yap_SpinUtilities_h
#define yap_SpinUtilities_h

#include <string>

namespace yap {

/// \brief Clebsch Gordan coefficients and related functions
/// \author Johannes Rauch, Daniel Greenwald
/// This code has been taken from rootpwa and modified

/// returns Clebsch-Gordan coefficient (j1 m1 j2 m2 | J M)
double clebschGordan(int two_j1, int two_m1, int two_j2, int two_m2, int two_J, int two_M);

/// checks that spin and its projection quantum number are consistent
bool spinAndProjAreCompatible(const int spin, const int spinProj);

/// returns, whether j1 and j2 can couple to J
bool spinStatesCanCouple(const int two_j1, const int two_j2, const int two_J);

/// convert 2*J to string (e.g. 1/2, 1, 3/2, etc.)
std::string spinToString(const int twoJ);

}

#endif
