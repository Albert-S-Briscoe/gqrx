
#include "dsp/msg_store.h"

msg_store_sptr make_msg_store()
{
    return gnuradio::get_initial_sptr(new msg_store());
}


msg_store::msg_store() : gr::block ("msg_store",
                                gr::io_signature::make (0, 0, 0),
                                gr::io_signature::make (0, 0, 0))
{
        message_port_register_in(pmt::mp("store"));
        set_msg_handler(pmt::mp("store"), std::bind(&msg_store::store, this, std::placeholders::_1));
}

msg_store::~msg_store ()
{

}

void msg_store::store(pmt::pmt_t msg)
{
    std::lock_guard<std::mutex> lock(d_mutex);
    d_messages.push(msg);

}

void msg_store::get_message(std::vector<std::string> &out, int &type)
{
    std::lock_guard<std::mutex> lock(d_mutex);

    if (d_messages.size()>0) {
        pmt::pmt_t msg=d_messages.front();
        type=pmt::to_long(pmt::tuple_ref(msg,0));
		out.resize(1);
        out[0]=pmt::symbol_to_string(pmt::tuple_ref(msg,1));
        d_messages.pop();
    } else {
        type=-1;
    }
}
