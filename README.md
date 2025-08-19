# LSB_Steganography_image
I am excited to share my project “LSB Image Steganography”, developed during my learning journey with Emertxe Information Technologies. This project focuses on the concept of hiding sensitive information within digital images using the Least Significant Bit (LSB) technique, a widely used method in steganography.
LSB Image Steganography

This project implements Least Significant Bit (LSB) Image Steganography using the C programming language. It was developed as part of my learning journey with Emertxe Information Technologies. The project focuses on securely hiding sensitive data inside BMP images without affecting their visual quality.

🔍 Project Overview

Steganography is the art of hiding information within digital media. This project uses the LSB technique, where the least significant bits of pixel data in a BMP image are modified to embed secret information. Since these changes are imperceptible to the human eye, the cover image looks visually identical to the original.

The project provides two key functionalities:

Encoding: Hide a secret message or file inside a BMP image.

Decoding: Extract the hidden message/file from the stego-image.

⚙️ Features

Secure data hiding inside BMP images

No visible distortion in the output stego-image

Encode and decode using a console-based C program

Support for text and small files as hidden data

Hands-on implementation of file handling, bitwise operations, and image processing in C

📂 Project Structure
├── src/            # Source code files (.c, .h)
├── images/         # Sample BMP images
├── secret/         # Secret messages/files
├── output/         # Stego images generated
├── Makefile        # Build automation
└── README.md       # Project documentation

🚀 Getting Started
Prerequisites

GCC Compiler

Linux/Windows environment with terminal


Run
Encode data:
./steganography -e cover.bmp secret.txt stego.bmp

Decode data:
./steganography -d stego.bmp output.txt

📸 Example

Input Cover Image → cover.bmp

Secret File → secret.txt

Output Stego Image → stego.bmp (looks identical but contains hidden data)

📖 Learning Outcomes

Understanding of steganography & data hiding techniques

Practical use of bitwise operators in C

File I/O handling with binary data

Image processing concepts

🛡️ Applications

Secure data transfer

Digital watermarking

Copyright protection

Covert communication

👤 Author

Developed by Sivananda Reddy during training at Emertxe Information Technologies.
