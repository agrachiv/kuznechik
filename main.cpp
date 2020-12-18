#include "kuznechik.h"

int main()
{
	char key_1[] = "aaadefgpqrstuvws"; //just random 16-byte key
	char key_2[] = "bBbbbbebbeaaaaas"; //just random 16-byte key

    encrypt_file( "beatles.txt"                 , "output/encrypted_beatles.txt", key_1, key_2);    //encryption
    decrypt_file( "output/encrypted_beatles.txt", "output/decrypted_beatles.txt", key_1, key_2);    //decryption

	return 0;
}
