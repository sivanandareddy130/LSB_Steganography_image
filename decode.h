#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char secret_fname[50];
    FILE *fptr_secret;
    //char *secret_file_extn[MAX_FILE_SUFFIX];
    int decode_size_secret_file;
    int size_secret_file_extn;
    char magic_string[3];
    

} DecodeInfo;



/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status Open_files(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/*Encode the secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file extension size */
Status decode_secret_file_extn_size(int *size, FILE *fptr_stego_image);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/*Decode a LSB into byte image data array*/
Status decode_lsb_to_byte( char data,char *image_buffer);

/*Decode secret file extension size*/
Status decode_size_to_lsb(char *buffer, int *data);


#endif

