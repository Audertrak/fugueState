// testing encoding of a sine function for memory and cpu usage

/* TODO:
 *	- implement different waveforms
 *		[] square
 *		[] triangle
 *		[] saw
 *	- implement encoding strategies
 *		[] simple encode, uniform sample count regardless of wavelength (could be used with
 *				any other encode strategy)
 *		[] fully 'in memory', zero-latency target
 *		[] 'spatial aware' encoding, dynamically encode wave based on bit depth/sample
 *				rate/wavelength
 *		[] 'partial encoding', use wave symmetry to encode only a partial section of wave
 *				and use transformations
 *	- implement midi parameters
 *		[] velocity
 *		etc...
 *	- implement 'streaming'
 *		[] time synced packets
 *	- variable parameters
 *		[] pass bit depth
 *		[] pass sample rate
 *	- implement 'waveshaping'
 *		[] lfo(s)
 *		[] ADSR envelope
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const float    PI            = 3.14159265358979323846f;
const float    REF_FREQ      = 440.0f;
const float    REF_INDEX     = 69.0f;

// to be used for variable quality selection
const int      BitDepth[3]   = { 16, 24, 32 };
const int      SampleRate[8] = { 44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000 };

/*************
 * WAVEFORMS *
 *************/
// 'generic' waveform pointer function
typedef double ( *WaveformFormula )( double x );

// parameters of a waveform
typedef struct Waveform {
    const char     *name;
    WaveformFormula formula;
} Waveform;

double   SineWave( double x ) { return sin( x ); }

double   SquareWave( double x ) { return ( fmod( x, 2 * PI ) < 2 * PI ) ? 1.0f : -1.0; }

double   TriangleWave( double x ) { return x; }

double   SawWave( double x ) { return x; }

// list of available waveforms
Waveform waveforms[] = {
  {"Sine",     SineWave  },
  {"Square",   SquareWave},
  {"Sawtooth", SawWave   }
};

const int num_waveforms =
  sizeof( waveforms ) / sizeof( Waveform );    // QUESTION: what is this? why is it necessary?

/********************
 * END OF WAVEFORMS *
 ********************/

// parameters of note object
typedef struct NoteData {
    uint8_t index[128];
    int     octave[128];
    float   frequency[128];
    int     samples[128];
} NoteData;

// parameters of a waveform sample; struct represents a single
typedef struct Sample {
    // QUESTION: pointers used for time and amplitude due to non uniform values? What would the
    // implication be of using arrays similar to NoteData, accepting that some notes would have
    // empty values
    float *time;
    int   *amplitude;
    int    length;
} Sample;

// when called, generates data for each note in the range
void Populate_Notes( struct NoteData *note ) {
    for ( int i = 0; i < 128; ++i ) {
        float exp          = ( (float) i - REF_INDEX ) / 12.0f;
        note->index[i]     = i;
        note->octave[i]    = i / 12;
        note->frequency[i] = REF_FREQ * powf( 2.0f, exp );
        note->samples[i]   = ( SampleRate[0] / note->frequency[i] ) + 1;
    }
}

// when called, generates samples for a given note
void Populate_Sample( struct Sample *sample, const struct NoteData *note, uint8_t note_index ) {
    float frequency         = note->frequency[note_index];
    float angular_frequency = 2.0f * PI * frequency;
    float sample_period     = 1.0f / SampleRate[0];
    int   num_samples       = note->samples[note_index];

    // QUESTION: is this explicit memory management required due to the dynamic data/use of pointers
    // in the sample struct?
    sample->time            = (float *) malloc( num_samples * sizeof( float ) );
    sample->amplitude       = (int *) malloc( num_samples * sizeof( int ) );
    sample->length          = num_samples;

    if ( sample->time == NULL || sample->amplitude == NULL ) { return; }

    for ( int i = 0; i < num_samples; ++i ) {
        float time           = i * sample_period;
        sample->time[i]      = time;
        sample->amplitude[i] = (int) ( 32767 * sinf( angular_frequency * time ) );
    }
}

// clean up on exit
void Free_Sample( struct Sample *sample ) {
    free( sample->time );
    free( sample->amplitude );
    sample->time      = NULL;
    sample->amplitude = NULL;
    sample->length    = 0;
}

int main() {
    struct NoteData seedNote;
    struct Sample   seedSample = { NULL, NULL, 0 };
    FILE           *csv_file;
    int             userNote;
    char            fileName[100];

    return 0;
}

/*************************************************************************************************
* UNUSED BOILERPLATE *****************************************************************************
**************************************************************************************************

// print values
for ( int i = 0; i < 128; ++i ) {
printf( "Note index: %d\n", seedNote.index[i] );
printf( "Note octave: %d\n", seedNote.octave[i] );
printf( "Note frequency: %f\n", seedNote.frequency[i] );
printf( "Note minimum samples: %d\n", seedNote.samples[i] );
    if ( i == 69 ) {
        printf( "seedSample.time: %f\n", seedSample.time[i] );
        printf( "seedSample.amplitude: %d\n", seedSample.amplitude[i] );
    }
}

// save to file
    printf( "Enter note index [0,127]: " );
    scanf( "%d", &userNote );

    Populate_Notes( &seedNote );

    Populate_Sample( &seedSample, &seedNote, seedNote.index[userNote] );

    for ( int i = 0; i < seedSample.length; ++i ) {
        printf( "seedSample.time: %f\n", seedSample.time[i] );
        printf( "seedSample.amplitude: %d\n", seedSample.amplitude[i] );
    }

    snprintf( fileName, sizeof( fileName ), "note_%d_wave.csv", userNote );

    csv_file = fopen( fileName, "w" );
    if ( csv_file == NULL ) {
        printf( "error opening file\n" );
        return 1;
    }

    fprintf( csv_file, "time,amplitude\n" );

    for ( int i = 0; i < seedSample.length; i++ ) {
        fprintf( csv_file, "%f,%d\n", seedSample.time[i], seedSample.amplitude[i] );
    }

    fclose( csv_file );
    Free_Sample( &seedSample );



*/
