#include "wav_interface.h"
#pragma pack(0)

/**
 * TODO: Functions to implement
 * ! Try to manage all that with double keys, ex: Ctrl+p
 * ! Create a structure for a file ro only calculate it's size once
 * ! Allow the loop playback
 */

// Calculates the size of a file in bytes
uint64_t findSize(const char *file_name)
{
    // opening the file in read mode
    FILE *fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL)
    {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    uint64_t res = ftell(fp);

    // closing the file
    fclose(fp);

    return res;
}

// Opens a WAV file
// DONT FORGET TO CLOSE MANUALLY
FILE *open_WAV(const char *filename)
{
    long int file_size = findSize(filename);
    if(file_size == -1)
    {

    }
    // Pointer to the file to which we will write
    FILE *fout = fopen(filename, "r");
    file_format_header hdr;
    // Read the header of the file
    printf("Total file size is %ld bytes\n", file_size);
    size_t file_header_read = fread(&hdr, sizeof(file_format_header), 1, fout);
    if (file_header_read != 1)
    {
        printf("ERROR: Uanble to read the WAV header\n");
        fclose(fout);
    }
    printf("Header file size is %ld in bytes\n", file_header_read);
    printf("Size left for sound is %ld bytes\n", file_size - file_header_read);
    return fout;
}

void create(const char * file_name,const char *type)
{
    FILE *fout = fopen(file_name, type);
    // If file was not opened correctly, stop execution
    if (fout == NULL)
    {
        printf("ERROR: Cannot open the file: %s\n", file_name);
    }

    file_format_header hdr;
    // Creates the header for the file
    set_header(10, &hdr);
    // Write the created header to the file
    size_t ws = fwrite(&hdr, sizeof(hdr), 1, fout);

    if (ws != 1)
    {
        printf("ERROR: Uanble to write the WAV header\n");
        fclose(fout);
    }
    fclose(fout);
}

// Writes to a WAV file, if such file is not yet created, creates the file and writes to it
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
    // Creates the header for the file
    bool little_endian = set_header(sample_size, &hdr);
    // Write the created header to the file
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
        for (uint64_t i = 0; i < sample_size * hdr.fNumChannels; ++i)
        {
            sound[i] = little_endian_float(sound[i]);
        }
    }
    // Write the sound data
    ws = fwrite(sound, sizeof(float), sample_size * hdr.fNumChannels, fout);
    fclose(fout);
    return ws == sample_size * hdr.fNumChannels;
}

/*Reads 8 bytes (2 float values) to a sound buffer from the file f_in,
This function allows to have playback, without destroying the data in the wav file
*/
int read_from_wav(ud *data)
{
    FILE *f_in = data->fout;
    float *sound = data->wav_manager->playback_buffer;
    // If no sound buffer is given or no filename provided, stop execution
    if (sound == NULL)
    {
        printf("ERROR:No Sound buffer\n");
    }
    size_t file_sound = fread(sound, sizeof(float), 2, f_in);
    // printf("Sound file size is %ld in bytes\n",file_sound);
    if (file_sound != 2)
    {
        printf("ERROR: Uanble to read the WAV sound\n");
        // Off the playback
        data->wav_manager->playback = 0;
        // Close the file
        fclose(f_in);
        data->fout = open_WAV("Bach.wav");
        return file_sound;
    }
    return file_sound;
}

/*
Goes to sec in a file if the length of the file does not exceed the timing
*/
// WORKS
void read_from_sec(FILE *fout, uint64_t file_size, double sec)
{
    // Pointer to the file to which we will write
    file_format_header hdr;
    // Read the header of the file
    printf("Total file size is %ld bytes\n", file_size);
    size_t file_header_read = fread(&hdr, sizeof(file_format_header), 1, fout);
    if (file_header_read != 1)
    {
        printf("ERROR: Uanble to read the WAV header\n");
        fclose(fout);
        exit(1);
    }
    printf("Header file size is %ld in bytes\n", file_header_read);
    printf("Size left for sound is %ld bytes\n", file_size - file_header_read);

    size_t new_playback_byte = sec * BYTES_PER_SECOND;
    if (new_playback_byte > file_size)
    {
        printf("You have exceeded the time of the file!");
        fclose(fout);
        exit(1);
    }
    // Set the read possition to offset
    fseek(fout, new_playback_byte, SEEK_SET);
}

/*
    Opens a file and does a loop playback on it (start to end)
    !DOES NOT WORK
*/
void loop_between(const char *filename, double start, double end)
{
    if (start > end)
    {
        printf("Start parameter is larger than end!\n");
        exit(1);
    }
    uint64_t file_size = findSize(filename);
    // Pointer to the file to which we will write
    FILE *fout = fopen(filename, "r");
    file_format_header hdr;
    // Read the header of the file
    printf("Total file size is %ld bytes\n", file_size);
    size_t file_header_read = fread(&hdr, sizeof(file_format_header), 1, fout);
    if (file_header_read != 1)
    {
        printf("ERROR: Uanble to read the WAV header\n");
        fclose(fout);
        exit(1);
    }
    printf("Header file size is %ld in bytes\n", file_header_read);
    printf("Size left for sound is %ld bytes\n", file_size - file_header_read);

    size_t new_playback_byte_start = start * BYTES_PER_SECOND;
    size_t new_playback_byte_end = end * BYTES_PER_SECOND;
    if (new_playback_byte_end > file_size)
    {
        printf("You have exceeded the time of the file!");
        fclose(fout);
        exit(1);
    }
    // Set the read possition to offset
    fseek(fout, new_playback_byte_start, SEEK_SET);
    /*
    while (ftell(fout) < new_playback_byte_end)
    {
        // read_from_wav(fout,sound);
    }
    */
    fclose(fout);
}

void set_new_working_wav_file(ud *data, const char *file_name)
{
    data->wav_file_name = file_name;
    data->fout = open_WAV(file_name);
    data->fout_size = findSize(file_name);
    printf("%s\n",data->wav_file_name);
}
