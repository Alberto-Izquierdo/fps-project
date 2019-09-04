#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "audio/Audio.h"


TEST_CASE("Decode file") {
    aiaudio::Audio audioManager;
    const std::string audioFile("audio.mp3");
    audioManager.getVolumeValuesFromAudioFile(audioFile);
    REQUIRE(8 == 4 + 4);
}
