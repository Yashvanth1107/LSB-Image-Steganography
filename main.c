#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    if (argc < 3)
    {
        printf("Enter the Arguments❗\n");
        printf("For Encoding args : ./a.out -e filename.bmp secret.txt\n");
        printf("For Decoding args : ./a.out -d encoded_image.bmp\n");
        return 1;
    }
    else
    {
        OperationType res = check_operation_type(argv);
        if(res == e_encode)
        {
            
            if(argc < 3)
            {
                printf("Enter the Arguments❗\n");
                printf("For Encoding args : ./a.out -e filename.bmp secret.txt\n");
                return e_failure;
            }
            printf("Encoding Operation Enabled...\n");
            Status ret = read_and_validate_encode_args(argv,&encInfo);
            if(ret==e_success)
            {
                printf("Arguments Verified SuccessFully...\n");
                do_encoding(&encInfo);
            }
        }
        else if(res==e_decode)
        {
            printf("Decoding Operation Enabled...\n");
            if(read_and_validate_decode_args(argv,&decInfo) == e_success)
            {
                printf("Arguments Verified SuccessFully...\n");
                do_decoding(&decInfo);
            }
        }
        else
        {
            printf("Invalid Operation type\n");
            return 1;
        }  
    }
    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))
    {
        return e_encode;
    }
    else if(!strcmp(argv[1],"-d"))
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    
    if(strstr(argv[2],".bmp") && argv[2] != NULL)
    {    
        encInfo->src_image_fname = argv[2];
        if(argv[3] == NULL)
        {
            printf("Enter the Arguments❗\n");
            printf("For Encoding args : ./a.out -e filename.bmp secret.txt\n");
            return e_failure;
        }
        
        char *ptr = strstr(argv[3],".txt");
        if(ptr)
        {
            strcpy(encInfo->extn_secret_file,ptr);
            encInfo->secret_fname=argv[3];
            if(argv[4]==NULL)
            {
                encInfo->stego_image_fname="stego.bmp";
                return e_success;
            }
            else
            {
                if(strstr(argv[4],".bmp"))
                {
                    encInfo->stego_image_fname=argv[4];
                    return e_success;
                }
                else
                {
                    printf("Please Provide with extention .bmp\n");
                    return e_failure;
                }
            }
        }
        else
        {
            printf("Please Provide  .txt File\n");
            return e_failure;
        }
    }
    else
    {
        printf("Please provide .bmp file\n");
        return e_failure;
    }
}

