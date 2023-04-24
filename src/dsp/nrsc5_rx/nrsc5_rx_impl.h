/* -*- c++ -*- */
/*
 * Copyright 2022 Albert Briscoe.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NRSC5_RX_NRSC5_RX_IMPL_H
#define INCLUDED_NRSC5_RX_NRSC5_RX_IMPL_H

#include <nrsc5_rx/nrsc5_rx.h>
#include <queue>

extern "C" {
#include <nrsc5.h>
}

namespace gr {
namespace nrsc5_rx {

class nrsc5_rx_impl : public nrsc5_rx {
private:
	nrsc5_t* nrsc5;

	// Is there a better (gnuradio specific?) way of buffering audio in a ring buffer/fifo?
	std::queue<int16_t> left_audio_queue, right_audio_queue;

	int nrsc5_sync;
	unsigned int _program;
	bool new_sis_message, new_id3_message;
	pmt::pmt_t sis_message, id3_message;

	void set_program(unsigned int program) override;
	int get_sync() override;

public:
	nrsc5_rx_impl(unsigned int program);
	~nrsc5_rx_impl();

	// Where all the action really happens
	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items,
	gr_vector_int &ninput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items);

	void handle_callback(const nrsc5_event_t *evt);

}; // class nrsc5_rx_impl

void nrsc5_rx_callback(const nrsc5_event_t *event, void* opaque);

} // namespace nrsc5_rx
} // namespace gr

#endif /* INCLUDED_NRSC5_RX_NRSC5_RX_IMPL_H */
