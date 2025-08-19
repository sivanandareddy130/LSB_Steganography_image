/*
Name : C Sivananda reddy;
Project : Steganography;
*/


#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char *argv[])
{
    // Check if there are enough command-line arguments
    if (argc < 4)
    {
        // If not, display an error message and show the correct usage

        printf("Invalid input\n");
        printf("For encoding: ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For decoding: ./a.out -d stego.bmp [decode.txt]\n");
        return 1;
    }

    // Determine the operation type: encoding or decoding
    OperationType op_type = check_operation_type(argv);
    
    // If the operation is encoding
    if (op_type == e_encode)
    {
        printf(":::Selected encoding:::\n");
        EncodeInfo encode;
        
        // Validate encoding arguments
        if (read_and_validate_encode_args(argv, &encode) == e_success)
        {
            printf("Read and validate encode arguments is a success\n");
            
            // Perform the encoding
            if (do_encoding(&encode) == e_success)
            {
                printf("Encoding is successful\n");
            }
            else
            {
                // If encoding fails, print an error message and return -4

                printf("Encoding Performance is failed\n");
                return -4;
            }
        }
        else
        {
            // If argument validation for encoding fails, print an error message and return -3
            printf("Read and validate that the encode argument has failed\n");
            return -3;
        }
    }
    // If the operation is decoding
    else if (op_type == e_decode)
    {
        printf(":::Selected decoding:::\n");
        DecodeInfo decode;
        
        // Validate decoding arguments
        if (read_and_validate_decode_args(argv, &decode) == e_success)
        {
            printf("Read and validate decode arguments is a success\n");
            
            // Perform the decoding
            if (do_decoding(&decode) == e_success)
            {
                printf("Decoding is successful\n");
            }
            else
            {
                // If decoding fails, print an error message and return -4
                printf("Decoding performance is failed\n");
                return -4;
            }
        }
        else
        {
            // If argument validation for decoding fails, print an error message and return -3
            printf("Read and validate that the decode argument has failed\n");
            return -3;
        }
    }
    else
    {
        // Print the invalid input error and display usage information
        printf("Invalid input\n");
        printf("For encoding: ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For decoding: ./a.out -d stego.bmp [decode.txt]\n");
        return 1;
    }

    return 0;
}

// Function to check if the operation is encoding, decoding, or unsupported
OperationType check_operation_type(char *argv[])
{
    // Check if the operation is encoding (-e)
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    // Check if the operation is decoding (-d)
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    // If neither, return unsupported
    else
        return e_unsupported;
}
