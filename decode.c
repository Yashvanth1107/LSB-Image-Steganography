#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"

static unsigned char ch;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp"))
    {
        decInfo->decode_image_fname = argv[2];
        decInfo->fptr_decode_image = fopen(decInfo->decode_image_fname,"r");
        if(decInfo->fptr_decode_image == NULL)
        {
            printf("Opening the file Failed\n");
            return e_failure;
        }
        if(argv[3] == NULL)
        {
            strcpy(decInfo->output_secret_file_fname,"output");
            return e_success;
        }
        else
        {
            strcpy(decInfo->output_secret_file_fname, strtok(argv[3],"."));
            return e_success;
        }
    }
    else 
    {
        return e_failure;
    }
}

Status do_decoding(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_decode_image,54,SEEK_SET);
    char *arr = malloc(1*sizeof(char));
    int size=0; 
    if(decode_magic_string(decInfo,arr)==e_success)
    {
        printf("Decoded Magic string Successfully...\n");
        if(decode_secret_file_extn_size(decInfo)==e_success)
        {
            printf("Decoded Secret File Extension Size Successfully...\n");
            if(decode_secret_file_extn(decInfo,arr) == e_success)
            {
                printf("Decoded Secret File Extension Successfully...\n");
                if(decode_secret_file_size(decInfo)== e_success)
                {
                    printf("Decoded Secret File Size Successfully...\n");
                    if(decode_secret_file_data(decInfo,arr) == e_success)
                    {
                        printf("Decoded Secret File Data Successfully...\n");
                        if(close_all_decoding_files(decInfo) == e_success)
                        {
                            printf("Closing All the Files...\n");
                            printf("Decoding Done Successfully...\n");
                        }
                    }
                }
            }
        }
    }
}


Status close_all_decoding_files(DecodeInfo *decInfo)
{
    fclose(decInfo->fptr_decode_image);
    fclose(decInfo->fptr_output_file);
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo , char *arr)
{
    decInfo->fptr_output_file = fopen(decInfo->output_secret_file_fname, "w");
    if(decInfo->fptr_output_file == NULL)
    {
        printf("Opening the file is failed\n");
        return e_failure;
    }
    arr = realloc(arr,decInfo->output_file_size*sizeof(char));  
    decode_data_to_image(decInfo->output_file_size,decInfo->fptr_decode_image,arr);
    for(int i=0;i<decInfo->output_file_size;i++)
    {
        fprintf(decInfo->fptr_output_file,"%c",arr[i]);
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    int size=0;
    fread(image_buffer,1,32,decInfo->fptr_decode_image);
    decode_size_from_lsb(&size,image_buffer);
    decInfo->output_file_size = size;
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo, char *arr)
{
    arr = realloc(arr,decInfo->secret_file_extn_size*sizeof(char));  
    decode_data_to_image(decInfo->secret_file_extn_size,decInfo->fptr_decode_image,arr);
    strcat(decInfo->output_secret_file_fname,arr);
    return e_success;
}


Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    int size=0;
    fread(image_buffer,1,32,decInfo->fptr_decode_image);
    decode_size_from_lsb(&size,image_buffer);
    decInfo->secret_file_extn_size = size;
    return e_success;
}

Status decode_size_from_lsb(int *size,char *image_buffer)
{
    int s=0;
    for(int i=0;i<=31;i++)
    {
        s = s | ((image_buffer[i] & 1) << (31-i)); 
    }
    *size = s;
}

Status decode_magic_string(DecodeInfo *decInfo, char *arr)
{
    arr = realloc(arr,(strlen(MAGIC_STRING))*sizeof(char)); 
    decode_data_to_image(strlen(MAGIC_STRING),decInfo->fptr_decode_image,arr);
    strcpy(decInfo->decoded_magic_string,arr);
    if(!strcmp(MAGIC_STRING,decInfo->decoded_magic_string))
    {
        printf("Magic String Verified Sucessfully...\n");
        return e_success;
    }
    else
    {
        printf("Magic String Does Not Matched\n");
        return e_failure;
    }
}

Status decode_data_to_image(int size, FILE *fptr_decode_image, char* arr)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,1,8,fptr_decode_image);
        decode_byte_from_lsb(image_buffer);
        arr[i]= ch;
    }   
}

Status decode_byte_from_lsb(char *image_buffer)
{
    ch=0;
    for(int i=0;i<=7;i++)
    {
        ch = ch | ((image_buffer[i] & 1) << (7-i)); 
    }
}