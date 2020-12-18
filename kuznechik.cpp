#include "kuznechik.h"

void encrypt_file( const char* input_file_name, const char* output_file_name, const char* key_1, const char* key_2)
{
    kuznechik encryptor( input_file_name, block( key_1), block( key_2));
    encryptor.encrypt_data( output_file_name);
}

void decrypt_file( const char* input_file_name, const char* output_file_name, const char* key_1, const char* key_2)
{
    kuznechik encryptor( input_file_name, block( key_1), block( key_2));
    encryptor.decrypt_data( output_file_name);
}

void kuznechik::encrypt_data( const char* output_file_name)
{
    for ( int i = 0; i < data.size(); i++)
        data[i] = encrypt_block( data[i]);
    write_to_file( output_file_name);
}

void kuznechik::decrypt_data( const char* output_file_name)
{
    for ( int i = 0; i < data.size(); i++)
        data[i] = decrypt_block( data[i]);
    write_to_file( output_file_name);
}

kuznechik::kuznechik( const char* file_name, const block key_1, const block key_2)
{
    iteration_keys.resize( number_of_iteration_keys);
    read_file_to_data_buffer( file_name);
    calculate_iteration_constants();
    generate_iteraion_keys( key_1, key_2);
}

void kuznechik::read_file_to_data_buffer( const char* file_name)
{
    std::ifstream input_file_stream( file_name);
    assert( input_file_stream && "Can't find file");
    std::string file_content( ( std::istreambuf_iterator<char>( input_file_stream)), std::istreambuf_iterator<char>());

    int length_of_the_trailing_string = file_content.length() % block::size;

    for ( int i = 0; i + block::size <= file_content.length(); i += block::size)
        data.push_back( block( file_content.substr( i, block::size)));

    if (length_of_the_trailing_string != 0)
    {
        std::string trailing_content = file_content.substr( file_content.length() - length_of_the_trailing_string);
        for ( int i = 0; i < block::size - length_of_the_trailing_string; i++)
            trailing_content.push_back( ' ');
        data.push_back( block( trailing_content));
    }
}

void kuznechik::calculate_iteration_constants()
{
    //                        123456789012345     15 zeros
    std::string zero_string( "000000000000000");
    for( int i = 0; i < 32; i++)
    {
        std::string iterational_string;
        iterational_string.push_back( (char)i);
        iterational_string += zero_string;
        iteration_constants.push_back( L( block( iterational_string)));
    }
}

unsigned char kuznechik::get_mask_value( int index) const
{
    assert( index >= 0 && index < block::size && "Wrong index value");
    return mask[index];
}

unsigned char kuznechik::get_substituted_value( int index) const
{
    assert( index >= 0 && index <= UCHAR_MAX && "Wrong index value");
    return substitution_table[index];
}

unsigned char kuznechik::get_reversed_substituted_value( int index) const
{
    assert( index >= 0 && index <= UCHAR_MAX && "Wrong index value");
    return substitution_table_reversed[index];
}

block kuznechik::get_iteration_constant( int index) const
{
    assert( index >= 0 && index < 2 * block::size && "Wrong index value");
    return iteration_constants.at( index);
}

block kuznechik::get_iteration_key( int index) const
{
    assert( index >= 0 && index < number_of_iteration_keys && "Wrong index value");
    return iteration_keys[index];
}

void kuznechik::set_iteration_key( int index, const block value)
{
    assert( index >= 0 && index < number_of_iteration_keys && "Wrong index value");
    iteration_keys[index] = value;
}

block kuznechik::S( const block input_block)
{
	std::vector<unsigned char> transformed_data;
	for ( int i = 0 ; i < block::size; i++)
		transformed_data.push_back( get_substituted_value( input_block[i]));
	return block( transformed_data);
}

block kuznechik::S_reversed( const block input_block)
{
	std::vector<unsigned char> transformed_data;

	for ( int i = 0 ; i < block::size; i++)
		transformed_data.push_back( get_reversed_substituted_value( input_block[i]));
	return block( transformed_data);
}

unsigned char kuznechik::GF_mul( unsigned char a, unsigned char b)
{
	unsigned char c = 0;
	for ( int i = 0; i < 8; i++)
    {
		if ( ( b & 1) == 1)
			c ^= a;
		unsigned char hi_bit = (char)( a & 0x80);
		a <<= 1;
		if( hi_bit < 0)
			a ^= 0xC3;
		b >>= 1;
	}
	return c;
}

 block kuznechik::R( const block input_block)
 {
    std::vector<unsigned char> transformed_data( block::size);
	unsigned char trailing_symbol = 0;

	for ( int i = block::size - 1; i >= 0; i--)
    {
		if( i == 0)
			transformed_data[block::size] = input_block[i];
		else
			transformed_data[i-1] = input_block[i];
		trailing_symbol ^= GF_mul( input_block[i], get_mask_value(i));
	}
	transformed_data[block::size - 1] = trailing_symbol;
	return block( transformed_data);
}

block kuznechik::R_reversed( const block input_block)
{
    std::vector<unsigned char> transformed_data( block::size);
    unsigned char leading_symbol = input_block[block::size - 1];

    for ( int i = 1; i < block::size; i++)
    {
        transformed_data[i] = input_block[i - 1];
        leading_symbol ^= GF_mul( transformed_data[i], get_mask_value(i));
    }
    transformed_data[0] = leading_symbol;
    return block( transformed_data);
}

block kuznechik::L( const block input_block)
{
	block transformed_block = input_block;
    for ( int i = 0; i < block::size; i++)
        transformed_block = R( transformed_block);
    return transformed_block;
}


block kuznechik::L_reversed( const block input_block)
{
    block transformed_block = input_block;
    for ( int i = 0; i < block::size; i++)
        transformed_block = R_reversed( transformed_block);
    return transformed_block;
}

key_pair kuznechik::F( const key_pair input_key_pair, const block iteraion_constant)
{
    block returned_key_1;
    block returned_key_2 = input_key_pair.key_1;
    returned_key_1 = L( S( input_key_pair.key_2 ^ iteraion_constant)) ^ returned_key_2;
    return key_pair( returned_key_1, returned_key_2);
}

void kuznechik::generate_iteraion_keys( block key_1, block key_2)
{
    iteration_keys[0] = key_1;
    iteration_keys[1] = key_2;
    key_pair key_pair_1_2( key_1, key_2);
    key_pair key_pair_3_4;
    for( int i = 0; i < 4; i++)
    {
        key_pair_3_4 = F( key_pair_1_2, get_iteration_constant(0 + 8 * i));
        key_pair_1_2 = F( key_pair_3_4, get_iteration_constant(1 + 8 * i));
        key_pair_3_4 = F( key_pair_1_2, get_iteration_constant(2 + 8 * i));
        key_pair_1_2 = F( key_pair_3_4, get_iteration_constant(3 + 8 * i));
        key_pair_3_4 = F( key_pair_1_2, get_iteration_constant(4 + 8 * i));
        key_pair_1_2 = F( key_pair_3_4, get_iteration_constant(5 + 8 * i));
        key_pair_3_4 = F( key_pair_1_2, get_iteration_constant(6 + 8 * i));
        key_pair_1_2 = F( key_pair_3_4, get_iteration_constant(7 + 8 * i));
        set_iteration_key( 2 * i + 2, key_pair_1_2.key_1);
        set_iteration_key( 2 * i + 3, key_pair_1_2.key_2);
    }
}

block kuznechik::encrypt_block( const block input_block)
{
    block returned_block = input_block;
    for( int i = 0; i < 9; i++)
    {
        returned_block = get_iteration_key( i) ^ returned_block;
        returned_block = S( returned_block);
        returned_block = L( returned_block);
    }
    returned_block = returned_block ^ get_iteration_key( 9);
    return returned_block;
}

block kuznechik::decrypt_block( const block input_block)
{
    block returned_block = input_block ^ get_iteration_key( 9);
    for( int i = 8; i >= 0; i--)
    {
        returned_block = L_reversed( returned_block);
        returned_block = S_reversed( returned_block);
        returned_block = get_iteration_key(i) ^ returned_block;
    }
    return returned_block;
}

void kuznechik::write_to_file( const char* output_file)
{
    std::ofstream output_stream;
    output_stream.open( output_file);
    assert( output_stream.is_open() && "Can't open file");
    for ( block i : data)
        for ( int j = 0; j < block::size; j++)
            output_stream << (unsigned char)(i[j]);
}
/////////////////////////

block::block( std::vector<unsigned char> input_string) : data( input_string) { assert ( input_string.size() == size); };

block::block() { data.resize( size); }

block::block( std::string input_string)
{
    assert( input_string.length() == size && "Wrong length of the block");
    for ( int i = 0; i < size; i++)
        data.push_back( input_string[i]);
}

unsigned char block::operator[] ( const int index) const
{
    assert ( index < size && "given index causes overflow");
    return data[index];
}

block operator ^ ( const block& a, const block& b)
{
    block result;
    for( int i = 0; i < result.size; i++)
		result.data[i] = b[i]^a[i];
    return result;
}

std::ostream& operator << ( std::ostream& os, const block& b)
{
    return os  << b.data << std::endl;
}

void block::print()
{
    for( int i : data)
        std::cout << (unsigned char)i;
    std::cout << std::endl;
}

