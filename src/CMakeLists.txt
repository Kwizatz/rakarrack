# Copyright (C) 2022 Rodrigo Jose Hernandez Cordoba
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

if(ENABLE_MIDI)
	set(RAKARRACK_MIDI_SOURCES
		MIDIConverter.cpp
		rkrMIDI.cpp
	)
	set(RAKARRACK_MIDI_HEADERS
		MIDIConverter.hpp
	)
endif()
set(RAKARRACK_SOURCES
	Alienwah.cpp
	AnalogFilter.cpp
	APhaser.cpp
	Arpie.cpp
	beattracker.cpp
	Chorus.cpp
	CoilCrafter.cpp
	CompBand.cpp
	Compressor.cpp
	Convolotron.cpp
	delayline.cpp
	Distorsion.cpp
	Dual_Flange.cpp
	DynamicFilter.cpp
	Echo.cpp
	Echotron.cpp
	EffectLFO.cpp
	EQ.cpp
	Exciter.cpp
	Expander.cpp
	fileio.cpp
	Filter.cpp
	FilterParams.cpp
	FormantFilter.cpp
	FPreset.cpp
	Gate.cpp
	HarmonicEnhancer.cpp
	Harmonizer.cpp
	icono_rakarrack_128x128.xpm
	icono_rakarrack_32x32.xpm
	Infinity.cpp
	jack.cpp
	Looper.cpp
	mayer_fft.cpp
	MBDist.cpp
	MBVvol.cpp
	metronome.cpp
	MusicDelay.cpp
	NewDist.cpp
	Opticaltrem.cpp
	Pan.cpp
	Phaser.cpp
	process.cpp
	RBEcho.cpp
	RBFilter.cpp
	RecChord.cpp
	RecognizeNote.cpp
	Resample.cpp
	Reverb.cpp
	Reverbtron.cpp
	Ring.cpp
	RyanWah.cpp
	Sequence.cpp
	ShelfBoost.cpp
	Shifter.cpp
	Shuffle.cpp
	smbPitchShift.cpp
	StereoHarm.cpp
	StompBox.cpp
	Sustainer.cpp
	SVFilter.cpp
	Synthfilter.cpp
	Tuner.cpp
	Valve.cpp
	varios.cpp
	Vibe.cpp
	Vocoder.cpp
	Waveshaper.cpp
	main.cpp
	rakarrack.cxx
)
set(RAKARRACK_HEADERS
	Alienwah.hpp
	AnalogFilter.hpp
	APhaser.hpp
	Arpie.hpp
	beattracker.hpp
	Chorus.hpp
	CoilCrafter.hpp
	CompBand.hpp
	Compressor.hpp
	Convolotron.hpp
	delayline.hpp
	Distorsion.hpp
	Dual_Flange.hpp
	DynamicFilter.hpp
	Echo.hpp
	Echotron.hpp
	Effect.hpp
	EffectLFO.hpp
	EQ.hpp
	Exciter.hpp
	Expander.hpp
	f_sin.hpp
	Filter_.hpp
	Filter.hpp
	FilterParams.hpp
	FormantFilter.hpp
	FPreset.hpp
	Gate.hpp
	global.hpp
	HarmonicEnhancer.hpp
	Harmonizer.hpp
	Infinity.hpp
	jack.hpp
	Looper.hpp
	mayer_fft.hpp
	MBDist.hpp
	MBVvol.hpp
	metronome.hpp
	MusicDelay.hpp
	NewDist.hpp
	Opticaltrem.hpp
	Pan.hpp
	Phaser.hpp
	RBEcho.hpp
	RBFilter.hpp
	RecChord.hpp
	RecognizeNote.hpp
	Resample.hpp
	Reverb.hpp
	Reverbtron.hpp
	Ring.hpp
	RyanWah.hpp
	Sequence.hpp
	ShelfBoost.hpp
	Shifter.hpp
	Shuffle.hpp
	smbPitchShift.hpp
	StereoHarm.hpp
	StompBox.hpp
	Sustainer.hpp
	SVFilter.hpp
	Synthfilter.hpp
	Tuner.hpp
	Valve.hpp
	Vibe.hpp
	Vocoder.hpp
	Waveshaper.hpp
	rakarrack.h
)

add_custom_command(OUTPUT rakarrack.cxx rakarrack.h
	COMMAND ${FLTK_FLUID_EXECUTABLE} -c ${CMAKE_CURRENT_SOURCE_DIR}/rakarrack.fl
	MAIN_DEPENDENCY rakarrack.fl
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(rakarrack
	${RAKARRACK_SOURCES}
	${RAKARRACK_HEADERS}
	${RAKARRACK_MIDI_SOURCES}
	${RAKARRACK_MIDI_HEADERS}
)
target_include_directories(rakarrack PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${FLTK_INCLUDE_DIR}
	${ALSA_INCLUDE_DIRS}
	${RAPIDJSON_INCLUDE_PATH}
)
target_link_libraries(rakarrack
	${FLTK_LIBRARIES}
	${ALSA_LIBRARIES}
	PkgConfig::FFTW
	PkgConfig::XPM
	PkgConfig::JACK
	PkgConfig::SAMPLERATE
	PkgConfig::SNDFILE
)

install(TARGETS rakarrack RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
