cmake_minimum_required(VERSION 3.22)

project(StarlightDrift VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.10
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(StarlightDrift
  COMPANY_NAME "Starlight"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE
  PLUGIN_MANUFACTURER_CODE Stlt
  PLUGIN_CODE SDrf
  FORMATS VST3 AU Standalone
  PRODUCT_NAME "Starlight Drift"
)

target_sources(StarlightDrift PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginProcessor.h
  Source/PluginEditor.cpp
  Source/PluginEditor.h
  Source/DSP/GranularDelay.h
  Source/DSP/ShimmerReverb.h
  Source/UI/LookAndFeel.h
  Source/UI/LockableSlider.h
  Source/UI/LockableSlider.cpp
  Source/UI/LockableButton.h
  Source/UI/LockableButton.cpp
)

juce_generate_juce_header(StarlightDrift)

target_compile_definitions(StarlightDrift PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(StarlightDrift PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_gui_extra
)
