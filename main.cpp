#include "kuznechik.h"

int main()
{
	char key_1[] = "aaadefgpqrstuvws"; //just random 16-byte key
	char key_2[] = "bBbbbbebbeaaaaas"; //just random 16-byte key

	char key_hex[] = "8899aabbccddeeff0011223344556677fedcba98765432100123456789abcdef"; //hex key

    encrypt_file( "beatles.txt"                 , "output/encrypted_beatles.txt", key_hex);    //encryption
    decrypt_file( "output/encrypted_beatles.txt", "output/decrypted_beatles.txt", key_hex);    //decryption

	return 0;
}
