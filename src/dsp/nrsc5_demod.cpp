#include <iostream>
#include <dsp/nrsc5_demod.h>
#include <nrsc5_rx/nrsc5_rx.h>

/* Create a new instance of stereo_demod and return a shared_ptr. */
nrsc5_demod_sptr make_nrsc5_demod()
{
    return gnuradio::get_initial_sptr(new nrsc5_demod());
}

/*! \brief Create nrsc5 demodulator object.
 *
 * Use make_nrsc5_demod() instead.
 */
nrsc5_demod::nrsc5_demod()
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

nrsc5_demod::~nrsc5_demod()
{

}

void nrsc5_demod::set_program(int program)
{
    nrsc5_rx->set_program(program);
}

float nrsc5_demod::get_level_db()
{
    if (nrsc5_rx->get_sync())
    {
        return 0.0;
    }
    return -100.0;
}
