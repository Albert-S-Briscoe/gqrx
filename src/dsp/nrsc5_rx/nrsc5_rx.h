/* -*- c++ -*- */
/*
 * Copyright 2022 Albert Briscoe.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NRSC5_RX_NRSC5_RX_H
#define INCLUDED_NRSC5_RX_NRSC5_RX_H

#include <gnuradio/block.h>

#include "dsp/nrsc5_rx/api.h"

extern "C" {
#include <nrsc5.h>
}

namespace gr {
namespace nrsc5_rx {

/*!
 * \brief <+description of block+>
 * \ingroup nrsc5_rx
 *
 */
class NRSC5_RX_API nrsc5_rx : virtual public gr::block
{
public:
	typedef std::shared_ptr<nrsc5_rx> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of nrsc5_rx::nrsc5_rx.
	 *
	 * To avoid accidental use of raw pointers, nrsc5_rx::nrsc5_rx's
	 * constructor is in a private implementation
	 * class. nrsc5_rx::nrsc5_rx::make is the public interface for
	 * creating new instances.
	 */
	static sptr make(int program=0);

	virtual void set_program(int program) = 0;
	virtual int get_sync() {return -100.0;};
};

} // namespace nrsc5_rx
} // namespace gr

#endif /* INCLUDED_NRSC5_RX_NRSC5_RX_H */
