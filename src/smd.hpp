#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class SMD
{
private:
    using ConvFunc = std::function<void( void )>;
    struct Header
    {
        char                        identifier[4];
        unsigned int                file_size;
        unsigned char               number_of_channels;
        unsigned short              offset_of_filename;
        unsigned short              offset_of_data_chunk;
        std::vector<unsigned short> offset_of_channel_n;
        std::string                 filename;
    };

    Header                                      header;
    std::unique_ptr<std::vector<unsigned char>> data;
    unsigned int                                offset        = 0;
    unsigned int                                dataChunkSize = 0;

    std::map<unsigned char, ConvFunc> convertionTable;

    void PrintNote( unsigned char note );
    void PrintInstruction( unsigned char code, std::string message, unsigned int num_parameters );
    std::optional<unsigned char> NextByte();
    void                         Read( const std::string &filename );

    template <int Start, int End> constexpr void GeneratePairs();

public:
    inline SMD( const std::string &filename )
    {
        this->Read( filename );
    }

    void ToMIDI( const std::string &output_file );
};
