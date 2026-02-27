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

# ============================================================================
# Engine sources — GUI-independent DSP, effects, JACK, MIDI, preferences
# ============================================================================

if(ENABLE_MIDI)
	set(ENGINE_MIDI_SOURCES
		MIDIConverter.cpp
		rkrMIDI.cpp
	)
	set(ENGINE_MIDI_HEADERS
		MIDIConverter.hpp
	)
endif()

set(ENGINE_SOURCES
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
	EngineController.cpp
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
	Preferences.cpp
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
)

set(ENGINE_HEADERS
	AllEffects.hpp
	Alienwah.hpp
	AnalogFilter.hpp
	AppConfig.hpp
	EmbeddedResource.hpp
	EngineController.hpp
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
	dsp_constants.hpp
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
	PresetBank.hpp
	Preferences.hpp
	RBEcho.hpp
	RBFilter.hpp
	RecChord.hpp
	RecognizeNote.hpp
	Resample.hpp
	Reverb.hpp
	Reverbtron.hpp
	Ring.hpp
	RingBuffer.hpp
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
)

# ============================================================================
# Embedded data resources (banks, IRs, skins — used by engine)
# ============================================================================

include(embed_resource)

set(RAKARRACK_DATA_DIR ${CMAKE_SOURCE_DIR}/data)

set(RESOURCE_LIST
	1.wav   1_wav
	2.wav   2_wav
	3.wav   3_wav
	4.wav   4_wav
	5.wav   5_wav
	6.wav   6_wav
	7.wav   7_wav
	8.wav   8_wav
	9.wav   9_wav
	1.rvb   1_rvb
	2.rvb   2_rvb
	3.rvb   3_rvb
	4.rvb   4_rvb
	5.rvb   5_rvb
	6.rvb   6_rvb
	7.rvb   7_rvb
	8.rvb   8_rvb
	9.rvb   9_rvb
	10.rvb  10_rvb
	1.dly   1_dly
	2.dly   2_dly
	3.dly   3_dly
	4.dly   4_dly
	5.dly   5_dly
	6.dly   6_dly
	7.dly   7_dly
	8.dly   8_dly
	9.dly   9_dly
	10.dly  10_dly
	11.dly  11_dly
	Default.rkrb  Default_rkrb
	Extra.rkrb    Extra_rkrb
	Extra1.rkrb   Extra1_rkrb
	bg.png        bg_png
	bg1.png       bg1_png
	bg2.png       bg2_png
	bg3.png       bg3_png
	bg4.png       bg4_png
	bg5.png       bg5_png
	bg6.png       bg6_png
	bg_gray_furr.png  bg_gray_furr_png
)

set(RESOURCE_GENERATED_SOURCES "")
set(RESOURCE_GENERATED_HEADERS "")
list(LENGTH RESOURCE_LIST _len)
math(EXPR _last "${_len} - 1")
foreach(_i RANGE 0 ${_last} 2)
	math(EXPR _j "${_i} + 1")
	list(GET RESOURCE_LIST ${_i} _filename)
	list(GET RESOURCE_LIST ${_j} _varname)
	set(_cpp "${CMAKE_CURRENT_BINARY_DIR}/${_varname}.cpp")
	set(_hpp "${CMAKE_CURRENT_BINARY_DIR}/${_varname}.hpp")
	embed_resource(
		WORKDIR "${RAKARRACK_DATA_DIR}"
		INPUT "${_filename}"
		CPP   "${_cpp}"
		HPP   "${_hpp}"
		NAME  "${_varname}"
	)
	list(APPEND RESOURCE_GENERATED_SOURCES "${_cpp}")
	list(APPEND RESOURCE_GENERATED_HEADERS "${_hpp}")
endforeach()

# ============================================================================
# Engine static library
# ============================================================================

add_library(rakarrack_engine STATIC
	${ENGINE_SOURCES}
	${ENGINE_HEADERS}
	${ENGINE_MIDI_SOURCES}
	${ENGINE_MIDI_HEADERS}
	${RESOURCE_GENERATED_SOURCES}
	${RESOURCE_GENERATED_HEADERS}
)

target_include_directories(rakarrack_engine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${ALSA_INCLUDE_DIRS}
)

target_link_libraries(rakarrack_engine PUBLIC
	${ALSA_LIBRARIES}
	PkgConfig::FFTW
	PkgConfig::JACK
	PkgConfig::SAMPLERATE
	PkgConfig::SNDFILE
	nlohmann_json::nlohmann_json
	rakconvert_lib
	rakverb_lib
)

target_precompile_headers(rakarrack_engine PRIVATE
	dsp_constants.hpp
	<memory>
	<array>
	<vector>
	<cmath>
	<cstdlib>
	<cstring>
	<string>
)

# ============================================================================
# Qt6 GUI
# ============================================================================

add_subdirectory(gui/qt6)
