
# Add the source files to SRCS_LIST
add_source_files(SRCS_LIST
	afsk1200/cafsk12.cpp
	afsk1200/cafsk12.h
	afsk1200/costabf.c
	afsk1200/filter.h
	filter/fir_decim.cpp
	filter/fir_decim.h
	filter/fir_decim_coef.h
	rds/api.h
	rds/constants.h
	rds/decoder_impl.cc
	rds/decoder_impl.h
	rds/decoder.h
	rds/parser_impl.cc
	rds/parser_impl.h
	rds/parser.h
	rds/tmc_events.h
	nrsc5_rx/api.h
	nrsc5_rx/nrsc5_rx.h
	nrsc5_rx/nrsc5_rx_impl.cc
	nrsc5_rx/nrsc5_rx_impl.h
	agc_impl.cpp
	agc_impl.h
	correct_iq_cc.cpp
	correct_iq_cc.h
	downconverter.cpp
	downconverter.h
	fm_deemph.cpp
	fm_deemph.h
	lpf.cpp
	lpf.h
	resampler_xx.cpp
	resampler_xx.h
	rx_agc_xx.cpp
	rx_agc_xx.h
	rx_demod_am.cpp
	rx_demod_am.h
	rx_demod_fm.cpp
	rx_demod_fm.h
	rx_fft.cpp
	rx_fft.h
	rx_filter.cpp
	rx_filter.h
	rx_meter.cpp
	rx_meter.h
	rx_noise_blanker_cc.cpp
	rx_noise_blanker_cc.h
	rx_rds.cpp
	rx_rds.h
	sniffer_f.cpp
	sniffer_f.h
	stereo_demod.cpp
	stereo_demod.h
	rx_demod_nrsc5.cpp
	rx_demod_nrsc5.h
)
