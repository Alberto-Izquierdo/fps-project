#ifndef AUDIO_H_
#define AUDIO_H_

#include <optional>
#include <vector>
#include <string>
#include <variant>

namespace aiaudio
{
class Audio
{
public:
    std::variant<std::vector<float>, std::string> getVolumeValuesFromAudioFile(const char *path);
};
}  // namespace aiaudio

#endif
