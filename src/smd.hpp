#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class SMD
{
private:
    using ConvFunc   = std::function<void( void )>;
    using Instrument = std::pair<unsigned char, std::string>;
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

    inline static const std::map<unsigned char, std::string> instruments = {
        Instrument( 0x00, "Heavy Square Wave (F-4)" ),
        Instrument( 0x01, "Empty" ),
        Instrument( 0x02, "Telephone (A#-3)" ),
        Instrument( 0x03, "Fat Saw Wave-Asp (F-4)" ),
        Instrument( 0x04, "Heavy Square Wave (F-4)" ),
        Instrument( 0x05, "Fat Saw Wave-Asp (F-5)" ),
        Instrument( 0x06, "Heavy Square Wave (F-5)" ),
        Instrument( 0x07, "Tuba (C-3)" ),
        Instrument( 0x08, "Glockenspiel (A-3)" ),
        Instrument( 0x09, "Bright Beep (C-3)" ),
        Instrument( 0x0A, "Seashore (C-2)" ),
        Instrument( 0x0B, "Seashore (C-3)" ),
        Instrument( 0x0C, "Seashore (C-4)" ),
        Instrument( 0x0D, "Drum Hit + Seashore (C-3)" ),
        Instrument( 0x0E, "Telephone 2 (C-5)" ),
        Instrument( 0x0F, "Explosion (C-2)" ),
        Instrument( 0x10, "Cymbal (A-3)" ),
        Instrument( 0x11, "Telephone 2 (C-7)" ),
        Instrument( 0x12, "Tubular Bells (C-5)" ),
        Instrument( 0x13, "Slap Bass 2 (C-1)" ),
        Instrument( 0x14, "Slap Bass 2 (C-2)" ),
        Instrument( 0x15, "Slap Bass 2 (C-3)" ),
        Instrument( 0x16, "Slap Bass 2 (C-4)" ),
        Instrument( 0x17, "Slap Bass 2 (C-5)" ),
        Instrument( 0x18, "Trumpet (C-3)" ),
        Instrument( 0x19, "Trumpet (C-4)" ),
        Instrument( 0x1A, "Pizzicato Strings (C-3)" ),
        Instrument( 0x1B, "Vibraslap/Spinner (A#-3)" ),
        Instrument( 0x1C, "Bassoon (C-3)" ),
        Instrument( 0x1D, "Bassoon (C-4)" ),
        Instrument( 0x1E, "Bassoon (C-5)" ),
        Instrument( 0x1F, "Machine Gun (C-5)" ),
        Instrument( 0x20, "Synth Strings (C-2)" ),
        Instrument( 0x21, "Synth Strings (C-4)" ),
        Instrument( 0x22, "Synth Strings (C-4)" ),
        Instrument( 0x23, "Synth Strings (C-5)" ),
        Instrument( 0x24, "Synth Strings (C-6)" ),
        Instrument( 0x25, "Synth Strings (C-7)" ),
        Instrument( 0x26, "Fretless Bass (C-2)" ),
        Instrument( 0x27, "Fretless Bass (C-3)" ),
        Instrument( 0x28, "Fretless Bass (C-4)" ),
        Instrument( 0x29, "Fretless Bass (C-5)" ),
        Instrument( 0x2A, "Fretless Bass (C-6)" ),
        Instrument( 0x2B, "Tuba (C-1)" ),
        Instrument( 0x2C, "Tuba (C-2)" ),
        Instrument( 0x2D, "Empty" ),
        Instrument( 0x2E, "Bassoon (C-4)" ),
        Instrument( 0x2F, "Bassoon (C-5)" ),
        Instrument( 0x30, "Trombone (C-2)" ),
        Instrument( 0x31, "Trombone (C-3)" ),
        Instrument( 0x32, "Trombone (C-4)" ),
        Instrument( 0x33, "Synth Brass 1" ),
        Instrument( 0x34, "Synth Brass 1" ),
        Instrument( 0x35, "Synth Brass 1" ),
        Instrument( 0x36, "Empty" ),
        Instrument( 0x37, "Empty" ),
        Instrument( 0x38, "Empty" ),
        Instrument( 0x39, "Cymbal Hit (A-3)" ),
        Instrument( 0x3A, "Cymbal Hit (B-3)" ),
        Instrument( 0x3B, "Vibraphone (C-3)" ),
        Instrument( 0x3C, "French Horns (C-2)" ),
        Instrument( 0x3D, "French Horns (C-3)" ),
        Instrument( 0x3E, "French Horns (C-4)" ),
        Instrument( 0x3F, "French Horns (C-5)" ),
        Instrument( 0x40, "Timpani (C-3)" ),
        Instrument( 0x41, "Pizzicato (C-4)" ),
        Instrument( 0x42, "Timpani (C-2)" ),
        Instrument( 0x43, "Tubular Bells (C-5)" ),
        Instrument( 0x44, "Clarinet (C-3)" ),
        Instrument( 0x45, "Empty" ),
        Instrument( 0x46, "Clarinet (C-4)" ),
        Instrument( 0x47, "Clarinet (C-5)" ),
        Instrument( 0x48, "Brush (A-4)" ),
        Instrument( 0x49, "Cymbal (A-3)" ),
        Instrument( 0x4A, "French Horns (C-3)" ),
        Instrument( 0x4B, "French Horns (C-4)" ),
        Instrument( 0x4C, "French Horns (C-5)" ),
        Instrument( 0x4D, "French Horns (C-6)" ),
        Instrument( 0x4E, "French Horns (C-7)" ),
        Instrument( 0x4F, "Trombone (C-1)" ),
        Instrument( 0x50, "Trombone (C-2)" ),
        Instrument( 0x51, "Trombone (C-3)" ),
        Instrument( 0x52, "Trombone (C-4)" ),
        Instrument( 0x53, "Trombone (C-5)" ),
        Instrument( 0x54, "Synth Strings (C-2)" ),
        Instrument( 0x55, "Synth Strings (C-3)" ),
        Instrument( 0x56, "Synth Strings (C-4)" ),
        Instrument( 0x57, "Synth Strings (C-5)" ),
        Instrument( 0x58, "Synth Strings (C-6)" ),
        Instrument( 0x59, "Synth Strings (C-7)" ),
        Instrument( 0x5A, "Synth Strings 1 (C-2)" ),
        Instrument( 0x5B, "Synth Strings 1 (C-3)" ),
        Instrument( 0x5C, "Synth Strings 1 (C-4)" ),
        Instrument( 0x5D, "Synth Strings 1 (C-5)" ),
        Instrument( 0x5E, "Synth Strings 1 (C-6)" ),
        Instrument( 0x5F, "Synth Strings 2 (C-2)" ),
        Instrument( 0x60, "Synth Strings 2 (C-3)" ),
        Instrument( 0x61, "Synth Strings 2 (C-4)" ),
        Instrument( 0x62, "Synth Strings 2 (C-5)" ),
        Instrument( 0x63, "Snare Hit (E-2)" ),
        Instrument( 0x64, "Synth Strings 1 (C-5)" ),
        Instrument( 0x65, "Tubular Bells (C-6)" ),
        Instrument( 0x66, "Bass Drum Hit (F-2)" ),
        Instrument( 0x67, "Empty" ),
        Instrument( 0x68, "Empty" ),
        Instrument( 0x69, "High-Frequency Noise" ),
        Instrument( 0x6A, "High-Pitched Strings" ),
        Instrument( 0x6B, "Sub-Bass Horn" ),
        Instrument( 0x6C, "Bass Horn" ),
        Instrument( 0x6D, "Horn" ),
        Instrument( 0x6E, "Treble Horn" ),
        Instrument( 0x6F, "Above-Treble Horn" ),
        Instrument( 0x70, "Oscillating Static" ),
        Instrument( 0x71, "Percussive Hammer" ),
        Instrument( 0x72, "Sustained Oscillating Bass Noise" ),
        Instrument( 0x73, "Low Percussive Hammer" ),
        Instrument( 0x74, "High-Frequency Oscillating Voice" ),
        Instrument( 0x75, "Metal Percussive Strike" ),
        Instrument( 0x76, "Wide Signal" ),
        Instrument( 0x77, "Pitched-Up Wide Signal" ),
        Instrument( 0x78, "Sub-Bass Horn" ),
        Instrument( 0x79, "Bass Horn" ),
        Instrument( 0x7A, "Horn" ),
        Instrument( 0x7B, "Treble Horn" ),
        Instrument( 0x7C, "Above-Treble Horn" ),
        Instrument( 0x7D, "Bass Tone" ),
        Instrument( 0x7E, "Pitched-Up Bass Tone" ),
        Instrument( 0x7F, "Wide Sub-Bass Tone" ),
        Instrument( 0x80, "Wide Bass Tone" ),
        Instrument( 0x81, "Wide Tone" ),
        Instrument( 0x82, "Ambient High-Frequency Voice" ),
        Instrument( 0x83, "High-Pitched Bell" ),
        Instrument( 0x84, "Ambient Voice" ),
        Instrument( 0x85, "Error Tone" ),
        Instrument( 0x86, "High-Frequency Tone" ),
        Instrument( 0x87, "Loud High-Frequency Tone" ),
        Instrument( 0x88, "Ambient High-Frequency Oscillating Tone" ),
        Instrument( 0x89, "Pitched-Up Ambient High-Frequency Oscillating Tone" ),
        Instrument( 0x8A, "Drum Roll" ),
        Instrument( 0x8B, "Bright Ambient Voice" ),
        Instrument( 0x8C, "Cymbal Crashing" ),
        Instrument( 0x8D, "Tremolo Noise" ),
        Instrument( 0x8E, "Deep Ambient Voice" ),
        Instrument( 0x8F, "Sub-Bass String" ),
        Instrument( 0x90, "Bass String" ),
        Instrument( 0x91, "String" ),
        Instrument( 0x92, "Treble String" ),
        Instrument( 0x93, "Above-Treble String" ),
        Instrument( 0x94, "High-Pitched String" ),
        Instrument( 0x95, "Wide Sub-Bass String" ),
        Instrument( 0x96, "Wide Bass String" ),
        Instrument( 0x97, "Wide String" ),
        Instrument( 0x98, "Wide Treble String" ),
        Instrument( 0x99, "Wide Above-Treble String" ),
        Instrument( 0x9A, "Wide High-Pitched String" ),
        Instrument( 0x9B, "Bass Vibrato String" ),
        Instrument( 0x9C, "Vibrato String" ),
        Instrument( 0x9D, "Treble Vibrato String" ),
        Instrument( 0x9E, "Above-Treble Vibrato String" ),
        Instrument( 0x9F, "High-Pitched Vibrato String" ),
        Instrument( 0xA0, "Bass String (Duplicate of 161)" ),
        Instrument( 0xA1, "Bass String" ),
        Instrument( 0xA2, "Treble String" ),
        Instrument( 0xA3, "Above-Treble String" ),
        Instrument( 0xA4, "Bass String" ),
        Instrument( 0xA5, "String" ),
        Instrument( 0xA6, "Above-Treble String" ),
        Instrument( 0xA7, "Treble Bell (Envelope Close)" ),
        Instrument( 0xA8, "Above-Treble Voice" ),
        Instrument( 0xA9, "Bell (Envelope Close)" ),
        Instrument( 0xAA, "Staccato Bell" ),
        Instrument( 0xAB, "Bell (Envelope Close)" ),
        Instrument( 0xAC, "Bass Muted Horn" ),
        Instrument( 0xAD, "Muted Horn" ),
        Instrument( 0xAE, "Treble Muted Horn" ),
        Instrument( 0xAF, "Above-Treble Muted Horn" ),
        Instrument( 0xB0, "Static Clip  Hit/slap sound" ),
        Instrument( 0xB1, "Clipped Thump" ),
        Instrument( 0xB2, "Inaudible Clip" ),
        Instrument( 0xB3, "Nearly Inaudible Clip" ),
        Instrument( 0xB4, "Blip" ),
        Instrument( 0xB5, "Empty" ),
        Instrument( 0xB6, "Empty" ),
        Instrument( 0xB7, "Empty" ),
        Instrument( 0xB8, "Clip" ),
        Instrument( 0xB9, "Empty" ),
        Instrument( 0xBA, "Empty" ),
        Instrument( 0xBB, "Empty" ),
        Instrument( 0xBC, "Empty" ),
        Instrument( 0xBD, "Blip" ),
        Instrument( 0xBE, "Nearly Inaudible Clip" ),
        Instrument( 0xBF, "Empty" ),
        Instrument( 0xC0, "Empty" ),
        Instrument( 0xC1, "Empty" ),
        Instrument( 0xC2, "Empty" ),
        Instrument( 0xC3, "Nearly Inaudible Clip" ),
        Instrument( 0xC4, "Blip" ),
        Instrument( 0xC5, "Empty" ),
        Instrument( 0xC6, "Empty" ),
        Instrument( 0xC7, "Empty" ),
        Instrument( 0xC8, "Empty" ),
        Instrument( 0xC9, "Empty" ),
        Instrument( 0xCA, "Empty" ),
        Instrument( 0xCB, "Empty" ),
        Instrument( 0xCC, "Empty" ),
        Instrument( 0xCD, "Empty" ),
        Instrument( 0xCE, "Empty" ),
        Instrument( 0xCF, "Empty" ),
        Instrument( 0xD0, "Empty" ),
        Instrument( 0xD1, "Empty" ),
        Instrument( 0xD2, "Empty" ),
        Instrument( 0xD3, "Blip" ),
        Instrument( 0xD4, "Nearly Inaudible Clip" ),
        Instrument( 0xD5, "Empty" ),
        Instrument( 0xD6, "Pop" ),
        Instrument( 0xD7, "Empty" ),
        Instrument( 0xD8, "Pop" ),
        Instrument( 0xD9, "Pop" ),
        Instrument( 0xDA, "Pop" ),
        Instrument( 0xDB, "Empty" ),
        Instrument( 0xDC, "Empty" ),
        Instrument( 0xDD, "Empty" ),
        Instrument( 0xDE, "Empty" ),
        Instrument( 0xDF, "Empty" ),
        Instrument( 0xE0, "Empty" ),
        Instrument( 0xE1, "Empty" ),
        Instrument( 0xE2, "Empty" ),
        Instrument( 0xE3, "Empty" ),
        Instrument( 0xE4, "Empty" ),
        Instrument( 0xE5, "Empty" ),
        Instrument( 0xE6, "Empty" ),
        Instrument( 0xE7, "Empty" ),
        Instrument( 0xE8, "Empty" ),
        Instrument( 0xE9, "Empty" ),
        Instrument( 0xEA, "Pop" ),
        Instrument( 0xEB, "Empty" ),
        Instrument( 0xEC, "Inaudible Clip" ),
        Instrument( 0xED, "Bell" ),
        Instrument( 0xEE, "Empty" ),
        Instrument( 0xEF, "Pop" ),
        Instrument( 0xF0, "Static + Bass String" ),
        Instrument( 0xF1, "Empty" ),
        Instrument( 0xF2, "Empty" ),
        Instrument( 0xF3, "Bell (Envelope Close)" ),
        Instrument( 0xF4, "Nearly Inaudible Clip" ),
        Instrument( 0xF5, "Empty" ),
        Instrument( 0xF6, "Pop" ),
        Instrument( 0xF7, "Nearly Inaudible Clip" ),
        Instrument( 0xF8, "Nearly Inaudible Clip" ),
        Instrument( 0xF9, "Empty" ),
        Instrument( 0xFA, "Empty" ),
        Instrument( 0xFB, "Empty" ),
        Instrument( 0xFC, "Empty" ),
        Instrument( 0xFD, "Empty" ),
        Instrument( 0xFE, "Empty" ),
        Instrument( 0xFF, "Empty" ) };

    Header                                      header;
    std::unique_ptr<std::vector<unsigned char>> data;
    unsigned int                                offset        = 0;
    unsigned int                                dataChunkSize = 0;

    std::map<unsigned char, ConvFunc> convertionTable;

    void PrintNote( unsigned char note );
    void PrintInstruction(
        unsigned char                                       code,
        std::string                                         message,
        unsigned int                                        num_parameters,
        std::function<void( std::optional<unsigned char> )> additionalFunctionality =
            []( std::optional<unsigned char> b ) {} );
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
