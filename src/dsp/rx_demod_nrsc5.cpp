#include <iostream>
#include <dsp/rx_demod_nrsc5.h>
#include <nrsc5_rx/nrsc5_rx.h>

/* Create a new instance of rx_demod_nrsc5 and return a shared_ptr. */
rx_demod_nrsc5_sptr make_rx_demod_nrsc5()
{
    return gnuradio::get_initial_sptr(new rx_demod_nrsc5());
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
    nrsc5_rx = gr::nrsc5_rx::nrsc5_rx::make(0, true);

    connect(self(), 0, type_converter, 0);
    connect(type_converter, 0, nrsc5_rx, 0);
    connect(nrsc5_rx, 0, self(), 0);
    connect(nrsc5_rx, 1, self(), 1);
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
