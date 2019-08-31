#include "Audio.h"
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

using namespace aiaudio;

struct AVPacketRAII {
    AVPacketRAII()
        : pkt(av_packet_alloc())
    {
    }
    ~AVPacketRAII() { av_packet_free(&pkt); }
    AVPacket *operator->() const { return pkt; }
    bool operator!() const { return pkt != nullptr; }

private:
    AVPacket *pkt = nullptr;
};

struct AVCodecParserContextRAII {
    AVCodecParserContextRAII(AVCodecID codecId)
        : parser(av_parser_init(codecId))
    {
    }
    ~AVCodecParserContextRAII() { av_parser_close(parser); }
    AVCodecParserContext &operator->() const { return *parser; }
    bool operator!() const { return parser != nullptr; }
    AVCodecParserContext *getPtr() const { return parser; }

private:
    AVCodecParserContext *parser = nullptr;
};

struct AVFrameRAII {
    AVFrameRAII() {}
    ~AVFrameRAII()
    {
        if (frame) {
            av_frame_free(&frame);
        }
    }
    AVFrame &operator->() const { return *frame; }
    bool operator!() const { return frame != nullptr; }
    AVFrame *&getPtr() { return frame; }

private:
    AVFrame *frame = nullptr;
};

struct FileRAII {
    FileRAII(const char *filepath, const char *mode)
        : f(fopen(filepath, mode))
    {
    }
    ~FileRAII() { fclose(f); }
    FILE &operator->() const { return *f; }
    bool operator!() const { return f != nullptr; }
    FILE *getPtr() const { return f; }

private:
    FILE *f = nullptr;
};

struct AVCodecContextRAII {
    AVCodecContextRAII(const AVCodec *codec)
        : c(avcodec_alloc_context3(codec))
    {
    }
    ~AVCodecContextRAII() { avcodec_free_context(&c); }
    AVCodecContext &operator->() const { return *c; }
    bool operator!() const { return c != nullptr; }
    AVCodecContext *getPtr() const { return c; }

private:
    AVCodecContext *c = nullptr;
};

std::variant<std::vector<float>, std::string> Audio::getVolumeValuesFromAudioFile(const char *path)
{
    AVPacketRAII pkt;

    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    if (!codec) {
        return std::string("Codec not found");
    }

    AVCodecParserContextRAII parser(codec->id);
    if (!parser) {
        return std::string("Parser not found");
    }

    AVCodecContextRAII c(codec);
    if (!c) {
        return std::string("Could not allocate context");
    }

    if (avcodec_open2(c.getPtr(), codec, nullptr) < 0) {
        return std::string("Could not open codec");
    }

    FileRAII f(path, "rb");
    if (!f) {
        return std::string("Error opening file %s", path);
    }

    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data = inbuf;
    size_t data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f.getPtr());
    AVFrameRAII decoded_frame;

    while (data_size > 0) {
        if (!decoded_frame) {
            if (!(decoded_frame.getPtr() = av_frame_alloc())) {
                return std::string("Could not allocate audio frame");
            }
        }
        int ret = av_parser_parse2(
            parser.getPtr(), c.getPtr(), &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            return std::string("Error while parsing");
        }

        data += ret;
        data_size -= ret;

        if (pkt->size) {
            // decode(c, pkt->pkt, decoded_frame, outfile);
        }
        if (data_size < AUDIO_REFILL_THRESH) {
            memmove(inbuf, data, data_size);
            data = inbuf;
            int len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, f.getPtr());
            if (len > 0)
                data_size += len;
        }
    }
}
