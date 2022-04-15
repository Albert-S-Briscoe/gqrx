#ifndef NRSC5_H
#define NRSC5_H

#include "receivers/receiver_base.h"
#include "dsp/resampler_xx.h"
#include "dsp/rx_filter.h"
#include "dsp/nrsc5_demod.h"

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
    /*void set_nb_on(int nbid, bool on);
    void set_nb_threshold(int nbid, float threshold);*/

    /* Squelch parameter */
    bool has_sql() { return false; }
    /*void set_sql_level(double level_db);
    void set_sql_alpha(double alpha);*/

    /* AGC */
    bool has_agc() { return false; }
    /*void set_agc_on(bool agc_on);
    void set_agc_hang(bool use_hang);
    void set_agc_threshold(int threshold);
    void set_agc_slope(int slope);
    void set_agc_decay(int decay_ms);
    void set_agc_manual_gain(int gain);*/

    void set_demod(int demod);

    bool has_fm() {return false; }
    /*void set_fm_maxdev(float maxdev_hz);
    void set_fm_deemph(double tau);*/

    bool has_nrsc5() {return true; }
    void set_nrsc5_program(int program);

private:
    bool   d_running;          /*!< Whether receiver is running or not. */
    float  d_quad_rate;        /*!< Input sample rate. */
    int    d_audio_rate;       /*!< Audio output rate. */

    digital_demod             d_demod;    /*!< Current demodulator. */

    resampler_cc_sptr         iq_resamp;   /*!< Baseband resampler. */
    rx_filter_sptr            filter;  /*!< Non-translating bandpass filter.*/

    nrsc5_demod_sptr          nrsc5;

    resampler_ff_sptr         audio_rr0;  /*!< Audio resampler. */
    resampler_ff_sptr         audio_rr1;  /*!< Audio resampler. */
};

#endif // NRSC5_H
