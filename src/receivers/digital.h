#ifndef NRSC5_H
#define NRSC5_H

#include "receivers/receiver_base.h"
#include "dsp/resampler_xx.h"
#include "dsp/rx_filter.h"
#include "dsp/rx_demod_nrsc5.h"

class digital;

#if GNURADIO_VERSION < 0x030900
typedef boost::shared_ptr<digital> digital_sptr;
#else
typedef std::shared_ptr<digital> digital_sptr;
#endif

/*! \brief Public constructor of digital_sptr. */
digital_sptr make_digital(float quad_rate, float audio_rate);

class digital : public receiver_base_cf
{
public:
    enum digital_demod {
        DIGITAL_DEMOD_NRSC5 = 0,
        DIGITAL_DEMOD_NUM = 1
    };

    digital(float quad_rate, float audio_rate);
    virtual ~digital() { };

    bool start();
    bool stop();

    void set_quad_rate(float quad_rate);

    void set_filter(double low, double high, double tw);
    void set_cw_offset(double offset);

    float get_signal_level();

    /* Noise blanker */
    bool has_nb() { return false; }

    /* Squelch parameter */
    bool has_sql() { return false; }

    /* AGC */
    bool has_agc() { return false; }

    void set_demod(int demod);

    bool has_fm() { return false; }

    bool has_multiple_programs() { return true; }
    void set_nrsc5_program(int program);

    void get_metadata(std::vector<std::string> &outbuff, int &num);
    bool is_rds_decoder_active();

private:
    bool   d_running;          /*!< Whether receiver is running or not. */
    float  d_quad_rate;        /*!< Input sample rate. */
    int    d_audio_rate;       /*!< Audio output rate. */

    digital_demod             d_demod;    /*!< Current demodulator. */

    resampler_cc_sptr         iq_resamp;   /*!< Baseband resampler. */
    rx_filter_sptr            filter;  /*!< Non-translating bandpass filter.*/

    rx_demod_nrsc5_sptr       nrsc5;

    resampler_ff_sptr         audio_rr0;  /*!< Audio resampler. */
    resampler_ff_sptr         audio_rr1;  /*!< Audio resampler. */

    rx_demod_nrsc5_store_sptr msg_store; /*!< RDS decoded messages */

};

#endif // NRSC5_H
