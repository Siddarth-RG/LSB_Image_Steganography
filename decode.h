#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_OUTPUT_FILE 20

typedef struct _DecodeInfo
{
    /* stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    long extn_size;
    long secret_size;

    /* output file info */
    char output_fname[MAX_OUTPUT_FILE];
    char *output_file_fname;
    FILE *fptr_output_file;

} DecodeInfo;


/* Decoding function prototypes */


/* Read and validate Decode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status_d do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status_d open_decode_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status_d decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension size*/
Status_d decode_secret_file_extnsize(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status_d decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status_d decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status_d decode_secret_file_data(DecodeInfo *decInfo);

/* Decode Function, which does the real decoding */
Status_d decode_data_from_image(char *data, long size, FILE *fptr_stego_image);

/* Decode a byte from LSB of image data array */
Status_d decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode a size from LSB of str array */
Status_d decode_size_from_lsb (long *data, char *image_buffer);

#endif

