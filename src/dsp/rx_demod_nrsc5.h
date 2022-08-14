#ifndef RX_DEMOD_NRSC5_H
#define RX_DEMOD_NRSC5_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/filter/firdes.h>

#include <gnuradio/blocks/complex_to_interleaved_short.h>
#include <mutex>
#include <queue>

#include "dsp/nrsc5_rx/nrsc5_rx.h"

class rx_demod_nrsc5;
class rx_demod_nrsc5_store;

#if GNURADIO_VERSION < 0x030900
typedef boost::shared_ptr<rx_demod_nrsc5> rx_demod_nrsc5_sptr;
typedef boost::shared_ptr<rx_demod_nrsc5_store> rx_demod_nrsc5_store_sptr;
#else
typedef std::shared_ptr<rx_demod_nrsc5> rx_demod_nrsc5_sptr;
typedef std::shared_ptr<rx_demod_nrsc5_store> rx_demod_nrsc5_store_sptr;
#endif

/*! \brief Return a shared_ptr to a new instance of rx_demod_nrsc5.
 *
 * This is effectively the public constructor. To avoid accidental use
 * of raw pointers, stereo_demod's constructor is private.
 * make_stereo_demod is the public interface for creating new instances.
 */
rx_demod_nrsc5_sptr make_rx_demod_nrsc5();
rx_demod_nrsc5_store_sptr make_rx_demod_nrsc5_store();

/*! \brief FM stereo demodulator.
 *  \ingroup DSP
 *
 * This class implements the nrsc5 demodulator for FM band NRSC-5 IBOC digital radio.
 *
 */
class rx_demod_nrsc5 : public gr::hier_block2
{
    friend rx_demod_nrsc5_sptr make_rx_demod_nrsc5();
protected:
    rx_demod_nrsc5();

public:
    ~rx_demod_nrsc5();
    void set_program(int program);
    float get_level_db();

private:
    /* GR blocks */
    gr::nrsc5_rx::nrsc5_rx::sptr                            nrsc5_rx;
    gr::blocks::complex_to_interleaved_short::sptr          type_converter;
};


class rx_demod_nrsc5_store : public gr::block
{
public:
    rx_demod_nrsc5_store();
    ~rx_demod_nrsc5_store();

    void get_message(std::string (&out)[6], int &type);

private:
    void store(pmt::pmt_t msg);

    std::mutex d_mutex;
    std::queue<pmt::pmt_t> d_messages;

};

#endif // RX_DEMOD_NRSC5_H
