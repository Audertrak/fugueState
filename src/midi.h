// macro to force values to be within a valid range
#define CLAMP( value, minVal, maxVal ) \
    ( ( value ) < ( minVal ) ? ( minVal ) : ( ( value ) > ( maxVal ) ? ( maxVal ) : ( value ) #define MAX_VOICES 64

#define MAX_VOICES 64

/** struct that contains note data   **/
typedef struct {
    NoteName name;          // 0-107
    int      octave;        // 0-8
    float    frequency;     // TODO: implement frequency calculation
    int      volume;        // 0-127
    int      velocity;      // 0-127
    int      pitch;         // 0-127
    int      modulation;    // 0-127
} NoteData;

// convert from 'musical' note to note index
static int note_to_index( const NoteData *nd ) {
    int index = nd->octave * 12 + (int) nd->name;
    return CLAMP( index, 0, 107 );
}

/**
 * @brief [TODO:description]
 *
 * @param index [TODO:parameter]
 * @return [TODO:return]
 */
static NoteData index_to_note( int index ) {
    index = CLAMP( index, 0, 107 );
    NoteData nd;
    nd.octave = index / 12;
    nd.name   = (NoteName) ( index % 12 );
    return nd;
}

// midi struct
typedef struct {
    NoteData noteData;      // note name + octave
    int      volume;        // 0-127
    int      velocity;      // 0-127
    int      modulation;    // 0-127
    float    pitch;         // 0-127
} MidiNote;

// create a midi note from parameters
static MidiNote create_midi_note(
  NoteName name, int octave, int volume, int velocity, int modulation, int pitch
) {
    MidiNote midiNote;
    midiNote.noteData.name   = name;
    midiNote.noteData.octave = CLAMP( octave, 0, 8 );
    midiNote.volume          = CLAMP( volume, 0, 127 );
    midiNote.velocity        = CLAMP( velocity, 0, 127 );
    midiNote.modulation      = CLAMP( modulation, 0, 127 );
    midiNote.pitch           = CLAMP( pitch, 0, 127 );
    return midiNote;
}
