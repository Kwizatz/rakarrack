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
	1.wav   res_1_wav
	2.wav   res_2_wav
	3.wav   res_3_wav
	4.wav   res_4_wav
	5.wav   res_5_wav
	6.wav   res_6_wav
	7.wav   res_7_wav
	8.wav   res_8_wav
	9.wav   res_9_wav
	1.rvb   res_1_rvb
	2.rvb   res_2_rvb
	3.rvb   res_3_rvb
	4.rvb   res_4_rvb
	5.rvb   res_5_rvb
	6.rvb   res_6_rvb
	7.rvb   res_7_rvb
	8.rvb   res_8_rvb
	9.rvb   res_9_rvb
	10.rvb  res_10_rvb
	1.dly   res_1_dly
	2.dly   res_2_dly
	3.dly   res_3_dly
	4.dly   res_4_dly
	5.dly   res_5_dly
	6.dly   res_6_dly
	7.dly   res_7_dly
	8.dly   res_8_dly
	9.dly   res_9_dly
	10.dly  res_10_dly
	11.dly  res_11_dly
	Default.rkrb  res_Default_rkrb
	Extra.rkrb    res_Extra_rkrb
	Extra1.rkrb   res_Extra1_rkrb
	bg.png        res_bg_png
	bg1.png       res_bg1_png
	bg2.png       res_bg2_png
	bg3.png       res_bg3_png
	bg4.png       res_bg4_png
	bg5.png       res_bg5_png
	bg6.png       res_bg6_png
	bg_gray_furr.png  res_bg_gray_furr_png
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
		INPUT "${RAKARRACK_DATA_DIR}/${_filename}"
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
