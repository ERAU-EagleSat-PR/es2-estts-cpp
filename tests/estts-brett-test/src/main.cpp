

#include <functional>
#include "constants.h"
#include "ax25_package.h"
#include "ax25_data.h"
#include "esttc_command.h"

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    esttc_command cmd("r","11","11","test");
    std::string temp = cmd.buildCommand();
    std::cout << temp;
    ax25_data axData(temp);
    temp = axData.build_data();
    std::cout << temp;
    ax25_package axPack(temp);
    temp = axPack.buildFrame();
    std::cout << temp;

    /******************************************************************************************************************/

    return 0;
}