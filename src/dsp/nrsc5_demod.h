#ifndef NRSC5_DEMOD_H
#define NRSC5_DEMOD_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/filter/firdes.h>

#include <gnuradio/blocks/complex_to_interleaved_short.h>
#include <nrsc5_rx/nrsc5_rx.h>

class nrsc5_demod;

#if GNURADIO_VERSION < 0x030900
typedef boost::shared_ptr<nrsc5_demod> nrsc5_demod_sptr;
#else
typedef std::shared_ptr<nrsc5_demod> nrsc5_demod_sptr;
#endif

/*! \brief Return a shared_ptr to a new instance of nrsc5_demod.
 *
 * This is effectively the public constructor. To avoid accidental use
 * of raw pointers, stereo_demod's constructor is private.
 * make_stereo_demod is the public interface for creating new instances.
 */
nrsc5_demod_sptr make_nrsc5_demod();

/*! \brief FM stereo demodulator.
 *  \ingroup DSP
 *
 * This class implements the nrsc5 demodulator for FM band NRSC-5 IBOC digital radio.
 *
 */
class nrsc5_demod : public gr::hier_block2
{
    friend nrsc5_demod_sptr make_nrsc5_demod();
protected:
    nrsc5_demod();

public:
    ~nrsc5_demod();

private:
    /* GR blocks */
    gr::nrsc5_rx::nrsc5_rx::sptr                            nrsc5_rx;
    gr::blocks::complex_to_interleaved_short::sptr          type_converter;
};


#endif // NRSC5_DEMOD_H
