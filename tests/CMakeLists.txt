# rakarrack - guitar multi-effects processor
# SPDX-License-Identifier: GPL-2.0-only

# EffectGraph is compiled directly into the test rather than linked from
# rakarrack_engine. That keeps the test free of JACK, FFTW and the engine's
# global state, and means a build failure here is a real signal: the graph has
# picked up a dependency it was designed not to have.
add_executable(effect_graph_test
	EffectGraphTest.cpp
	${CMAKE_SOURCE_DIR}/src/EffectGraph.cpp
)

target_include_directories(effect_graph_test PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
)

add_test(NAME EffectGraph COMMAND effect_graph_test)
