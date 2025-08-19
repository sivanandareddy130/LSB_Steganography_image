#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Function to get the size of the image for a BMP file */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);  // Move the file pointer to the position where the width is stored (offset 18)

    fread(&width, sizeof(int), 1, fptr_image);  // Read the width of the image (4 bytes)
    printf("Width of the image: %u pixels\n", width);  // Modified message for width

    fread(&height, sizeof(int), 1, fptr_image);  // Read the height of the image (4 bytes)
    printf("Height of the image: %u pixels\n", height);  // Modified message for height
    
    // Return the image size (width * height * 3 for RGB format)
    return width * height * 3;
}

/* Function to open source image, secret file, and stego image */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");  // Open source image file in read mode
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");  // Print error if source image file cannot be opened
        fprintf(stderr, "ERROR: Unable to open source image file: %s\n", encInfo->src_image_fname);  // Modified message
        return e_failure;  // Return failure status
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");  // Open secret file in read mode
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");  // Print error if secret file cannot be opened
        fprintf(stderr, "ERROR: Unable to open secret file: %s\n", encInfo->secret_fname);  // Modified message
        return e_failure;  // Return failure status
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");  // Open the stego image file in write mode
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");  // Print error if stego image file cannot be opened
        fprintf(stderr, "ERROR: Unable to open stego image file: %s\n", encInfo->stego_image_fname);  // Modified message
        return e_failure;  // Return failure status
    }

    return e_success;  // Return success status
}

/* Function to validate the command-line arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if the source image file is a BMP file
    if (strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];  // Assign source image filename
    }
    else
    {
        return e_failure;  // Return failure if the source image is not a BMP file
    }

    // Check if the secret file is a TXT file
    if (strstr(argv[3], ".txt") != NULL)
    {
        encInfo->secret_fname = argv[3];  // Assign secret file filename
    }
    else
    {
        return e_failure;  // Return failure if the secret file is not a TXT file
    }

    // Check if the stego image filename is provided
    if (argv[4] != NULL)
        encInfo->stego_image_fname = argv[4];  // Assign stego image filename
    else
        encInfo->stego_image_fname = "stego.bmp";  // Default to "stego.bmp" if not provided

    return e_success;  // Return success status
}

/* Function to check if the image has enough capacity to store the secret file */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  // Get the image capacity
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);  // Get the size of the secret file
    
    // Check if the image has enough space for the secret file and headers
    if (encInfo->image_capacity >54+ (16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))
        return e_success;  // Return success if capacity is enough
    else
        return e_failure;  // Return failure if capacity is insufficient
}

/* Function to get the size of a file */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);  // Move the file pointer to the end of the file
    return ftell(fptr);  // Return the size of the file
}

/* Function to copy the BMP header from source image to stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);  // Move file pointer to the start of the source image
    char str[54];
    fread(str, 54, 1, fptr_src_image);  // Read the first 54 bytes of the BMP header
    fwrite(str, 54, 1, fptr_dest_image);  // Write the BMP header to the stego image
    printf("Successfully copied BMP header to stego image.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to encode a magic string into the image */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);  // Encode the magic string into the image
    printf("Magic string has been successfully encoded into the image.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to encode data (such as magic string, file extension) into the image */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)  // Iterate over each byte of data
    {
        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);  // Read 1 byte from source image
        encode_byte_to_lsb(data[i], encInfo->image_data);  // Encode the byte into the least significant bit
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);  // Write the modified byte to the stego image
    }
    return e_success;  // Return success status
}

/* Function to encode a byte of data into the least significant bits of the image */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)  // Loop over each bit of the byte
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);  // Set the LSB with the corresponding bit of data
    }
    return e_success;  // Return success status
}

/* Function to encode the size of the secret file extension */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);  // Encode the size of the file extension
    printf("Successfully encoded the size of the secret file extension.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to encode the size of data into the least significant bits */
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 32, 1, encInfo->fptr_src_image);  // Read 32 bytes from the source image
    for (int i = 0; i < 32; i++)  
    {
        str[i] = (str[i] & 0xFE) | ((size >> i) & 1);  // Set the LSB with the corresponding bit of size
    }
    fwrite(str, 32, 1, encInfo->fptr_stego_image);  // Write the modified bytes to the stego image
    return e_success;  // Return success status
}

/* Function to encode the secret file extension into the image */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);  // Encode the file extension into the image
    printf("Secret file extension encoded successfully into the image.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to encode the size of the secret file into the image */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);  // Encode the size of the secret file
    printf("Encoded the size of the secret file into the image.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to encode the actual secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char str[encInfo->size_secret_file];
    fseek(encInfo->fptr_secret, 0, SEEK_SET);  // Move the file pointer to the start of the secret file
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);  // Read the secret file data
    encode_data_to_image(str, encInfo->size_secret_file, encInfo);  // Encode the secret file data into the image
    printf("Encoded secret file data successfully.\n");  // Modified message
    return e_success;  // Return success status
}

/* Function to copy any remaining data from the source image to the stego image */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)  // Read the remaining bytes from the source image
    {
        fwrite(&ch, 1, 1, fptr_dest);  // Write the bytes to the stego image
    }
    printf("Remaining image data copied to stego image successfully.\n");  // Modified message
    return e_success;  // Return success status
}

/* Main function to perform encoding process */
Status do_encoding(EncodeInfo *encInfo)
{
    // Open the files (source image, secret file, stego image)
    if (open_files(encInfo) == e_success)
    {
        printf("Files opened successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to open files.\n");
        return e_failure;  // Return failure if file opening fails
    }

    // Check if the image has enough capacity for encoding
    if (check_capacity(encInfo) == e_success)
    {
        printf("Sufficient capacity in the image for encoding.\n");  // Modified message
    }
    else
    {
        printf("Insufficient capacity in the image.\n");
        return e_failure;  // Return failure if capacity check fails
    }

    // Copy the BMP header from source to stego image
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("BMP header copied successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to copy BMP header.\n");
        return e_failure;  // Return failure if header copy fails
    }

    // Encode the magic string into the image
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Magic string encoded successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to encode magic string.\n");
        return e_failure;  // Return failure if encoding magic string fails
    }

    // Extract file extension of the secret file
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.'));

    // Encode the secret file extension size
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
        printf("Secret file extension size encoded successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to encode secret file extension size.\n");
        return e_failure;  // Return failure if encoding extension size fails
    }

    // Encode the secret file extension itself
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("Secret file extension encoded successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to encode secret file extension.\n");
        return e_failure;  // Return failure if encoding extension fails
    }

    // Encode the secret file size
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size encoded successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to encode secret file size.\n");
        return e_failure;  // Return failure if encoding file size fails
    }

    // Encode the actual secret file data
    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data encoded successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to encode secret file data.\n");
        return e_failure;  // Return failure if encoding file data fails
    }

    // Copy any remaining data from the source image to the stego image
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Remaining data copied successfully.\n");  // Modified message
    }
    else
    {
        printf("Failed to copy remaining data.\n");
        return e_failure;  // Return failure if copying remaining data fails
    }

    return e_success;  // Return success if encoding process is completed successfully
}
