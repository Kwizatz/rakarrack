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

# Same reasoning for the bank serialiser: it depends only on the preset
# structs and nlohmann, so the test does not drag in the engine.
add_executable(bank_json_test
	BankJsonTest.cpp
	${CMAKE_SOURCE_DIR}/src/BankJson.cpp
)

target_include_directories(bank_json_test PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
)

target_link_libraries(bank_json_test PRIVATE nlohmann_json::nlohmann_json)

add_test(NAME BankJson COMMAND bank_json_test)

# The graph serialiser works on layouts rather than live effects, so this needs
# only EffectGraph and nlohmann -- no engine, no audio device.
add_executable(graph_json_test
	GraphJsonTest.cpp
	${CMAKE_SOURCE_DIR}/src/GraphJson.cpp
	${CMAKE_SOURCE_DIR}/src/EffectGraph.cpp
)

target_include_directories(graph_json_test PRIVATE
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
)

target_link_libraries(graph_json_test PRIVATE nlohmann_json::nlohmann_json)

add_test(NAME GraphJson COMMAND graph_json_test)
