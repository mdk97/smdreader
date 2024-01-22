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

void SMD::ToMIDI( const std::string &output_filename )
{
    /*
     *===============================*
     * BEHOLD THE GIANT WALL OF TEXT *
     *===============================*
     */
    this->convertionTable = {
        std::pair<unsigned char, ConvFunc>( 0x0, [&]() { PrintNote( 0x0 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1, [&]() { PrintNote( 0x1 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2, [&]() { PrintNote( 0x2 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3, [&]() { PrintNote( 0x3 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4, [&]() { PrintNote( 0x4 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5, [&]() { PrintNote( 0x5 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6, [&]() { PrintNote( 0x6 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7, [&]() { PrintNote( 0x7 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x8, [&]() { PrintNote( 0x8 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x9, [&]() { PrintNote( 0x9 ); } ),
        std::pair<unsigned char, ConvFunc>( 0xA, [&]() { PrintNote( 0xa ); } ),
        std::pair<unsigned char, ConvFunc>( 0xB, [&]() { PrintNote( 0xb ); } ),
        std::pair<unsigned char, ConvFunc>( 0xC, [&]() { PrintNote( 0xc ); } ),
        std::pair<unsigned char, ConvFunc>( 0xD, [&]() { PrintNote( 0xd ); } ),
        std::pair<unsigned char, ConvFunc>( 0xE, [&]() { PrintNote( 0xe ); } ),
        std::pair<unsigned char, ConvFunc>( 0xF, [&]() { PrintNote( 0xf ); } ),
        std::pair<unsigned char, ConvFunc>( 0x10, [&]() { PrintNote( 0x10 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x11, [&]() { PrintNote( 0x11 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x12, [&]() { PrintNote( 0x12 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x13, [&]() { PrintNote( 0x13 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x14, [&]() { PrintNote( 0x14 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x15, [&]() { PrintNote( 0x15 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x16, [&]() { PrintNote( 0x16 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x17, [&]() { PrintNote( 0x17 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x18, [&]() { PrintNote( 0x18 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x19, [&]() { PrintNote( 0x19 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1A, [&]() { PrintNote( 0x1a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1B, [&]() { PrintNote( 0x1b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1C, [&]() { PrintNote( 0x1c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1D, [&]() { PrintNote( 0x1d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1E, [&]() { PrintNote( 0x1e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x1F, [&]() { PrintNote( 0x1f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x20, [&]() { PrintNote( 0x20 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x21, [&]() { PrintNote( 0x21 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x22, [&]() { PrintNote( 0x22 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x23, [&]() { PrintNote( 0x23 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x24, [&]() { PrintNote( 0x24 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x25, [&]() { PrintNote( 0x25 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x26, [&]() { PrintNote( 0x26 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x27, [&]() { PrintNote( 0x27 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x28, [&]() { PrintNote( 0x28 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x29, [&]() { PrintNote( 0x29 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2A, [&]() { PrintNote( 0x2a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2B, [&]() { PrintNote( 0x2b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2C, [&]() { PrintNote( 0x2c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2D, [&]() { PrintNote( 0x2d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2E, [&]() { PrintNote( 0x2e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x2F, [&]() { PrintNote( 0x2f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x30, [&]() { PrintNote( 0x30 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x31, [&]() { PrintNote( 0x31 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x32, [&]() { PrintNote( 0x32 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x33, [&]() { PrintNote( 0x33 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x34, [&]() { PrintNote( 0x34 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x35, [&]() { PrintNote( 0x35 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x36, [&]() { PrintNote( 0x36 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x37, [&]() { PrintNote( 0x37 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x38, [&]() { PrintNote( 0x38 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x39, [&]() { PrintNote( 0x39 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3A, [&]() { PrintNote( 0x3a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3B, [&]() { PrintNote( 0x3b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3C, [&]() { PrintNote( 0x3c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3D, [&]() { PrintNote( 0x3d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3E, [&]() { PrintNote( 0x3e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x3F, [&]() { PrintNote( 0x3f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x40, [&]() { PrintNote( 0x40 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x41, [&]() { PrintNote( 0x41 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x42, [&]() { PrintNote( 0x42 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x43, [&]() { PrintNote( 0x43 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x44, [&]() { PrintNote( 0x44 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x45, [&]() { PrintNote( 0x45 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x46, [&]() { PrintNote( 0x46 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x47, [&]() { PrintNote( 0x47 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x48, [&]() { PrintNote( 0x48 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x49, [&]() { PrintNote( 0x49 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4A, [&]() { PrintNote( 0x4a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4B, [&]() { PrintNote( 0x4b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4C, [&]() { PrintNote( 0x4c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4D, [&]() { PrintNote( 0x4d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4E, [&]() { PrintNote( 0x4e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x4F, [&]() { PrintNote( 0x4f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x50, [&]() { PrintNote( 0x50 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x51, [&]() { PrintNote( 0x51 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x52, [&]() { PrintNote( 0x52 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x53, [&]() { PrintNote( 0x53 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x54, [&]() { PrintNote( 0x54 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x55, [&]() { PrintNote( 0x55 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x56, [&]() { PrintNote( 0x56 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x57, [&]() { PrintNote( 0x57 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x58, [&]() { PrintNote( 0x58 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x59, [&]() { PrintNote( 0x59 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5A, [&]() { PrintNote( 0x5a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5B, [&]() { PrintNote( 0x5b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5C, [&]() { PrintNote( 0x5c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5D, [&]() { PrintNote( 0x5d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5E, [&]() { PrintNote( 0x5e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x5F, [&]() { PrintNote( 0x5f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x60, [&]() { PrintNote( 0x60 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x61, [&]() { PrintNote( 0x61 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x62, [&]() { PrintNote( 0x62 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x63, [&]() { PrintNote( 0x63 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x64, [&]() { PrintNote( 0x64 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x65, [&]() { PrintNote( 0x65 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x66, [&]() { PrintNote( 0x66 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x67, [&]() { PrintNote( 0x67 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x68, [&]() { PrintNote( 0x68 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x69, [&]() { PrintNote( 0x69 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6A, [&]() { PrintNote( 0x6a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6B, [&]() { PrintNote( 0x6b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6C, [&]() { PrintNote( 0x6c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6D, [&]() { PrintNote( 0x6d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6E, [&]() { PrintNote( 0x6e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x6F, [&]() { PrintNote( 0x6f ); } ),
        std::pair<unsigned char, ConvFunc>( 0x70, [&]() { PrintNote( 0x70 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x71, [&]() { PrintNote( 0x71 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x72, [&]() { PrintNote( 0x72 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x73, [&]() { PrintNote( 0x73 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x74, [&]() { PrintNote( 0x74 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x75, [&]() { PrintNote( 0x75 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x76, [&]() { PrintNote( 0x76 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x77, [&]() { PrintNote( 0x77 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x78, [&]() { PrintNote( 0x78 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x79, [&]() { PrintNote( 0x79 ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7A, [&]() { PrintNote( 0x7a ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7B, [&]() { PrintNote( 0x7b ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7C, [&]() { PrintNote( 0x7c ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7D, [&]() { PrintNote( 0x7d ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7E, [&]() { PrintNote( 0x7e ); } ),
        std::pair<unsigned char, ConvFunc>( 0x7F, [&]() { PrintNote( 0x7f ); } ),

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