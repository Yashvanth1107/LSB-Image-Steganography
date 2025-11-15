#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("Opening Files Done...\n");
        if(check_capacity(encInfo)==e_success)
        {
            printf("Checking the capacity done...\n");
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) ==  e_success)
            {
                printf("Header Copied Successfully...\n");
                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                {
                    printf("Encoded Magic string Successfully...\n");
                    if(encode_secret_file_extn_size(encInfo->extn_secret_file,encInfo)==e_success)
                    {
                        printf("Encoded secret File extention Size Successfully...\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        {
                            printf("Encoded secret File extention Successfully...\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
                            {
                                printf("Encoded secret File Data Successfully...\n");
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
                                    {
                                        printf("Copied Remaining Data...\n");
                                        if(close_all_files(encInfo)== e_success)
                                        {
                                            printf("Files Closing Done...\n");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

Status close_all_files(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    unsigned char ch[1];
    while(fread(ch,1,1,fptr_src)>0)
    fwrite(ch,1,1,fptr_dest);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(encInfo->secret_data,1,1,encInfo->fptr_secret);
    for(int i=0;i<encInfo->size_secret_file;i++,fread(encInfo->secret_data,1,1,encInfo->fptr_secret))
    {
        encode_data_to_image(encInfo->secret_data,MAX_SECRET_BUF_SIZE,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buff[32];
    fread(image_buff,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,image_buff);
    fwrite(image_buff,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn_size(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buff[32];
    fread(image_buff,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(strlen(file_extn),image_buff);
    fwrite(image_buff,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=((image_buffer[i] & ~(1)) | ((size >> (31-i)) & 1));
    }
    uint size11=0;
    for(int i=0;i<=31;i++)
    {
        size11 = size11 | ((image_buffer[i] & 1) << (31-i));
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Getting the size of the image.
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Getting the size of the secret file.
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity >= (54 + (strlen(MAGIC_STRING) + 4 + 4 + 4 + encInfo->size_secret_file)*8))
    {
        return e_success;
    }
    return e_failure;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,1,8,fptr_src_image);
        encode_byte_to_lsb(data[i],image_buffer);
        fwrite(image_buffer,1,8,fptr_stego_image);
    }  
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=((image_buffer[i] & ~(1)) | ((data >> (7-i)) & 1));
    }
    // Decoding
    unsigned char ch2=0;
    for(int i=0;i<=7;i++)
    {
        ch2 = ch2 | ((image_buffer[i] & 1) << (7-i));
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint size = ftell(fptr);
    printf("Size of Secret File : %u\n",size);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char header[54];
    if(fread(header,1,54,fptr_src_image)==54)
    {
        fwrite(header,1,54,fptr_dest_image);
        return e_success;
    }
    return e_failure;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    rewind(fptr_image);
    // Return image capacity
    return width * height * 3;
}


Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}
