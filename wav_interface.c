#include "wav_interface.h"
#pragma pack(0)

//Calculates the size of a file in bytes
long int findSize(const char *file_name)
{
    // opening the file in read mode
    FILE* fp = fopen(file_name, "r");
  
    // checking if the file exist or not
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }
  
    fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int res = ftell(fp);
  
    // closing the file
    fclose(fp);
  
    return res;
}
// Opens a WAV file
//DONT FORGET TO CLOSE MANUALLY
FILE *open_WAV(const char *filename)
{
    long int file_size = findSize(filename);
    // Pointer to the file to which we will write
    FILE *fout = fopen(filename, "r");
    file_format_header hdr;
    // Read the header of the file
    printf("Total file size is %ld bytes\n",file_size);
    size_t file_header_read = fread(&hdr, sizeof(file_format_header), 1, fout);
    if (file_header_read != 1)
    {
        printf("ERROR: Uanble to read the WAV header\n");
        fclose(fout);
    }
    printf("Header file size is %ld in bytes\n",file_header_read);
    printf("Size left for sound is %ld bytes\n",file_size - file_header_read);
    return fout;
}
//Writes to a WAV file, if such file is not yet created, creates the file and writes to it
bool record(uint64_t sample_size, float *sound, const char *file_name, const char *type)
{
    // If no sound buffer is given or no filename provided, stop execution
    if (sound == NULL || file_name == NULL)
    {
        printf("ERROR: No filename, or no sound buffer\n");
        return false;
    }
    // Pointer to the file to which we will write
    FILE *fout = fopen(file_name, type);
    // If file was not opened correctly, stop execution
    if (fout == NULL)
    {
        printf("ERROR: Cannot open the file: %s\n", file_name);
        return false;
    }

    file_format_header hdr;
    //Creates the header for the file
    bool little_endian = set_header(sample_size,&hdr);
    //Write the created header to the file
    size_t ws = fwrite(&hdr, sizeof(hdr), 1, fout);

    if (ws != 1)
    {
        printf("ERROR: Uanble to write the WAV header\n");
        fclose(fout);
        return false;
    }

    // If little endian do the sound conversion
    if (!little_endian)
    {
        printf("smalll endiannnnn\n");
        for (int i = 0; i < sample_size * hdr.fNumChannels; ++i)
        {
            sound[i] = little_endian_float(sound[i]);
        }
    }
    //Write the sound data
    ws = fwrite(sound, sizeof(float), sample_size * hdr.fNumChannels, fout);
    fclose(fout);
    return ws == sample_size * hdr.fNumChannels;
}

/*Reads 8 bytes (2 float values) to a sound buffer from the file f_in,
This function allows to have playback, without destroying the data in the wav file
*/
void read_from_wav(FILE* f_in, const char *filename, float *sound)
{
    // If no sound buffer is given or no filename provided, stop execution
    if (sound == NULL || filename == NULL)
    {
        printf("ERROR: No filename, or no sound buffer\n");
    }
    size_t file_sound = fread(sound, sizeof(float), 2, f_in);
    //printf("Sound file size is %ld in bytes\n",file_sound);
    if (file_sound != 2)
    {
        printf("ERROR: Uanble to read the WAV sound\n");
        fclose(f_in);
    }
}


