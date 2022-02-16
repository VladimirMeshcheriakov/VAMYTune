#include "little_endian.h"

#ifndef HEADER_WAV_H
#define HEADER_WAV_H

typedef struct
{

    /***************
     * RIFF header. *
     ***************/
    char fChunkID[4];
    uint32_t fChunkSize;
    char fFormat[4];

    /******************
     * "fmt" subchunk. *
     ******************/
    char fSubchunk1ID[4];
    uint32_t fSubchunk1Size;
    uint16_t fAudioFormat;
    uint16_t fNumChannels;
    uint32_t fSampleRate;
    uint32_t fByteRate;
    uint16_t fBlockAlign;
    uint16_t fBitsPerSample;

    /*******************
     * "data" subchunk. *
     *******************/
    char fSubchunk2ID[4];
    uint32_t fSubchunk2Size;
} file_format_header;

bool set_header(size_t pairAmount, file_format_header *hdr)

#endif