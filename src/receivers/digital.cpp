#include <cmath>
#include <iostream>
#include <QDebug>
#include "receivers/digital.h"

#define PREF_QUAD_RATE 744187.5
#define PREF_AUDIO_RATE 44100.0

digital_sptr make_digital(float quad_rate, float audio_rate)
{
    return gnuradio::get_initial_sptr(new digital(quad_rate, audio_rate));
}

digital::digital(float quad_rate, float audio_rate)
    : receiver_base_cf("DIGITAL"),
      d_running(false),
      d_quad_rate(quad_rate),
      d_audio_rate(audio_rate),
      d_demod(DIGITAL_DEMOD_NRSC5)
{
    iq_resamp = make_resampler_cc(PREF_QUAD_RATE/d_quad_rate);
    filter = make_rx_filter(PREF_QUAD_RATE, -320000.0, 320000.0, 50000.0);
    nrsc5 = make_rx_demod_nrsc5();
    rds_store = make_rx_demod_nrsc5_store();

    audio_rr0.reset();
    audio_rr1.reset();
    if (d_audio_rate != PREF_AUDIO_RATE)
    {
        std::cout << "Resampling audio " << PREF_AUDIO_RATE << " -> "
                  << d_audio_rate << std::endl;
        audio_rr0 = make_resampler_ff(d_audio_rate/PREF_AUDIO_RATE);
        audio_rr1 = make_resampler_ff(d_audio_rate/PREF_AUDIO_RATE);
    }

    connect(self(), 0, filter, 0);
    connect(filter, 0, iq_resamp, 0);
    connect(iq_resamp, 0, nrsc5, 0);

    if (audio_rr0)
    {
        connect(nrsc5, 0, audio_rr0, 0);
        connect(nrsc5, 1, audio_rr1, 0);

        connect(audio_rr0, 0, self(), 0);
        connect(audio_rr1, 0, self(), 1);
    }
    else
    {
        connect(nrsc5, 0, self(), 0);
        connect(nrsc5, 1, self(), 1);
    }
    msg_connect(nrsc5, "SIS", rds_store, "store");
}

bool digital::start()
{
    d_running = true;

    return true;
}

bool digital::stop()
{
    d_running = false;

    return true;
}

void digital::set_quad_rate(float quad_rate)
{
    if (std::abs(d_quad_rate-quad_rate) > 0.5)
    {
        qDebug() << "Changing Digital quad rate:"  << d_quad_rate << "->" << quad_rate;
        d_quad_rate = quad_rate;
        lock();
        iq_resamp->set_rate(PREF_QUAD_RATE/d_quad_rate);
        unlock();
    }
}

void digital::set_filter(double low, double high, double tw)
{
//    filter->set_param(low, high, tw);
}

void digital::set_cw_offset(double offset)
{
    filter->set_cw_offset(offset);
}

float digital::get_signal_level()
{
    return nrsc5->get_level_db();
}

void digital::set_demod(int rx_demod)
{
    digital_demod current_demod = d_demod;

    if ((rx_demod < DIGITAL_DEMOD_NRSC5) || (rx_demod >= DIGITAL_DEMOD_NUM))
        return;

    if (rx_demod == current_demod) {
        /* nothing to do */
        return;
    }

    /* lock graph while we reconfigure */
    lock();

    switch (current_demod)
    {
    case DIGITAL_DEMOD_NRSC5:
    default:
        if (audio_rr0)
        {
            disconnect(nrsc5, 0, audio_rr0, 0);
            disconnect(nrsc5, 1, audio_rr1, 0);

            disconnect(audio_rr0, 0, self(), 0);
            disconnect(audio_rr1, 0, self(), 1);
        }
        else
        {
            disconnect(nrsc5, 0, self(), 0);
            disconnect(nrsc5, 1, self(), 1);
        }
        break;
    }

    switch (rx_demod)
    {
    case DIGITAL_DEMOD_NRSC5:
    default:
        if (audio_rr0)
        {
            connect(nrsc5, 0, audio_rr0, 0);
            connect(nrsc5, 1, audio_rr1, 0);

            connect(audio_rr0, 0, self(), 0);
            connect(audio_rr1, 0, self(), 1);
        }
        else
        {
            connect(nrsc5, 0, self(), 0);
            connect(nrsc5, 1, self(), 1);
        }
        break;
    }

    /* continue processing */
    unlock();
}

void digital::set_nrsc5_program(int program)
{
    nrsc5->set_program(program);
}

void digital::get_sis_data(std::string (&outbuff)[6], int &num)
{
    rds_store->get_message(outbuff, num);
}

bool digital::is_rds_decoder_active()
{
    return true;
}
