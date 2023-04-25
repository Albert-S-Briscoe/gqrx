#include <iostream>
#include <dsp/rx_demod_nrsc5.h>

#include "dsp/nrsc5_rx/nrsc5_rx.h"

/* Create a new instance of rx_demod_nrsc5 and return a shared_ptr. */
rx_demod_nrsc5_sptr make_rx_demod_nrsc5()
{
    return gnuradio::get_initial_sptr(new rx_demod_nrsc5());
}
rx_demod_nrsc5_store_sptr make_rx_demod_nrsc5_store()
{
    return gnuradio::get_initial_sptr(new rx_demod_nrsc5_store());
}

/*! \brief Create nrsc5 demodulator object.
 *
 * Use make_rx_demod_nrsc5() instead.
 */
rx_demod_nrsc5::rx_demod_nrsc5()
    : gr::hier_block2("stereo_demod",
        gr::io_signature::make (1, 1, sizeof (gr_complex)),
        gr::io_signature::make (2, 2, sizeof (float)))
{
    type_converter = gr::blocks::complex_to_interleaved_short::make(true, 32767);
    nrsc5_rx = gr::nrsc5_rx::nrsc5_rx::make(0);
    message_port_register_hier_out(pmt::mp("SIS"));

    connect(self(), 0, type_converter, 0);
    connect(type_converter, 0, nrsc5_rx, 0);
    connect(nrsc5_rx, 0, self(), 0);
    connect(nrsc5_rx, 1, self(), 1);

    msg_connect(nrsc5_rx, "out", self(), "SIS");
}

rx_demod_nrsc5::~rx_demod_nrsc5()
{

}

void rx_demod_nrsc5::set_program(int program)
{
    nrsc5_rx->set_program(program);
}

float rx_demod_nrsc5::get_level_db()
{
    if (nrsc5_rx->get_sync())
    {
        return 0.0;
    }
    return -100.0;
}



rx_demod_nrsc5_store::rx_demod_nrsc5_store() : msg_store()
{
}

rx_demod_nrsc5_store::~rx_demod_nrsc5_store ()
{
}

/*
void rx_demod_nrsc5_store::store(pmt::pmt_t msg)
{
    std::lock_guard<std::mutex> lock(d_mutex);
    d_messages.push(msg);
}
*/

void rx_demod_nrsc5_store::get_message(std::vector<std::string> &out, int &type)
{
    std::lock_guard<std::mutex> lock(d_mutex);
    if (d_messages.size()>0) {
        pmt::pmt_t msg=d_messages.front();
        type=pmt::to_long(pmt::tuple_ref(msg,0));

		size_t length = (type == 0) ? 6 : 4;
		out.resize(length);

        for (size_t i = 0; i < length; i++)
        {
            out[i] = pmt::symbol_to_string(pmt::tuple_ref(msg, i + 1));
        }
        d_messages.pop();
    } else {
        type=-1;
    }
}
