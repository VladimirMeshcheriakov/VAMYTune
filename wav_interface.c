#include "wav_interface.h"

/****************************************************
* From http://stackoverflow.com/questions/12791864/ *
*      c-program-to-check-little-vs-big-endian      *
****************************************************/
static bool is_little_endian() 
{
    volatile uint32_t i = 0x01234567;
    return (*((uint8_t*)(&i))) == 0x67;
}

static uint16_t little_endian_uint16_t(uint16_t num) 
{
    return (((0xff00 & num) >> 8) | ((0xff & num) << 8));
}

static uint32_t little_endian_uint32_t(uint32_t num) {
    return ((((0xff000000 & num) >> 24) | 
             ((0xff & num) << 24)       | 
             ((0xff0000 & num) >> 8))   | 
             ((0xff00 & num) << 8));
}

static float little_endian_float( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

#pragma pack(0)
typedef struct {

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


bool WriteWavePCM(float* sound, size_t pairAmount, const char* fileName)
{
    //If no sound buffer is given or no filename provided, stop execution
    if (sound == NULL || fileName == NULL) 
    {
        printf("ERROR: No filename, or no sound buffer\n");
        return false;
    }
    //Pointer to the file to which we will write
    FILE* fout = fopen(fileName, "wb");
    //If file was not opened correctly, stop execution
    if (fout == NULL) 
    {
        printf("ERROR: Cannot open the file: %s\n",fileName);
        return false;
    }

    /******************************
    *  Magic file format strings. *
    ******************************/
    static const char fChunkID[]     = {'R', 'I', 'F', 'F'};
    static const char fFormat[]      = {'W', 'A', 'V', 'E'};
    static const char fSubchunk1ID[] = {'f', 'm', 't', ' '};
    static const char fSubchunk2ID[] = {'d', 'a', 't', 'a'};

    /*****************************************************************
    * Can't make the following local static, as we need to check the *
    * endianness.                                                    *
    *****************************************************************/
    static const uint16_t N_CHANNELS                   = 2;
    static const uint32_t fSubchunk1Size               = 16;
    static const uint16_t fAudioFormat                 = 3;
    static const uint16_t fBitsPerSample               = 32;
    static const uint32_t fRIFFChunkDescriptorLength   = 12;
    static const uint32_t fFmtSubChunkDescriptorLength = 24;

    file_format_header hdr;

    /**************************************
    * Load the magic file format strings. *
    **************************************/
    for (size_t i = 0; i < 4; ++i) 
    {
        hdr.fChunkID[i]     = fChunkID[i];
        hdr.fFormat[i]      = fFormat[i];
        hdr.fSubchunk1ID[i] = fSubchunk1ID[i];
        hdr.fSubchunk2ID[i] = fSubchunk2ID[i];
    }

    /********************************
    * WriteWavePCM() configuration: *
    * - 2 channels,                 *
    * - frequency 44100 Hz.         *
    ********************************/
    static const uint32_t SAMPLE_RATE   = 44100;
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
        hdr.fAudioFormat   = little_endian_uint16_t(hdr.fAudioFormat);
        hdr.fBitsPerSample = little_endian_uint16_t(hdr.fBitsPerSample);
        hdr.fBlockAlign    = little_endian_uint16_t(hdr.fBlockAlign);
        hdr.fByteRate      = little_endian_uint32_t(hdr.fByteRate);
        hdr.fChunkSize     = little_endian_uint32_t(hdr.fChunkSize);
        hdr.fNumChannels   = little_endian_uint16_t(hdr.fNumChannels);
        hdr.fSampleRate    = little_endian_uint32_t(hdr.fSampleRate);
        hdr.fSubchunk1Size = little_endian_uint32_t(hdr.fSubchunk1Size);
        hdr.fSubchunk2Size = little_endian_uint32_t(hdr.fSubchunk2Size);
    }
    else
    {
        hdr.fAudioFormat   = fAudioFormat;
        hdr.fBitsPerSample = fBitsPerSample;
        hdr.fBlockAlign    = fBlockAlign;
        hdr.fByteRate      = fByteRate;
        hdr.fChunkSize     = fChunkSize;
        hdr.fNumChannels   = N_CHANNELS;
        hdr.fSampleRate    = SAMPLE_RATE;
        hdr.fSubchunk1Size = fSubchunk1Size;
        hdr.fSubchunk2Size = fSubchunk2Size;
    }

    /******************************** 
    * Write the file format header. *
    ********************************/
    size_t ws = fwrite(&hdr, sizeof(hdr), 1, fout);

    if (ws != 1) 
    {
        printf("ERROR: Uanble to write the WAV header\n");
        fclose(fout);
        return false;
    }
    
    //If little endian do the sound conversion
    if (!little_endian) 
    {
        printf("smalll endiannnnn\n");
        for (int i = 0; i < pairAmount * N_CHANNELS; ++i) 
        {
            sound[i] = little_endian_float(sound[i]);
        }
    }

    /************************ 
    * Write the sound data. *
    ************************/
    ws = fwrite(sound, sizeof(float), pairAmount* N_CHANNELS, fout);
    fclose(fout);
    return ws == pairAmount * N_CHANNELS;
}

void record(uint64_t sample, float * sound, const char * filename)
{
    WriteWavePCM(sound,sample,filename);
}