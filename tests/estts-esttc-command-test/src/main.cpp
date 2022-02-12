//
// Created by Cody Park on 12/26/21.
//

#include <iostream>
#include <string>
#include "ti_esttc.h"

using namespace std;

int main() {
    ti_esttc esttc_object("/dev/ttyUSB0", 115200);

    string data1;
    string data2;
    string data3;

    //esttc_object.read_low_pwr_mode(mode);

    //cout << "The UHF transceiver's power mode is " << mode << " - \"00\" [Normal Mode] or \"01\" [Low Power Mode]" << endl;

    esttc_object.read_radio_freq(data1, data2, data3);
    return 0;
}