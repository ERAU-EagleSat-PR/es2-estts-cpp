//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H

#include <mutex>
#include <thread>
#include "ti_esttc.h"
#include "ti_socket_handler.h"

class transmission_interface : virtual public ti_esttc, virtual public ti_socket_handler {
private:

    std::mutex mtx;

    estts::Status initialize_ti();

    estts::Status check_ti_health();

    bool stream_active;

    std::thread session_keeper;

    void maintain_pipe();

public:
    bool session_active;

    explicit transmission_interface();

    ~transmission_interface();

    estts::Status transmit(const std::string &value);

    estts::Status transmit(const unsigned char * value, int length);

    std::string receive();

    std::string nonblock_receive();

    unsigned char * receive_uc();

    estts::Status request_new_session();

    bool check_session_active() const { return session_active; };

    estts::Status end_session(const std::string& end_frame);

    bool check_data_available();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H
