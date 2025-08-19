#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

Status Open_files(DecodeInfo *decInfo)
{
    // Try to open the stego image file in read mode ('rb' for binary)
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    
    // If unable to open the stego image file, return failure
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: Failed to open the stego image file: %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the stego image file has a .bmp extension
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2]; // Assign the filename
    }
    else
    {
        printf("Error: The stego image file must have a .bmp extension.\n");
        return e_failure;
    }

   

    


     // Check if magic string is provided
    if (argv[3] != NULL)
    {
        strcpy(decInfo->magic_string ,argv[3]); // Assign magic string
    }
    else
    {
        printf("Error: Magic string argument missing.\n");
        return e_failure;
    }
    // Check if output secret filename is provided, else use default
   

    if (argv[4] != NULL)
{
        strcpy(decInfo->secret_fname, "decode"); // Default filename

    // Check if filename contains a dot (.)
    if (strchr(argv[4], '.') != NULL)
    {
        printf("Invalid file name. Do not include extension. Use: ./a.out -d stego.bmp siva\n");
        return e_failure;
    }

    strcpy(decInfo->secret_fname, argv[4]);
}
else
{
    strcpy(decInfo->secret_fname, "decode"); // Default filename
}

    return e_success; // All validations passed
}


Status decode_magic_string(char *user_magic_string, DecodeInfo *decInfo)
{
    // The actual expected embedded magic string
    const char *expected_magic = "#*";

    // Move file pointer to BMP pixel data start
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    // Prepare buffer to read magic string
    char decoded_magic[3]; // 2 chars + null terminator
    decode_data_to_image(decoded_magic, 2, decInfo->fptr_stego_image, decInfo);
    decoded_magic[2] = '\0'; // Null-terminate

    // Check if decoded string matches expected embedded magic
    if (strcmp(decoded_magic, expected_magic) != 0)
    {
        printf("Error: Decoded magic string does not match embedded magic.\nExpected: \"%s\", Found: \"%s\"\n",
               expected_magic, decoded_magic);
        return e_failure;
    }

    // Now validate if user input magic matches embedded one
    if (strcmp(user_magic_string, expected_magic) != 0)
    {
        printf("Error: User-entered magic string is incorrect.\nExpected: \"%s\", Got: \"%s\"\n",
               expected_magic, user_magic_string);
        return e_failure;
    }

    printf("Successfully decoded the magic string.\n");
    return e_success;
}


Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    int i;
    char image_buffer[8]; // Buffer to read 8 bits at a time from image

    // Loop to read and decode each byte of data
    for (i = 0; i < size; i++)
    {
        // Read 8 bits from the image
        fread(image_buffer, 8, 1, fptr_stego_image);
        
        // Decode the 8 bits into a character and store it in 'data'
        data[i] = (decode_lsb_to_byte(data[i], image_buffer));
    }
    
    // Null-terminate the decoded string
    data[i] = '\0';
    return e_success;
}

Status decode_lsb_to_byte(char data, char *image_buffer)
{
    char ch = 0;

    // Loop over each bit of the 8-byte chunk and extract the LSB
    for (int i = 0; i < 8; i++)
    {
        ch = ((image_buffer[i] & 1) << i) | ch; // Shift and combine bits
    }

    return ch;
}

Status decode_size_to_lsb(char *buffer, int *data)
{
    *data = 0;

    // Loop through the 32 bits and reconstruct the size value
    for (int i = 0; i < 32; i++)
    {
        *data = ((buffer[i] & 1) << i) | *data;
    }

    return e_success;
}

Status decode_secret_file_extn_size(int *size, FILE *fptr_stego_image)
{
    char buffer[32];
    
    // Read 32 bits to decode the size of the secret file extension
    fread(buffer, 32, 1, fptr_stego_image);
    
    // Decode the size from the LSB
    decode_size_to_lsb(buffer, size);
    
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Allocate buffer to store the decoded extension
    char extn[decInfo->size_secret_file_extn];
    
    // Decode the extension data from the image and store in 'extn'
    if (decode_data_to_image(extn, decInfo->size_secret_file_extn, decInfo->fptr_stego_image, decInfo) == e_success)
    {
        // Append the extension to the secret filename
        strcat(decInfo->secret_fname, extn);
        
        // Open the secret file for writing with the decoded extension
        decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
        // decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

    // If unable to open the secret file for writing, return failure
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: Failed to open the secret file for writing: %s\n", decInfo->secret_fname);
        return e_failure;
    }
        
        return e_success;
    }
    return e_failure; // If decoding extension fails, return failure
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Decode the size of the secret file
    decode_secret_file_extn_size(&(decInfo->decode_size_secret_file), decInfo->fptr_stego_image);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Allocate buffer to store the secret file data
    char data[decInfo->decode_size_secret_file];
    
    // Decode the secret file data from the stego image
    decode_data_to_image(data, decInfo->decode_size_secret_file, decInfo->fptr_stego_image, decInfo);
    
    // Write the decoded data into the secret file
    fprintf(decInfo->fptr_secret, "%s", data);
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{

    printf("Selected decoding:\n");


    if (Open_files(decInfo) == e_failure)
    {
        printf("Error: Could not open files.\n");
        return e_failure;
    }
    printf("Files opened successfully for decoding.\n");

    if (decode_magic_string(decInfo->magic_string, decInfo) == e_failure)
    {
        printf("Decoding failed due to incorrect or mismatched magic string.\n");
        return e_failure;
    }

    
    // Decode the secret file extension size
    if (decode_secret_file_extn_size(&(decInfo->size_secret_file_extn), decInfo->fptr_stego_image) == e_success)
    {
        printf("Successfully decoded the secret file extension size.\n");
    }
    else
    {
        // If extension size decoding fails, return failure
        printf("Error: Failed to decode the secret file extension size.\n");
        return e_failure;
    }

    // Decode the secret file extension
    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("Successfully decoded the secret file extension.\n");
    }
    else
    {
        // If extension decoding fails, return failure
        printf("Error: Failed to decode the secret file extension.\n");
        return e_failure;
    }

    // Decode the size of the secret file
    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("Successfully decoded the secret file size.\n");
    }
    else
    {
        // If file size decoding fails, return failure
        printf("Error: Failed to decode the secret file size.\n");
        return e_failure;
    }

    // Decode the actual secret file data
    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("Successfully decoded the secret file data.\n");
    }
    else
    {
        // If data decoding fails, return failure
        printf("Error: Failed to decode the secret file data.\n");
        return e_failure;
    }

    // Return success if all decoding steps are successful
    return e_success;
}
