#include "Audio.h"
extern "C"
{
#include <libavutil/mathematics.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
}

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

using namespace aiaudio;

template <typename T, typename F>
class RAIIContainer
{
public:
    RAIIContainer(T *value, F releaseFunction)
        : m_value(value)
        , m_releaseFunction(releaseFunction)
    {
    }
    ~RAIIContainer()
    {
        if (m_value != nullptr) {
            m_releaseFunction();
            m_value = nullptr;
        }
    }
    T *operator->() const { return m_value; }
    bool operator!() const { return m_value == nullptr; }
    T *get() const { return m_value; }
    T *&get() { return m_value; }

private:
    T *m_value;
    F m_releaseFunction;
};

static std::optional<std::string> decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile)
{
    int i, ch;
    int ret, data_size;
    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        return "Error submitting the packet to the decoder";
    }
    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return {};
        else if (ret < 0) {
            return "Error during decoding";
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            return "Failed to calculate data size";
        }
        for (i = 0; i < frame->nb_samples; i++) {
            for (ch = 0; ch < dec_ctx->channels; ch++) {
                fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
            }
        }
    }
    return {};
}

std::variant<std::vector<float>, std::string> Audio::getVolumeValuesFromAudioFile(const std::string &path)
{
    std::hash<std::string> hasher;
    auto hash = hasher(path);
    auto searchResult = m_filesAlreadyProcessed.find(hash);
    if (searchResult != m_filesAlreadyProcessed.end()) {
        return searchResult->second;
    }
    AVPacket *pktPtr = av_packet_alloc();
    RAIIContainer pkt(pktPtr, [&]() { av_packet_free(&pktPtr); });

    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    if (!codec) {
        return std::string("Codec not found");
    }

    AVCodecParserContext *parserPtr = av_parser_init(codec->id);;
    RAIIContainer parser(parserPtr, [&]() { av_parser_close(parserPtr); });
    if (!parser) {
        return std::string("Parser not found");
    }

    AVCodecContext *cPtr = avcodec_alloc_context3(codec);
    RAIIContainer c(cPtr, [&]() { avcodec_free_context(&cPtr); });
    if (!c) {
        return std::string("Could not allocate context");
    }

    if (avcodec_open2(c.get(), codec, nullptr) < 0) {
        return std::string("Could not open codec");
    }

    FILE *fPtr = fopen(path.c_str(), "rb");
    RAIIContainer f(fPtr, [&]() { fclose(fPtr); });
    if (!f) {
        return std::string("Error opening file %s", path.c_str());
    }

    FILE *ofPtr = fopen(std::to_string(hash).c_str(), "wb");
    RAIIContainer outfile(ofPtr, [&]() { fclose(ofPtr); });
    if (!f) {
        return std::string("Error opening output file %s", std::to_string(hash).c_str());
    }

    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data = inbuf;
    size_t data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f.get());
    AVFrame *decodedFramePtr = nullptr;
    RAIIContainer decoded_frame(decodedFramePtr, [&]() { av_frame_free(&decodedFramePtr); });

    while (data_size > 0) {
        if (!decoded_frame) {
            if (!(decoded_frame.get() = av_frame_alloc())) {
                return std::string("Could not allocate audio frame");
            }
        }
        int ret = av_parser_parse2(
            parser.get(), c.get(), &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            return std::string("Error while parsing");
        }

        data += ret;
        data_size -= ret;

        if (pkt->size) {
            auto result = decode(c.get(), pkt.get(), decoded_frame.get(), outfile.get());
        }
        if (data_size < AUDIO_REFILL_THRESH) {
            memmove(inbuf, data, data_size);
            data = inbuf;
            int len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, f.get());
            if (len > 0)
                data_size += len;
        }
    }
    std::vector<float> result;
    return result;
}
