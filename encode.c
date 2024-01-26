#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)			//Function definition to find size of source file(.bmp)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);				//SEEK_SET => To set file pointer at 18th position

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);			//from 18th POSITION read sizeof(int)
    printf("\nImage Width ===> %u\n", width);			//and store it in width

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);			//Always file pointer will be at updates position 
    printf("Image Height ===> %u\n\n", height);			//from Updated position read sizeof(int) to find height

    // Return image capacity
    return width * height * 3;					//Multiplying by 3, to convert pixel to byte(each pixel => 3Byte)
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");			//Source file pointer name --> fptr_src_image
    // Do Error handling						
    if ( encInfo->fptr_src_image == NULL )						//"r" mode => If file exist it will return address else NULL
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);	//ERROR message if file doesn't exist

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");				//Secret file pointer name -> fptr_secret
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");			//Opening the stego file in write mode
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
/* Function Definition to read and validate Encoding Arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)		
{
    if (argv[2] != NULL && strstr(argv[2], ".bmp"))			//Checking whether argv[2] is .bmp file or not
    {									//strstr=>substring search(On success returns base address)
	encInfo -> src_image_fname = argv[2];				//If it is a (.bmp)file we have to store the name of (.bmp)file in src_image_fname
    }
    else
    {
	return e_failure;						//If argument 2 is not a (.bmp)file return e_failure
    }
    if(argv[3] != NULL && strchr(argv[3], '.'))				//Checking whether arg[3] is ((.c)file OR (.txt)file) OR not 
    {									//strchr => character search(On success it will return address else NULL)
	encInfo -> secret_fname = argv[3];				//argv[3] is a secret file and we have to store it in secret_fname
	strcpy(encInfo -> extn_secret_file, strchr(argv[3], '.')); 	//We need to take copy of extention, Storing only extension ((.txt) OR (.c))
    }
    else
    {
	return e_failure;
    }
    
    if(argv[4] != NULL)							//Checking argv[4] is PASSED (OR) NOT
    {
	if(argv[4] != NULL && strstr(argv[4], ".bmp"))			//If PASSED, then check whether it is a (.bmp)file or NOT
	{
	    encInfo -> stego_image_fname = argv[4];			//If it is (.bmp)file store it in here,
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {										//If argv[4] is not PASSED
	printf("OPTIONAL:Argument-4 not PASSED\n");
	encInfo->stego_image_fname = "wow.bmp";					//Storing default name
	encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");	//Function call to open default file
    }
    return e_success;
}
/* Function Definition to check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    /* Function call --> To find size of (.bmp)file and store it in image_capacity(structure member) */
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);		//fptr_secret => file pointer

    if( encInfo -> image_capacity > (54+2+4+4+4+(encInfo -> size_secret_file))*8 )/*(Header-54)(Magic-2)(sec_extn_size-4)(sec_extn-4)(secret_file_size-4)*8(											to store 1byte need 8bytes) */
    {
	return e_success;
    }
    return e_failure;
}
int get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);		//Setting file pointer at the end of the file(to get size of file)
    return ftell(fptr);			//return size
}
/* To Encode header --->Function definition */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];					//Declare a char buffer of 54 Bytes 
    fseek(fptr_src_image, 0, SEEK_SET);			//To set file pointer to 0th position again
    fread(buffer, 54, 1, fptr_src_image);		//reading 54byte and storing it in char buffer[]	
    fwrite(buffer, 54, 1, fptr_dest_image);		//writting 54 bytes into fptr_des_image
    return e_success;
}
/* To Encode MAGIC STRING "#*" ---->Function definition */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    /* Function call to Encode data(#*) to image */
    encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
}
/* Function definition ---> To Encode data(#*) to image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];					//Declaring one char buffer to store 8Bytes of data temporarily 	
    for(int i=0; i<size; i++)	
    {
	fread(buffer, 8, 1, fptr_src_image);		//Reading 8Bytes of data from source image and storing it in buffer 
	encode_byte_to_lsb(data[i], buffer);		//Function call --->To Encode byte(# OR *) to LSB of source image
	fwrite(buffer, 8, 1, fptr_stego_image);		//After Encoding byte..stroring it in stego image(destination image)
    }
    return e_success;
}
/* Function Definition ---> To Encode byte(# OR *) to LSB bit */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
	image_buffer[i] = ( (image_buffer[i] & 0xFE) | (data & (1<<i)) >> i );		//Logic to Encode byte to LSB
    }
    return e_success;
}
/* Function Definition ---> To Encode extention size (.bmp or .c) */
Status encode_extension_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];					//Declaring a char buffer of 32 Bytes 
    fread(buffer, 32, 1, fptr_src_image);		//Reading 32 Byte from source image
    encode_size_to_lsb(buffer, size);			//Function call (To Encode size to LSB)
    fwrite(buffer, 32, 1, fptr_stego_image);		//After Encoding size.. stroring it in stego image
    return e_success;
}
/* Function Definition ---> To Encode size to LSB */
Status encode_size_to_lsb(char *image_buffer, int data)
{
    for(int i=0; i<32; i++)
    {
	image_buffer[i] = ((image_buffer[i] & 0xFE) | (data & (1<<i)) >> i );	//Bitwise Operation to encode size
    }
    return e_success;
}
/* Function Definition ---> To Encode secret file extension (.txt) OR (.c) */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    /* Each byte of secret file extension Encoded one after other by calling function (encode _data_to_image) */
    encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}
/* Function Definition ---> Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];					//Declaring a char buffer of 32 Bytes
    fread(buffer, 32, 1, encInfo -> fptr_src_image);	//Reading 32 Bytes from source image
    encode_size_to_lsb(buffer, file_size);		//Function call to encode size to LSVB
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);	//Writing Encoded secret file size into destination image
    return e_success;
}
/* Function Definition ---> To Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);		//Setting file indicator at the begining of secret file
    char buffer[encInfo -> size_secret_file];		//Declaring a char buffer of (size_secret_file) Bytes
    /* Read the secret file data and store it in buffer */
    fread(buffer, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
    /* Function Call to Encode Secret file data */
    encode_data_to_image(buffer, encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}
/* Function Definition ---> To Encode remaining image data */ 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;					//Declaring a char variable ch
    while(fread(&ch, 1, 1, fptr_src) > 0)	//Reading data one after another from source image untill it reaches EOF and storing it in ch
    {
	fwrite(&ch, 1, 1, fptr_dest);		//Writing the data from ch to destination image
    }
    return e_success;
}
/* Function Definition ---> For Encoding (called in main()) (From here, other function's are called */
Status do_encoding(EncodeInfo *encInfo)					
{
    /* Function call ---> To check File opened successfully OR not (encInfo - contain the address) */
    /* Passing 3 files --> beautiful.bmp secret.txt stego.bmp */
    if( open_files(encInfo) == e_success )				
    {									
	printf("All Files Opened=============>> SUCCESSFULLY\n");
	/* Function call ---> To check Image has enough capacity to hold the secret data */
	if( check_capacity(encInfo) == e_success )
	{
	    printf("Checked Image-Capacity============>> SUCCESSFULLY\n");
	    /* Function call ---> To check source file Header copied to stego OR not */
	    if( copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success )	
	    {
		printf("Copied BMP-Header============>> SUCCESSFULLY\n");
		/*Function call ---> To encode MAGIC STRING "#*" */
		if( encode_magic_string( MAGIC_STRING, encInfo) == e_success )
		{
		    printf("Encoded MAGIC-STRING============>> SUCCESSFULLY\n");
		    /* Function call ---> To Encode Extension size */
		    if ( encode_extension_size(strlen(strstr(encInfo -> secret_fname, ".")), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
		    {
			printf("Encoded SECRET-FILE-EXTENSION-SIZE============>> SUCCESSFULLY\n");
			/* Function call ---> To Encode Secret file extension */
			if( encode_secret_file_extn(strstr(encInfo -> secret_fname, "."), encInfo) == e_success )
			{
			    printf("Encoded SECRET-FILE-EXTENSION============>> SUCCESSFULLY\n");
			    /* Function call ---> To Encode secret file size */
			    if( encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
			    {
				printf("Encoded SECRET-FILE-SIZE============>> SUCCESSFULLY\n");
				/* Function call ---> To Encode secret file data */
				if( encode_secret_file_data(encInfo) == e_success)
				{
				    printf("Encoded SECRET-FILE-DATA============>> SUCCESSFULLY\n");
				    /* Function call ---> To Encode Remaining Image data */
				    if( copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
				    {
					printf("Copied Remaining IMAGE-DATA============>> SUCCESSFULLY\n");
					return e_success;
				    }
				    else
					printf("Copying Remaining IMAGE-DATA============>> FAILURE!!!\n");
				}
				else
				    printf("Encoding SECRET-FILE-DATA============>> FAILURE!!!\n");
			    }
			    else
				printf("Encoding SECRET-FILE-SIZE============>> FAILURE!!!\n");
			}
			else
			    printf("Encoding SECRET-FILE-EXTENSION============>> FAILURE!!!\n");
		    }
		    else
			printf("Encoding SECRET-FILE-EXTENSION-SIZE============>> FAILURE!!!\n");
		}
		else
		    printf("Encoding MAGIC-STRING============>> FAILURE!!!\n");
	    }
	    else
		printf("Copying BMP-Header============>> FAILURE!!!\n");
	}
	else
	    printf("Checking Capacity============>> FAILURE!!!\n");
    }
    else
	printf("Open Files=============>> FAILURE!!!\n");
}
