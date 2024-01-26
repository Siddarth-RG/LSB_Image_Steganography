/* Documentation
   Name               : Siddarth RG
   Date               : 07 JAN 2023
   Description        : LSB IMAGE Steganography (OR) Digital Steganography
   Sample Execution   :

(1)FOR ENCODING: $ ./a.out -e beautiful.bmp secret.txt <OPTIONAL:stego.bmp>
<<===================Selected EncodingG=====================>>
<<============Read and Validation Done Successfully===========>>

<<============Started EncodingG============>>

All Files Opened=============>> SUCCESSFULLY

Image Width ===> 1024
Image Height ===> 768

Checked Image-Capacity============>> SUCCESSFULLY
Copied BMP-Header============>> SUCCESSFULLY
Encoded MAGIC-STRING============>> SUCCESSFULLY
Encoded SECRET-FILE-EXTENSION-SIZE============>> SUCCESSFULLY
Encoded SECRET-FILE-EXTENSION============>> SUCCESSFULLY
Encoded SECRET-FILE-SIZE============>> SUCCESSFULLY
Encoded SECRET-FILE-DATA============>> SUCCESSFULLY
Copied Remaining IMAGE-DATA============>> SUCCESSFULLY

<<============Encoding is Done Successfully============>>

(2)FOR DECODING: $ ./a.out -d stego.bmp <OPTIONAL:decode.txt>
<<============Selected DecodingG============>>

<<============Read and Validation Done Successfully============>>

<<=============Started DecodingG============>>

Opened Files============>> SUCCESSFULLY
Decoded MAGIC-STRING============>> SUCCESSFULLY
Decoded FILE-EXTENSION-SIZE============>> SUCCESSFULLY
Decoded SECRET-FILE-EXTENSION============>> SUCCESSFULLY
Decoded SECRET-FILE-SIZE============>> SUCCESSFULLY
Decoded SECRET-FILE-DATA============>> SUCCESSFULLY

<<============Decoding is Done Successfully============>>

*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Passing arguments through command line argument */
int main(int argc, char *argv[])
{
    /* Checking command line argumnet passed or not */
    if(argc != 1)
    {
	/* Function call for check operation type */
	int operation = check_operation_type(argv);					//Function call(passing 1 arg, i.e., argv[])
	/* To check Opration (ENCODING OR DECODING) */
	if(operation == e_encode)							//Validating condition and printing respective result
	{
	    printf("\n<<============Selected EncodingG============>>\n");
	    /* For Encoding 3 arguments mandatory */
	    if( argc > 3)
	    {
		EncodeInfo encInfo;							//Structure (structure variable name --> encInfo)
		/* Function call to read and validate Encode Arguments */
		/* If read and validate return e_success means, successfull */
		if( read_and_validate_encode_args(argv, &encInfo) == e_success )			
		{
		    printf("<<============Read and Validation Done Successfully===========>>\n");
		    printf("\n<<============Started EncodingG============>>\n\n");

		    /* Function call (for ENCODING) (passing structure variable address) */
		    if( do_encoding(&encInfo) == e_success )				
		    {
			printf("\n<<============Encoding is Done Successfully============>>\n\n");
		    }
		    else
		    {
			printf("!!!!!!Encoding Failed!!!!!!\n");
		    }
		}
		else
		{
		    printf("<<============Read and Validation is FAILED!!!============>>\n");
		}
	    }
	    else
	    {
		printf("ERROR!! Please pass command line arguments \nUsage : ./a.out -e beautiful.bmp secret.txt / secret.c <stego.bmp>\n");
	    }
	}
	/* If check_operation_type returns e_decode means, USER selected Decoding */
	else if( operation == e_decode)
	{
	    printf("<<============Selected DecodingG============>>\n\n");
	    if( argc > 2 )
	    {
		/* Structure Declaration (FOR DECODING) */
		DecodeInfo decInfo;
		/* Function call to read and validate Decoding Arguments */		
		if( read_and_validate_decode_args(argv, &decInfo) == d_success)
		{
		    printf("<<============Read and Validation Done Successfully============>>\n");
		    printf("\n<<=============Started DecodingG============>>\n\n");
		    /* DECODING PART */
		    if( do_decoding(&decInfo) == d_success )
		    {
			printf("\n<<============Decoding is Done Successfully============>>\n\n");
		    }
		    else
		    {
			printf("!!!!!!Decoding FAILED!!!!!!\n");
		    }
		}
		else
		{
		    printf("<<============Read and Validation is FAILED!!!============>>\n");
		}
	    }
	    else
	    {
		printf("ERROR!! Please pass command line arguments \nUsage : ./a.out -d  stego.bmp  <output-file>\n");
	    }
	}
	else if( operation != e_encode || operation != e_decode )
	{
	    printf("ERROR!! : please pass <-e>--->ENCODING\nERROR!! : please pass <-d>--->DECODING\n");
	}
    }
    else
    {
	printf("!!!!!!Error!!!!!!\nFOR Encoding---->PASS 4-Arguments with <-e> OPTION\nFOR Decoding---->PASS 3-Arguments with <-d> OPTION\n");
    }
    return 0;
}
OperationType check_operation_type(char *argv[])	//OperationType is nothing but a ENUM   (Function definition )
{							//Which operation USER want to do (either ENCODE or DECODE)
    if(!strcmp(argv[1], "-e"))				//Using strcmp to compare argv[1] and -e
	return e_encode;				//if both are same strcmp will return 0 else any number(we have to return e_encode(ENUM))
    else if(!strcmp(argv[1], "-d"))
	return e_decode;
    else
	return e_unsupported;
}
