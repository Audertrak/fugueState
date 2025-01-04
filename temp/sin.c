// testing encoding of a sine function for memory and cpu usage
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const float PI           = 3.14159265358979323846f;
const float REF_FREQ     = 440.0f;
const float REF_INDEX    = 69.0f;

const int   BitDepth[]   = { 16, 24, 32 };
const int   SampleRate[] = { 44100, 96000, 192000, 384000 };

struct NoteData {
    uint8_t index[128];
    int     octave[128];
    float   frequency[128];
    int     samples[128];
};

struct Sample {
    float *time;
    int   *amplitude;
    int    length;
};

void Populate_Notes( struct NoteData *note ) {
    for ( int i = 0; i < 128; ++i ) {
        float exp          = ( (float) i - REF_INDEX ) / 12.0f;
        note->index[i]     = i;
        note->octave[i]    = i / 12;
        note->frequency[i] = REF_FREQ * powf( 2.0f, exp );
        note->samples[i]   = ( SampleRate[0] / note->frequency[i] ) + 1;
    }
}

void Populate_Sample( struct Sample *sample, const struct NoteData *note, uint8_t note_index ) {
    float frequency         = note->frequency[note_index];
    float angular_frequency = 2.0f * PI * frequency;
    float sample_period     = 1.0f / SampleRate[0];
    int   num_samples       = note->samples[note_index];

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

    Populate_Notes( &seedNote );

    Populate_Sample( &seedSample, &seedNote, seedNote.index[69] );

	csv_file = fopen("note_a4_wave.csv", "w");
	if (csv_file == NULL) {
		printf("error opening file\n");
			return 1;
	}

	fprintf(csv_file, "time,amplitude\n");

	for (int i = 0; i < seedSample.length; i++) {
		fprintf(csv_file, "%f,%d\n", seedSample.time[i], seedSample.amplitude[i]);
	}

	fclose(csv_file);
	Free_Sample(&seedSample);

    /* 
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
    */

    return 0;
}
