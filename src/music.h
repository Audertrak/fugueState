/**
 * @file
 * @brief library that provides an api for music theory computations
 */

#ifndef MUSIC_H
#define MUSIC_H

#define CLAMP( value, minVal, maxVal )                                                            \
    ( ( value ) < ( minVal ) ? ( minVal ) : ( ( value ) > ( maxVal ) ? ( maxVal ) : ( value ) ) )

#define NOTA_MIN              0      // minimum note index, A0
#define NOTA_MAX              127    // maximum note index, G_SHARP9

#define DIAPASON_MIN          0     // lowest octave index
#define DIAPASON_MAX          9     // highest octave index
#define DIAPASON              12    // number of semitones in an octave

#define BASE_NOTA             A4        // reference note for frequency calculation
#define BASE_INDICE           48        // index of reference note
#define BASE_TUNING           440.0f    // standard tuning

#define MIN_FREQ              16.0f       // generally considered below human range of hearing
#define MAX_FREQ              20000.0f    // generally considered height of human hearing

#define HALF_STEP             1    // 1 semitone
#define WHOLE_STEP            2    // 2 semitone
#define PERFECT_FIFTH         7    // 3 semitone

#define MAX_CHORD_NOTES       7

#define MAJOR_SCALE_INTERVALS { 2, 2, 1, 2, 2, 2, 1 }    // major scale is the ionian mode

/* list of note 'names' */
typedef enum { C = 0, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A = 9, A_SHARP, B } NotaNomen;

/* 'full' note name, given by its name and octave */
typedef struct {
    NotaNomen nome;
    int       diapason;
} Nota;

/* list of interval modes  */
typedef enum {
    IONIAN,
    DORIAN,
    PHRYGIAN,
    LYDIAN,
    MIXOLYDIAN,
    AEOLIAN,
    LOCRIAN,
} Modus;

/* scale definition  */
typedef struct {
    Nota  tonic;
    Nota  notas[7];
    Modus modus;
} Scalae;

/* list of chord types or 'qualities' */
typedef enum {
    MAJOR,
    MINOR,
    DIMINISHED,    // diminitus
    AUGMENTED,     // auctus
    SUS2,          // suspensus
    SUS4
} Qualitas;

/* list of chord extensions */
typedef enum {
    SEVENTH,    // septima
    MAJ7TH,
    NINTH,        // nona
    ELEVENTH,     // undecima
    THIRTEENTH    // tertiadecima
} Extensio;

/* chord definition */
typedef struct {
    Nota     root;
    Qualitas quality;
    Extensio extension;
    bool     inversion;
    int      inversion_grade;
    Nota     notas[MAX_CHORD_NOTES];
    int      num_notes;
} Chorda;

/**
 * @brief Converts from note 'name' to note 'index'
 *
 * @param n [TODO:parameter]
 * @return [TODO:return]
 */
int   nota_to_indice( const Nota *n );

/**
 * @brief Converts from note 'index' to note 'name'
 *
 * @param indice [TODO:parameter]
 * @return [TODO:return]
 */
Nota  indice_to_nota( int indice );

/**
 * @brief Calculate the frequency of a given note by its index and the tuning base frequency
 *
 * @param indiceNota [TODO:parameter]
 * @param BASE_TUNING [TODO:parameter]
 * @param BASE_INDICE [TODO:parameter]
 * @param semiTone [TODO:parameter]
 * @return [TODO:return]
 */
float nota_frequency( int indiceNota, float (BASE_TUNING), int BASE_INDICE, float semiTone );

/**
 * @brief Calculate the intervals of a given mode
 *
 * @param modus [TODO:parameter]
 * @param intervals [TODO:parameter]
 */
void  generate_modal_intervals( Modus modus, int *intervals );

// returns the notes within a scale given the tonic and a mode
/**
 * @brief Returns the notes within a scale given the tonic and a mode
 *
 * @param tonic [TODO:parameter]
 * @param modus [TODO:parameter]
 * @param scalae [TODO:parameter]
 */
void  generate_scalae( const Nota *tonic, Modus modus, Scalae *scalae );

/**
 * @brief Construct a chord from root note and quality
 */
void  generate_chorda( const Nota *root, Qualitas quality, Extensio extension, Chorda *chord );

/**
 * @brief Identify a chord from a set of notes
 */
void  identify_chorda( const Nota *notas, int num_notes, Chorda *chorda );

/**
 * @brief Convert chord to string representation (e.g., "Cmaj7")
 */
void  consortium_to_string( const Chorda *chorda, char *str, size_t size );

/**
 *
 * @brief Get intervals for a given chord quality
 */
void  consortium_intervalla(
   Qualitas quality, Extensio extension, int *intervals, int *num_intervals
 );

/**
 * @brief Check if a set of notes forms a valid chord
 */
bool est_chorda( const Nota *notas, int num_notes );

/**
 * @brief Get the inversion of a chord
 */
void invert_chorda( Chorda *chorda, int inversion_grade );

/**
 * @brief Get all possible voicings of a chord
 */
void chorda_voces( const Chorda *chorda, Chorda *voces, int *num_voces );

#endif
