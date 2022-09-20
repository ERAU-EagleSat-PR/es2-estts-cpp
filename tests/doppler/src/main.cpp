//
// Created by Hayden Roszell on 9/12/22.
//
#include "../lib/sgp4/libsgp4/CoordTopocentric.h"
#include "../lib/sgp4/libsgp4/CoordGeodetic.h"
#include "../lib/sgp4/libsgp4/Observer.h"
#include "../lib/sgp4/libsgp4/SGP4.h"
#include "../lib/sgp4/libsgp4/Vector.h"
#include "../src/utils/constants.h"


#include <iostream>
#include <cmath>
#include <math.h>

int main() {

    //temp ground-station coordinates
    Observer obs(51.507406923983446, -0.12773752212524414, 0.05);

    //temp TLE value
    Tle tle = Tle("UK-DMC 2                ",
                  "1 35683U 09041C   12289.23158813  .00000484  00000-0  89219-4 0  5863",
                  "2 35683  98.0221 185.3682 0001499 100.5295 259.6088 14.69819587172294");

    SGP4 sgp4(tle);

    DateTime dt = tle.Epoch();

    Eci sat_eci = sgp4.FindPosition(dt);

    double sat_range_rate = obs.GetLookAngle(sat_eci).range_rate;

    double doppler435 = -435000000.0 *((sat_range_rate*1000.0)/299792458.0);

    double doppler = (435000000.0 + doppler435) / 1000000.0;

    std::cout << "Doppler Shift: " << doppler << std::endl;

    return 0;
}