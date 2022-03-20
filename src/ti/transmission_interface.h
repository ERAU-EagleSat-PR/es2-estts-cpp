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

    std::function<estts::Status(std::string)> telem_cb;

    std::mutex mtx;

    bool stream_active;

    std::thread session_keeper;

    void maintain_pipe();

public:
    bool session_active;

    explicit transmission_interface();

    ~transmission_interface();

    void set_telem_callback(const std::function<estts::Status(std::string)>& cb) {telem_cb = cb;}

    estts::Status transmit(const std::string &value);

    estts::Status transmit(const unsigned char *value, int length);

    std::string receive();

    std::string nonblock_receive();

    unsigned char * receive_uc();

    estts::Status request_new_session();

    estts::Status request_new_session1();

    bool check_session_active() const { return session_active; };

    estts::Status end_session(const std::string& end_frame);

    bool check_data_available();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H
