//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H

#include <mutex>
#include "ti_esttc.h"
#include "ti_socket_handler.h"

class transmission_interface : virtual public ti_esttc, virtual public ti_socket_handler {
private:

    std::mutex mtx;

    estts::Status initialize_ti();

    estts::Status check_ti_health();

    bool session_active;

    bool stream_active;

public:
    explicit transmission_interface();

    ~transmission_interface();

    estts::Status transmit(const std::string &value);

    std::string receive();

    estts::Status request_new_session(const std::string& handshake);

    bool check_session_active() const { return session_active; };

    estts::Status end_session(const std::string& end_frame);

    bool check_data_available();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H
