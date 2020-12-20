#include "kuznechik.cpp"

int main()
{
	char key_1[] = "aaadefgpqrstuvws"; //just random 16-byte key
	char key_2[] = "bBbbbbebbeaaaaas"; //just random 16-byte key

    //encrypt_file( "beatles.txt"                 , "output/encrypted_beatles.txt", key_1, key_2);
    //decrypt_file( "output/encrypted_beatles.txt", "output/decrypted_beatles.txt", key_1, key_2);

    //GOST testing

    char hex_key[] = "8899aabbccddeeff0011223344556677fedcba98765432100123456789abcdef";

    char block_1[] = "1122334455667700ffeeddccbbaa9988";
    char block_2[] = "00112233445566778899aabbcceeff0a";
    char block_3[] = "112233445566778899aabbcceeff0a00";
    char block_4[] = "2233445566778899aabbcceeff0a0011";

    char test_block[] = "41414141414141414141414141414141";
    //std::cout << "-" << encrypt_single_block( std::string( block_1), hex_key).length();
    std::cout << decrypt_single_block( encrypt_single_block( std::string( test_block), hex_key), hex_key);
    //decrypt_file( "output/encrypted_beatles.txt", "output/decrypted_beatles.txt", key_1, key_2);

	return 0;
}
