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

const float PI        = 3.14159265358979323846f;
const float REF_FREQ  = 440.0f;
const float REF_INDEX = 69.0f;

// to be used for variable quality selection
typedef struct {
    const int depth;
} BitDepth;

typedef struct {
    const int hz;
} SampleRate;

BitDepth   BitDepths[3]   = { 16, 24, 32 };
SampleRate SampleRates[8] = { 44100, 48000, 88200, 96000, 176400, 192000, 352800, 384000 };

/*********
 * NOTES *
 *********/
// parameters of note object
typedef struct {
    uint8_t index;
    int     octave;
    float   frequency;
    int     sampleCount;
} NoteData;

// array of notes
NoteData Notes[128];

// when called, generates data for each note in the range
void     Populate_Notes( int sampleRate ) {
    for ( int i = 0; i < 128; ++i ) {
        float exp          = ( (float) i - REF_INDEX ) / 12.0f;
        Notes[i].index     = i;
        Notes[i].octave    = i / 12;
        Notes[i].frequency = REF_FREQ * powf( 2.0f, exp );
        Notes[i].sampleCount   = ( (float) SampleRates[sampleRate].hz / Notes[i].frequency ) + 1;
    }
}

/*************
 * WAVEFORMS *
 *************/
// 'generic' waveform pointer function
typedef double ( *WaveformFormula )( double x );

// parameters of a waveform
typedef struct {
    const char     *name;
    WaveformFormula formula;
} Waveform;

double   SineWave( double x ) { return sin( x ); }

double   SquareWave( double x ) { return ( fmod( x, 2 * PI ) < 2 * PI ) ? 1.0f : -1.0; }

double   TriangleWave( double x ) { return x; }

double   SawWave( double x ) { return x; }

// list of available waveforms
Waveform waveforms[] = {
  {"Sine",     SineWave    },
  {"Square",   SquareWave  },
  {"Triangle", TriangleWave},
  {"Sawtooth", SawWave     }
};

const int num_waveforms =
  sizeof( waveforms ) / sizeof( Waveform );    // QUESTION: what is this? why is it necessary?

/***********
 * SAMPLES *
 ***********/
// parameters of a waveform sample; struct represents a single
typedef struct {
    // QUESTION: pointers used for time and amplitude due to non uniform values? What would the
    // implication be of using arrays similar to NoteData, accepting that some notes would have
    // empty values
    float time;
    int   amplitude;
} Sample;

// function that creates a single sample
void Sample_Create (NoteData index, Sample time, Waveform formula ) {
	Sample.amplitude = 0; // f(amplitude) = time(formula)
}

// function that creates an array of samples by calling createSample while i < sampleCount
void Sample_Note () {}

// function that creates an array of sampled notes while i < 128
void Sample_Wave () {}

// function that creates an array of sample notes while i < num_waveforms
void Sample_DB () {}

// when called, generates data for each note in the range
void waveformSamples( void ) {
    for ( int i = 0; i < 4; ++i ) {
		wave = waveforms[i];

        float exp          = ( (float) i - REF_INDEX ) / 12.0f;
        Notes[i].index     = i;
        Notes[i].octave    = i / 12;
        Notes[i].frequency = REF_FREQ * powf( 2.0f, exp );
        Notes[i].samples   = ( (float) SampleRates[sampleRate].hz / Notes[i].frequency ) + 1;
    }
}

// when called, generates samples for a given note
void Populate_Sample( Sample *sample, NoteData *note, uint8_t note_index, SampleRate *hz ) {
    int   i                 = note_index;
    float frequency         = Notes[i].frequency;
    float angular_frequency = 2.0f * PI * frequency;
    float sample_period     = 1.0f / SampleRates->hz;
    int   num_samples       = Notes[i].samples;

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
void Free_Sample( Sample *sample ) {
    free( sample->time );
    free( sample->amplitude );
    sample->time      = NULL;
    sample->amplitude = NULL;
    sample->length    = 0;
}

/************
 * PLAYBACK *
 ************/
void Playback() {}

int  Playback_samples( Sample time, Sample length, int signal ) {
    if ( signal == 1 ) {
        return Sample.amplitude;
    } else return NULL;
}

int main() {
    struct NoteData seedNote;
    struct Sample   seedSample = { NULL, NULL, 0 };
    FILE           *csv_file;
    int             userNote;
    char            fileName[100];

    /*
     *	playback function: accepts a note and returns the amplitude value synced to the sample rate
     *		[] need to know what time it is
     *		[] need to know what the return interval is (while play = true, return amplitude
     *where a =
     *
     */

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
