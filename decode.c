#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/*Defining function to get file names and validate
 *Input : stego.bmp file
 *output: data hidden in stego.bmp image is decoded in output.txt file
 *return value:d_success or d_failure, on file errors
 */

/* Function Definition ---> To read and validate arguments for decoding */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /* Checking argv[2] is PASSED or NOT */
    if( argv[2] != NULL )
    {
	if( strstr(argv[2], ".bmp") )			//Checking argv[2] is (.bmp)file or NOT
	{
	    decInfo -> stego_image_fname = argv[2];	//Assigning argv[2] to pointer stego_image_fname
	}
	else
	{
	    return d_failure;
	}
    }
    else
    {
	return d_failure;
    }
    /* Checking If argv[3] is PASSED or NOT */
    if( argv[3] != NULL )                                    
    {                                                    
	strcpy(decInfo -> output_fname, argv[3]);	//If not NULL Copy argv[3] to output_fname array
    }
    else
    {
	strcpy(decInfo -> output_fname, "output");	//Else Copy "output" to output_fname
    }
    return d_success;
}

/* Function Definition ---> To open files for decoding */
Status_d open_decode_files(DecodeInfo *decInfo)
{
    /* Opening stego image file in READ MODE */
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");

    /* Do Error Handling */
    if (decInfo -> fptr_stego_image == NULL)
    {
	perror("fopen");                             
	fprintf(stderr,"ERROR: Unable to open file %s\n", decInfo -> stego_image_fname);
	return e_failure;
    }
    return d_success;
}
/* Function Definition ---> To decode MAGIC-STRING */
Status_d decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    /* fseek --> To set the file indicator to point at 54th position in source image */
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    char magic_data[(strlen(magic_string) + 1)];	// Declaring char array-magic_data of size of (string length of magic string + 1) 

    /* Function Call(decode_data_from_image) ---> To Decode MAGIC-STRING from the stego image */
    decode_data_from_image(magic_data, strlen(magic_string), decInfo -> fptr_stego_image);
    magic_data[strlen(magic_string)] = '\0';		//Assigning NULL value to char array
    if( strcmp(magic_data, MAGIC_STRING) == 0 )		//Comparing MAGIC-STRING with the decoded MAGIC-STRING
    {
	return d_success;
    }
    else
    {
	return d_failure;
    }
}
/* Function Definition ---> To decode data from the stego image */
Status_d decode_data_from_image(char *data, long size, FILE *fptr_stego_image)
{
    int i;
    char buffer[8];					//Declaring a char buffer of 8 Bytes
    for(i=0; i<size; i++)
    {
	fread(buffer, 8, 1, fptr_stego_image);		//Reading 8Bytes from stego image and storing it in buffer

	/* Function Call(decode_byte_from_lsb) ---> To decode each character */
	decode_byte_from_lsb(&data[i], buffer);
    }
}
/* Function Definition ---> To decode each chacter from stego image */
Status_d decode_byte_from_lsb(char *data, char *image_buffer)
{
    int i;
    char ch=0x00;					//Initializing a char variable ch with 0x00
    for (i=0; i<8; i++)
    {
	ch = ch | ((image_buffer[i] & 1) << i);		//Bitwise Operation to get each character
    }
    *data=ch;
}
/* Function Definition ---> To decode secret file extn size from the stego image */
Status_d decode_secret_file_extnsize(DecodeInfo *decInfo)
{
    char buffer[32];             			//Declaring a char buffer of size 32Bytes

    fread(buffer, 32, 1, decInfo -> fptr_stego_image);	//Reading 32 Bytes from stego image and stroring it in buffer

    /* Function Call ---> To decode size from the stego image */
    decode_size_from_lsb(&decInfo -> extn_size, buffer);
    return d_success;
}
/* Function Definition ---> To decode size from the stego image */
Status_d decode_size_from_lsb (long *data, char *image_buffer)
{
    int i;
    char ch=0x00;					//Initializing a char variable ch with 0x00
    for (i=0; i<32; i++)
    {
	ch = ch | ((image_buffer[i] & 1) << i);		//Bitwise Operation to get size 
    }
    *data=ch;
}
/* Function Definition ---> To decode secret file extn from stego image */
Status_d decode_secret_file_extn(DecodeInfo *decInfo)
{
    char extn_data[decInfo -> extn_size];		//declare char array EXTN_DATA of size EXTN_SIZE

    /* Function Call(decode_data_from_image) ---> To decode secret file extn from the stego image */
    decode_data_from_image(extn_data, decInfo -> extn_size, decInfo -> fptr_stego_image);

    /* Concatinate(strcat) the output_fname(output) with extn_data((.txt) OR (.c)) and assign it to output_file_fname(output.txt OR output.c) */
    decInfo -> output_file_fname = strcat(decInfo -> output_fname, extn_data);

    /* Opening the file output_file_fname in WRITE MODE */
    decInfo -> fptr_output_file = fopen(decInfo -> output_file_fname, "w");

    /* Do Error Handling */
    if( decInfo -> fptr_output_file == NULL )
    {
	perror("fopen");
	fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo -> output_fname);
	return d_failure;
    }
    return d_success;
}
/* Function Definition ---> To decode secret file size from the stego image */
Status_d decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];                			//Declaring a char buffer of size 32Bytes

    fread(buffer, 32, 1, decInfo -> fptr_stego_image);	//Read 32 Bytes from the stego image and store it in buffer

    /* Function Call(decode_size_from_lsb) ---> To decode size from the stego image */
    decode_size_from_lsb(&decInfo -> secret_size, buffer);
    return d_success;
}
/* Function Definition ---> To decode secret data from the stego image */
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    char secret_data[decInfo -> secret_size];		//Declaring a char array SECRET_DATA of size SECRET_SIZE

    /* Function Call(decode_data_from_image) ---> To decode secret data from the stego image */
    decode_data_from_image(secret_data, decInfo -> secret_size, decInfo -> fptr_stego_image);

    /* Write the secret_data into output file */
    fwrite(secret_data, decInfo -> secret_size, 1, decInfo -> fptr_output_file);
    return d_success;
}

/* Function Definition ---> To do Decoding */
Status_d do_decoding(DecodeInfo *decInfo)
{
    /* Function Call ---> To check files opened successfully or not */
    if( open_decode_files(decInfo) == d_success )
    {
	printf("Opened Files============>> SUCCESSFULLY\n");

	/* Function Call ---> To check whether the MAGIC-STRING decoded successfully or not */
	if( decode_magic_string(MAGIC_STRING, decInfo) == d_success )
	{
	    printf("Decoded MAGIC-STRING============>> SUCCESSFULLY\n");

	    /* Function Call ---> To check whether the Extension Size decoded or not */
	    if( decode_secret_file_extnsize(decInfo) == d_success )
	    {
		printf("Decoded FILE-EXTENSION-SIZE============>> SUCCESSFULLY\n");

		/* Function Call ---> To check whether the Extension data is decoded or not */
		if( decode_secret_file_extn(decInfo) == d_success )
		{
		    printf("Decoded SECRET-FILE-EXTENSION============>> SUCCESSFULLY\n");

		    /* Function Call ---> To check Secret file size is decoded or not */
		    if( decode_secret_file_size(decInfo) == d_success )
		    {
			printf("Decoded SECRET-FILE-SIZE============>> SUCCESSFULLY\n");

			/* Function Call ---> To check Secret file data is decoded or not */
			if( decode_secret_file_data(decInfo) == d_success )
			{
			    printf("Decoded SECRET-FILE-DATA============>> SUCCESSFULLY\n");
			}
			else
			    printf("Decoding of SECRET-FILE-DATA============>> FAILURE!!!\n");
		    }
		    else
			printf("Decoding of SECRET-FILE-SIZE============>> FAILURE!!!\n");
		}
		else
		    printf("Decoding of SECRET-FILE-EXTENSION============>> FAILURE!!!\n");
	    }
	    else
		printf("Decoding of FILE-EXTENSION-SIZE============>> FAILURE!!!\n");
	}
	else
	    printf("Decoding of MAGIC-STRING============>> FAILURE!!!\n");
    }
    else
	printf("Open FILES============>> FAILURE!!!\n");

    return d_success;
}

