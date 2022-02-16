#include "header_wav.h"

bool set_header(size_t pairAmount, file_format_header *hdr)
{
    /******************************
     *  Magic file format strings. *
     ******************************/
    static const char fChunkID[] = {'R', 'I', 'F', 'F'};
    static const char fFormat[] = {'W', 'A', 'V', 'E'};
    static const char fSubchunk1ID[] = {'f', 'm', 't', ' '};
    static const char fSubchunk2ID[] = {'d', 'a', 't', 'a'};

    /*****************************************************************
     * Can't make the following local static, as we need to check the *
     * endianness.                                                    *
     *****************************************************************/
    static const uint16_t N_CHANNELS = 2;
    static const uint32_t fSubchunk1Size = 16;
    static const uint16_t fAudioFormat = 3;
    static const uint16_t fBitsPerSample = 32;
    static const uint32_t fRIFFChunkDescriptorLength = 12;
    static const uint32_t fFmtSubChunkDescriptorLength = 24;

    

    /**************************************
     * Load the magic file format strings. *
     **************************************/
    for (size_t i = 0; i < 4; ++i)
    {
        hdr->fChunkID[i] = fChunkID[i];
        hdr->fFormat[i] = fFormat[i];
        hdr->fSubchunk1ID[i] = fSubchunk1ID[i];
        hdr->fSubchunk2ID[i] = fSubchunk2ID[i];
    }

    /********************************
     * WriteWavePCM() configuration: *
     * - 2 channels,                 *
     * - frequency 44100 Hz.         *
     ********************************/
    static const uint32_t SAMPLE_RATE = 44100;
    static const uint16_t BITS_PER_BYTE = 8;

    const uint32_t fByteRate = SAMPLE_RATE * N_CHANNELS * fBitsPerSample /
                               BITS_PER_BYTE;

    const uint16_t fBlockAlign = N_CHANNELS * fBitsPerSample / BITS_PER_BYTE;

    const uint32_t fSubchunk2Size = pairAmount * N_CHANNELS * fBitsPerSample /
                                    BITS_PER_BYTE;

    const uint32_t fChunkSize = fRIFFChunkDescriptorLength +
                                fFmtSubChunkDescriptorLength + fSubchunk2Size;

    bool little_endian = is_little_endian();

    if (!little_endian)
    {
        hdr->fAudioFormat = little_endian_uint16_t(hdr->fAudioFormat);
        hdr->fBitsPerSample = little_endian_uint16_t(hdr->fBitsPerSample);
        hdr->fBlockAlign = little_endian_uint16_t(hdr->fBlockAlign);
        hdr->fByteRate = little_endian_uint32_t(hdr->fByteRate);
        hdr->fChunkSize = little_endian_uint32_t(hdr->fChunkSize);
        hdr->fNumChannels = little_endian_uint16_t(hdr->fNumChannels);
        hdr->fSampleRate = little_endian_uint32_t(hdr->fSampleRate);
        hdr->fSubchunk1Size = little_endian_uint32_t(hdr->fSubchunk1Size);
        hdr->fSubchunk2Size = little_endian_uint32_t(hdr->fSubchunk2Size);
    }
    else
    {
        hdr->fAudioFormat = fAudioFormat;
        hdr->fBitsPerSample = fBitsPerSample;
        hdr->fBlockAlign = fBlockAlign;
        hdr->fByteRate = fByteRate;
        hdr->fChunkSize = fChunkSize;
        hdr->fNumChannels = N_CHANNELS;
        hdr->fSampleRate = SAMPLE_RATE;
        hdr->fSubchunk1Size = fSubchunk1Size;
        hdr->fSubchunk2Size = fSubchunk2Size;
    }
    return little_endian;
}
