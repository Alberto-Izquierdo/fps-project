#ifndef AUDIO_H_
#define AUDIO_H_

#include <optional>
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>

namespace aiaudio
{
class Audio
{
public:
    std::variant<std::vector<float>, std::string> getVolumeValuesFromAudioFile(const std::string &path);

private:
    std::unordered_map<size_t, std::vector<float>> m_filesAlreadyProcessed;
};
}  // namespace aiaudio

#endif
