/* -*- c++ -*- */
/*
 * Copyright 2022 Albert Briscoe.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <string>

#include "nrsc5_rx_impl.h"

namespace gr {
namespace nrsc5_rx {

nrsc5_rx::sptr
nrsc5_rx::make(int program)
{
	return gnuradio::make_block_sptr<nrsc5_rx_impl>(program);
}

/*
 * The private constructor
 */
nrsc5_rx_impl::nrsc5_rx_impl(int program)
	: gr::block("nrsc5_rx",
	            gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, 2 * sizeof(int16_t)),
	            gr::io_signature::make(2 /* min outputs */, 2 /*max outputs */, sizeof(float)))
{
	message_port_register_out(pmt::mp("out"));

	nrsc5_sync = 0;
	new_sis_message = false;
	new_id3_message = false;

	if ((program >= 0) && (program < 4))
		_program = program;
	else {
		std::cerr << "gr-nrsc5_rx: Warning: " << program << " is not a valid program number.\n";
		_program = 0;
	}

	if (nrsc5_open_pipe(&nrsc5) != 0)
		throw std::runtime_error("nrsc5_rx_impl: nrsc5_open_pipe failed");

	nrsc5_set_auto_gain(nrsc5, 1);
	nrsc5_set_callback(nrsc5, nrsc5_rx_callback, NULL);

	nrsc5_start(nrsc5);
}

/*
 * Our virtual destructor.
 */
nrsc5_rx_impl::~nrsc5_rx_impl() {
	nrsc5_stop(nrsc5);
	nrsc5_close(nrsc5);
}

void
nrsc5_rx_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required) {
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	unsigned ninputs = ninput_items_required.size ();
	for(unsigned i = 0; i < ninputs; i++) {
//		ninput_items_required[i] = noutput_items;
//		std::cerr << "input" << i << " needs: " << (noutput_items * 135) / 8 << "\n";
		ninput_items_required[i] = (noutput_items * 135) / 8;
	}

	// add a case here for when nrsc5 doesn't have sync?
	// maybe for when nrsc5 has a reasonably sized buffer of data but hasn't output anything yet?
}

int
nrsc5_rx_impl::general_work (int                       noutput_items,
                             gr_vector_int             &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star       &output_items)
{
	int n = 0;

//	const int16_t* in = static_cast<const int16_t*>(input_items[0]);

	// hack to get around non-const variables in nrsc5.
	// It won't cause undefined behavior as long as nrsc5 doesn't change how piped samples work
	int16_t* in = const_cast<int16_t*>(static_cast<const int16_t*>(input_items[0]));

	auto out0 = static_cast<float*>(output_items[0]);
	auto out1 = static_cast<float*>(output_items[1]);

	// Do <+signal processing+>
	// Tell runtime system how many input items we consumed on
	// each input stream.

	nrsc5_pipe_samples_cs16(nrsc5, in, 2 * ninput_items[0]);

	for (int i = 0; i < noutput_items * 2; i++, n++) {
		if (left_audio_queue.empty() || right_audio_queue.empty())
			break;
		out0[i] = (float)left_audio_queue.front() / 32767;
		out1[i] = (float)right_audio_queue.front() / 32767;
		left_audio_queue.pop();
		right_audio_queue.pop();
	}

	consume_each (ninput_items[0]);

	// output SIS information
	if (new_sis_message) {
		message_port_pub(pmt::mp("out"), sis_message);
		new_sis_message = false;
	}
	if (new_id3_message) {
		message_port_pub(pmt::mp("out"), id3_message);
		new_id3_message = false;
	}

	// Tell runtime system how many output items we produced.
	return n;
}

void nrsc5_rx_impl::set_program(int program) {
	// only 4 programs are supported
	if ((program < 0) || (program > 3)) {
		std::cerr << "gr-nrsc5_rx: Warning: " << program << " is not a valid program number.\n";
		return;
	}

	_program = program;

	// Clear the program specific id3 information
	message_port_pub(pmt::mp("out"), pmt::mp(pmt::from_long(1), pmt::mp(""), pmt::mp(""), pmt::mp(""), pmt::mp("")));
	new_id3_message = false;
}

int nrsc5_rx_impl::get_sync() {
	return nrsc5_sync;
}

} /* namespace nrsc5_rx */
} /* namespace gr */


void nrsc5_rx_callback(const nrsc5_event_t *event, void *opaque) {
	switch (event->event) {
	case NRSC5_EVENT_LOST_DEVICE: // rtl_sdr disconnected. Should never occur, but process it anyways.
		nrsc5_sync = 0;
		break;

	// use these for signal level?
	case NRSC5_EVENT_BER: // Bit Error Rate
		break;
	case NRSC5_EVENT_MER: // Modulation Error Ratio?
		break;

	case NRSC5_EVENT_IQ: // IQ data
		break;
	case NRSC5_EVENT_HDC: // HDC audio packet?
		break;
	case NRSC5_EVENT_AUDIO: // Audio
		if (event->audio.program == _program) {
			for (int i = 0; i < 2048; i++) {
				left_audio_queue.push((event->audio.data)[2*i]);
				right_audio_queue.push((event->audio.data)[2*i+1]);
			}
		}
		break;

	case NRSC5_EVENT_SYNC: // Got sync
		nrsc5_sync = 1;
		break;
	case NRSC5_EVENT_LOST_SYNC: // Sync lost
		nrsc5_sync = 0;
		break;

	case NRSC5_EVENT_ID3: // ID3 information
		// only care about the current program
		if (event->id3.program != _program)
			break;

		// null values can cause errors, so every pointer is checked
		id3_message = pmt::mp(pmt::from_long(1), // ID3 data
		                      pmt::string_to_symbol(event->id3.title  ? event->id3.title  : ""),
		                      pmt::string_to_symbol(event->id3.artist ? event->id3.artist : ""),
		                      pmt::string_to_symbol(event->id3.album  ? event->id3.album  : ""),
		                      pmt::string_to_symbol(event->id3.genre  ? event->id3.genre  : ""));
		new_id3_message = true;
		break;
	case NRSC5_EVENT_SIG: // Service information?
		break;
	case NRSC5_EVENT_LOT: // LOT file data available?
		break;
	case NRSC5_EVENT_SIS: // Station information
		char nrsc5_facility_id[20];
		sprintf(nrsc5_facility_id, "%d", event->sis.fcc_facility_id);

		sis_message = pmt::mp(pmt::from_long(0), // SIS data
		                      pmt::string_to_symbol(event->sis.name ? event->sis.name : ""),
		                      pmt::string_to_symbol(event->sis.slogan ? event->sis.slogan : ""),
		                      pmt::string_to_symbol(event->sis.message ? event->sis.message : ""),
		                      pmt::string_to_symbol(event->sis.alert ? event->sis.alert : ""),
		                      pmt::string_to_symbol(event->sis.country_code ? event->sis.country_code : ""),
		                      pmt::string_to_symbol(nrsc5_facility_id));
		new_sis_message = true;
		break;
	default:
		std::cerr << "gr-nrsc5_rx: Warning: Unknown event\n";
		break;
	}
}
