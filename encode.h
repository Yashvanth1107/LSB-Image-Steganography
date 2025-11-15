#ifndef ENCODE_H
#define ENCODE_H
#include <string.h>
#include "common.h"
#include "types.h" // Contains user defined types
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and in termediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo);

Status encode_secret_file_extn_size(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

Status encode_size_to_lsb(int size, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

Status close_all_files(EncodeInfo *encInfo);


//DECODING PART

typedef struct 
{
    char *decode_image_fname;
    FILE *fptr_decode_image;

    char decoded_magic_string[3];
    // char *output_secret_file_ext;
    // char *output_secret_file_fname;

    char output_secret_file_fname[25];

    uint secret_file_extn_size;

    long output_file_size;

    FILE *fptr_output_file;

}DecodeInfo;

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo,char* arr);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(  DecodeInfo *decInfo ,char *arr);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo, char *arr);

Status decode_data_to_image(int size,FILE *fptr_decode_image,char *arr);

// Status decode_data_to_image(int size,FILE *fptr_decode_image,DecodeInfo *decInfo,char *arr);

Status decode_byte_from_lsb(char *image_buffer);

//Status decode_byte_from_lsb(char *image_buffer,DecodeInfo *decInfo);

Status decode_size_from_lsb(int *size, char *image_buffer);

Status close_all_decoding_files(DecodeInfo *decInfo);

#endif
