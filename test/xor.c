
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define KEY 0x86
char* xorEncrypt(char Encrypt[]);
char* xorDecrypt(char Encrypt[]);
int main()
{
    char p_data[16] = {"12344555"};
    char Encrypt[16]={0};


    printf("Initial data:  %s\n",p_data);

    xorEncrypt(p_data);

	
    printf("Encrypt data:  %s\n",p_data);

    xorDecrypt(p_data);

    printf("decrypt data:  %s\n",p_data);

    return 0;
}

char * xorEncrypt(char Encrypt[]){

    for(int i = 0; i < strlen(Encrypt); i++)
    {
	Encrypt[i] = Encrypt[i] ^ KEY;
    }
    return Encrypt;
}


char* xorDecrypt(char decrypt[]){

    for(int i = 0; i < strlen(decrypt); i++)
    {
	decrypt[i] = decrypt[i] ^ KEY;
    }
    return decrypt;
}