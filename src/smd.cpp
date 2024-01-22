#include "smd.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

void SMD::Read( const std::string &filename )
{
    if ( not std::filesystem::exists( filename ) )
    {
        std::cerr << "File " << filename << " does not exist!" << std::endl;
        exit( EXIT_FAILURE );
    }

    std::ifstream inputStream{ filename };

    /*
     *=============*
     * READ HEADER *
     *=============*
     */
    Header h;
    char   garbage[9];
    inputStream.read( h.identifier, 4 );
    std::cout << "Identifier: " << h.identifier[0] << h.identifier[1] << h.identifier[2] << h.identifier[3]
              << std::endl;

    if ( h.identifier[0] != 's' and h.identifier[1] != 'm' and h.identifier[2] != 'd' and h.identifier[3] != 's' )
    {
        std::cerr << "File " << filename << " does not contain an SMD identifier string!" << std::endl;
        exit( EXIT_FAILURE );
    }

    inputStream.read( garbage, 4 );
    inputStream.read( (char *)&h.file_size, 4 );
    std::cout << "File size: " << h.file_size << std::endl;

    inputStream.read( garbage, 8 );
    inputStream.read( (char *)&h.number_of_channels, 1 );
    std::cout << "Number of channels: " << (unsigned int)h.number_of_channels << std::endl;

    inputStream.read( garbage, 9 );
    inputStream.read( (char *)&h.offset_of_filename, 2 );
    std::cout << "Offset to filename: " << h.offset_of_filename << std::endl;

    inputStream.read( (char *)&h.offset_of_data_chunk, 2 );
    std::cout << "Offset to data chunk: " << h.offset_of_data_chunk << std::endl;

    for ( unsigned char i = 0; i < h.number_of_channels; i++ )
    {
        unsigned short offset;
        inputStream.read( (char *)&offset, 2 );
        std::cout << "Offset to channel " << (unsigned int)i << ": " << offset << std::endl;
        h.offset_of_channel_n.push_back( offset );
    }

    unsigned short endOfOffsets;
    inputStream.read( (char *)&endOfOffsets, 2 );
    if ( endOfOffsets != 0 )
    {
        std::cerr << "Error while parsing header of file " << filename << std::endl;
        exit( EXIT_FAILURE );
    }

    char *b = new char;
    do
    {
        inputStream.read( b, 1 );
        h.filename.push_back( *b );
    } while ( *b != '\0' );

    std::cout << "Filename: " << h.filename << std::endl;
    this->header = h;

    unsigned int headerSize = ( sizeof( char ) * 4 ) + sizeof( unsigned int ) + sizeof( unsigned char ) +
                              sizeof( unsigned short ) + sizeof( unsigned short ) +
                              ( sizeof( unsigned short ) * h.offset_of_channel_n.size() ) + h.filename.size() + 4 + 8 +
                              9 + 2;

    this->dataChunkSize = h.file_size - headerSize;

    std::printf( "Data chunk size: 0x%x\n", dataChunkSize );
    std::printf( "Header size: 0x%x\n", headerSize );

    /*
     *===========*
     * READ DATA *
     *===========*
     */
    this->data = std::make_unique<std::vector<unsigned char>>( dataChunkSize );
    inputStream.read( (char *)this->data->data(), dataChunkSize );

    /*
     *=============*
     * END OF FILE *
     *=============*
     */
    inputStream.close();
}

void SMD::PrintNote( unsigned char note )
{
    std::printf( "[0x%x]: Play note ", (unsigned int)note );
    auto b = NextByte();

    if ( b.has_value() )
    {
        std::printf( "0x%x", (unsigned int)b.value() );
        if ( b.value() % 0x13 == 0 )
        {
            b.reset();
            b = NextByte();
            std::printf( " 0x%x", (unsigned int)b.value() );
        }
        std::cout << std::endl;
    }
}

void SMD::PrintInstruction( unsigned char code, std::string message, unsigned int num_parameters )
{
    std::printf( "[0x%x]: %s", (unsigned int)code, message.c_str() );

    for ( unsigned int i = 0; i < num_parameters; i++ )
    {
        auto byte = NextByte();
        std::printf( "0x%x ", (unsigned int)byte.value() );
    }

    std::printf( "\n" );
}

std::optional<unsigned char> SMD::NextByte()
{
    if ( this->offset < this->dataChunkSize )
        return this->data->at( this->offset++ );
    return std::optional<unsigned char>();
}

template <int Start, int End> constexpr void SMD::GeneratePairs()
{
    if constexpr ( Start <= End )
    {
        this->convertionTable.insert( std::pair<unsigned char, ConvFunc>( Start, [&]() { PrintNote( Start ); } ) );
        GeneratePairs<Start + 1, End>();
    }

    return;
}

void SMD::ToMIDI( const std::string &output_filename )
{
    this->convertionTable = {
        std::pair<unsigned char, ConvFunc>( 0x80, [&]() { PrintInstruction( 0x80, "Rest for ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x81, [&]() { PrintInstruction( 0x81, "Extend previous note for ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x90, [&]() { PrintInstruction( 0x90, "End channel", 0 ); } ),
        std::pair<unsigned char, ConvFunc>(
            0x91, [&]() { PrintInstruction( 0x91, "Loop remainder of channel indefinitely", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x94, [&]() { PrintInstruction( 0x94, "Set octave to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x95, [&]() { PrintInstruction( 0x95, "Increment octave", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x96, [&]() { PrintInstruction( 0x96, "Decrement octave", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x97, [&]() { PrintInstruction( 0x97, "Set time signature to ", 2 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x98, [&]() { PrintInstruction( 0x98, "Begin loop, times=", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x99, [&]() { PrintInstruction( 0x99, "End loop", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x9C, [&]() { PrintInstruction( 0x9C, "? ", 3 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xA0, [&]() { PrintInstruction( 0xA0, "Set tempo to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xAC, [&]() { PrintInstruction( 0xAC, "Set instrument to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xBA, [&]() { PrintInstruction( 0xBA, "Begin channel", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xBF, [&]() { PrintInstruction( 0xBF, "? ", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC0, [&]() { PrintInstruction( 0xC0, "? ", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC2, [&]() { PrintInstruction( 0xC2, "Set attack rate to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC3, [&]() { PrintInstruction( 0xC3, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC4, [&]() { PrintInstruction( 0xC4, "Set sustain rate to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC5, [&]() { PrintInstruction( 0xC5, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC6, [&]() { PrintInstruction( 0xC6, "Set sustain rate to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC7, [&]() { PrintInstruction( 0xC7, "Set note volume? ", 2 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC8, [&]() { PrintInstruction( 0xC8, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC9, [&]() { PrintInstruction( 0xC9, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xD2, [&]() { PrintInstruction( 0xD2, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xD7, [&]() { PrintInstruction( 0xD7, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xD8, [&]() { PrintInstruction( 0xD8, "Pitch shift ", 3 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xDA, [&]() { PrintInstruction( 0xDA, "? ", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xDB, [&]() { PrintInstruction( 0xDB, "? ", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE0, [&]() { PrintInstruction( 0xE0, "Set volume to ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE3, [&]() { PrintInstruction( 0xE3, "? ", 1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE4, [&]() { PrintInstruction( 0xE4, "? ", 3 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE6, [&]() { PrintInstruction( 0xE6, "? ", 0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE8, [&]() { PrintInstruction( 0xE8, "Set balance ", 1 ); } ) };

    GeneratePairs<0x00, 0x7F>();


    /*
     *===============================*
     * READ AND EXECUTE INSTRUCTIONS *
     *===============================*
     *---------------------------------------------------------*
     * For now it only prints the instructions into the stdout *
     *---------------------------------------------------------*
     */
    std::optional<unsigned char> b;
    do
    {
        b.reset();
        b = NextByte();

        if ( b.has_value() )
        {
            this->convertionTable[b.value()]();
        }
    } while ( b.has_value() );
}
