/**
 * @file
 * @brief header file for a simple synth implementation
 */

// top level definitions
#ifndef SYNTH_H
#define SYNTH_H

// top level includes
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// platform identification
#ifdef _WIN32
  #include <windows.h>
  #define AUDIO_API_WINDOWS
  #include <mmeapi.h>
  #include <mmsystem.h>
  #ifdef _MSC_VER
    #pragma comment( lib, "winm.lib" )
  #endif
extern HWAVEOUT hWaveOut;
extern WAVEHDR  waveHeader;
#elif __LINUX__
  #define AUDIO_API_ALSA
  #include <alsa/asoundlib.h>
  #include <pthread.h>
#elif __APPLE__
  #define AUDIO_API_COREAUDIO
  #include <AudioUnit/AudioUnit.h>
  #include <pthread.h>
#endif

/******************
 * ERROR HANDLING *
 *****************/
typedef enum SynthError {
    SYNTH_ACK                       = 0,
    SYNTH_ERROR_NULL_PTR            = -1,
    SYNTH_ERROR_INVALID_PARAM       = -2,
    SYNTH_ERROR_OOM                 = -3,
    SYNTH_ERROR_BUFFER_OVERFLOW     = -4,
    SYNTH_ERROR_INIT_FAILED         = -5,
    SYNTH_ERROR_EXCEEDED_MAX_VOICES = -6,
    SYNTH_ERROR_VOICE_NOT_FOUND     = -7,
    SYNTH_ERROR_ARENA_FULL          = -8
} SynthError;

/*************
 * CONSTANTS *
 ************/
#define PI                 3.14159265358979323846f
#define SAMPLE_RATE        44100
#define BIT_RATE           16
#define MAX_VOICES         64
#define BUFFER_SIZE        ( SAMPLE_RATE * 2 )
#define MAX_BASE_WAVEFORMS 16

/****************
 * MEMORY ARENA *
 ***************/
#define SYNTH_ARENA_SIZE   1024 * 1024    // 1MB
#define SYNTH_ARENA_ALIGN  8

typedef struct {
    uint8_t *buffer;
    size_t   size;
    size_t   used;
#if defined(__linux__) ||  defined(__APPLE__)
    pthread_mutex_t mutex;
#elif defined( _WIN32 )
    CRITICAL_SECTION mutex;
#endif
} SynthArena;

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 * @param size [TODO:parameter]
 */
static inline void arena_init( SynthArena *arena, size_t size ) {
    if ( !arena ) return;
    arena->buffer = (uint8_t *) malloc( size );
    arena->size   = size;
    arena->used   = 0;
#ifdef defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_init( &arena->mutex, NULL );
#elif defined( _WIN32 )
    InitializeCriticalSection( &arena->mutex );
#endif
}

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 */
static inline void arena_lock( SynthArena *arena ) {
#ifdef defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_lock( &arena->mutex );
#elif defined( _WIN32 )
    EnterCriticalSection( &arena->mutex );
#endif
}

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 */
static inline void arena_unlock( SynthArena *arena ) {
#ifdef defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_unlock( &arena->mutex );
#elif defined( _WIN32 )
    LeaveCriticalSection( &arena->mutex );
#endif
}

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 * @param size [TODO:parameter]
 */
static inline void *arena_alloc( SynthArena *arena, size_t size ) {
    // if the arena is not initialized or the buffer is NULL, return NULL
    if ( !arena || !arena->buffer ) return NULL;
    arena_lock( arena );                                                     // lock the arena
    size = ( size + SYNTH_ARENA_ALIGN - 1 ) & ~( SYNTH_ARENA_ALIGN - 1 );    // align size

    // if the arena is full, return NULL
    if ( arena->used + size > arena->size ) {
        arena_unlock( arena );    // unlock the arena
        return NULL;
    }

    void *ptr    = arena->buffer + arena->used;    // get the pointer
    arena->used += size;                           // update the used size

    arena_unlock( arena );    // unlock the arena
    return ptr;               // return the pointer
}

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 */
static inline void arena_reset( SynthArena *arena ) {
    if ( !arena ) return;
    arena_lock( arena );
    arena->used = 0;
    arena_unlock( arena );
}

/**
 * @brief [TODO:description]
 *
 * @param arena [TODO:parameter]
 */
static inline void arena_destroy( SynthArena *arena ) {
    if ( !arena ) return;
#if defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_destroy( &arena->mutex );
#elif defined( _WIN32 )
    DeleteCriticalSection( &arena->mutex );
#endif
    free( arena->buffer );
    arena->buffer = NULL;
    arena->size   = 0;
    arena->used   = 0;
}

/*******************************
 * SYNTHESIZER DATA STRUCTURES *
 ******************************/
// basic waveforms
typedef enum {
    INVALID_WAVEFORM = -1,
    WAVEFORM_SINE    = 0,
    WAVEFORM_SQUARE,
    WAVEFORM_SAW,
    WAVEFORM_TRIANGLE,
    WAVEFORM_COUNT
} BaseWaveform;

// function pointer type for waveform generation
typedef float ( *WaveformFunction )( float phase );

// ADSR envelope
typedef struct {
    bool    isActive;
    uint8_t attack;
    uint8_t decay;
    uint8_t sustain;
    uint8_t release;
    uint8_t currentLevel;
    float   currentTime;
} Envelope;

// voice structure
typedef struct {
    bool         active;
    BaseWaveform waveform;
    float        frequency;
    float        phase;
    float        phaseIncrement;
    Envelope     env;
} Voice;

// custom waveform registration
typedef struct {
    BaseWaveform     type;
    WaveformFunction func;
    const char      *name;
} WaveformEntry;

// platform context for synthesizer implementation
typedef struct AudioContext AudioContext;

// main synthesizer structure
typedef struct {
    Voice         *voices;
    uint8_t        maxVoices;
    uint8_t        numActiveVoices;
    float          masterVolume;
    float          sampleRate;
    AudioContext  *audio;
    SynthArena     arena;
    WaveformEntry *customWaveforms;
    uint8_t        numCustomWaveforms;
#if defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_t mutex;
#elif defined( _WIN32 )
    CRITICAL_SECTION mutex;
#endif
} Synthesizer;

/***********************************
 * PLATFORM SPECIFIC AUDIO CONTEXT *
 **********************************/

#ifdef AUDIO_API_WINDOWS
typedef struct {
    HWAVEOUT hwaveOut;
    WAVEHDR  waveHeader;
    int16_t *buffer;
    size_t   bufferSize;
} WindowsAudioContext;

struct AudioContext {
    WindowsAudioContext platform;
    uint32_t            sampleRate;
    uint8_t             channels;
};

#elif defined( AUDIO_API_ALSA )
typedef struct {
    snd_pcm_t           *handle;
    snd_pcm_hw_params_t *params;
    int1int16_t         *buffer;
    size_t               bufferSize;
} AlsaAudioContext;

struct AudioContext {
    AlsaAudioContext platform;
    uint32_t         sampleRate;
    uint8_t          channels;
};

#elif defined( AUDIO_API_COREAUDIO )
typedef struct {
    AudioUnit        audioUnit;
    AudioBufferList *bufferList;
    intint16_t      *buffer;
    size_t           bufferSize;
} CoreAudioContext;

struct AudioContext {
    CoreAudioContext platform;
    uint32_t         sampleRate;
    uint8_t          channels;
};
#endif

/*****************************
 * BASIC WAVEFORM GENERATORS *
 ****************************/
float sine_wave( float phase ) { return sinf( 2.0f * PI * phase ); }

float square_wave( float phase ) { return phase < 0.5f ? 1.0f : -1.0f; }

float saw_wave( float phase ) { return 2.0f * phase - 1.0f; }

float triangle_wave( float phase ) { return 2.0f * fabsf( 2.0f * phase - 1.0f ) - 1.0f; }

// array of function pointers mirroring waveform enum
static const WaveformFunction waveform_functions[WAVEFORM_COUNT] = {
  sine_wave, square_wave, saw_wave, triangle_wave
};

/***********************************
 * SYNTHESIZER FUNCTION PROTOTYPES *
 **********************************/
static inline SynthError synth_init( Synthesizer *synth, uint8_t maxVoices, uint8_t channels ) {
    if ( !synth ) return SYNTH_ERROR_NULL_PTR;    // check for invalid parameters

    arena_init( &synth->arena, SYNTH_ARENA_SIZE );    // initialize memory arena

    synth->voices =
      arena_alloc( &synth->arena, maxVoices * sizeof( Voice ) );    // allocate memory for voices
    if ( !synth->voices ) return SYNTH_ERROR_OOM;                   // check for out of memory

// initialize mutex
#ifdef defined( __linux__ ) || defined( __APPLE__ )
    pthread_mutex_init( &synth->mutex, NULL );
#elif defined( _WIN32 )
    InitializeCriticalSection( &synth->mutex );
#endif

    // initialize waveform array
    synth->customWaveforms =
      arena_alloc( &synth->arena, sizeof( WaveformEntry ) * MAX_BASE_WAVEFORMS );
    if ( !synth->customWaveforms ) return SYNTH_ERROR_OOM;    // check for out of memory

    synth->maxVoices          = MAX_VOICES;
    synth->numActiveVoices    = 0;
    synth->masterVolume       = 1.0f;
    synth->sampleRate         = SAMPLE_RATE;
    synth->numCustomWaveforms = 0;

    // initialize audio context
    synth->audio = (AudioContext *) arena_alloc( &synth->arena, sizeof( AudioContext ) );
    if ( !synth->audio ) return SYNTH_ERROR_OOM;    // check for out of memory

    synth->audio->sampleRate = SAMPLE_RATE;
    synth->audio->channels   = channels;

    // initialize all voices
    memset( synth->voices, 0, maxVoices * sizeof( Voice ) );

    return SYNTH_ACK;
}

static short  audioBuffer[BUFFER_SIZE];

AudioContext *audio_init( int sample_rate, int channels );
void          audio_write( AudioContext *ctx, float *samples, int numSamples );
void          audio_close( AudioContext *ctx );

float         get_sample( BaseWaveform type, float phase ) {
    if ( type < 0 || type >= WAVEFORM_COUNT ) { return 0.0f; }
    return waveform_functions[type]( phase );
}

static WaveformEntry custom_waveforms[MAX_BASE_WAVEFORMS];
static int           num_custom_waveforms = 0;

/**
 * @brief [TODO:description]
 *
 * @param func [TODO:parameter]
 * @param name [TODO:parameter]
 * @return [TODO:return]
 */
BaseWaveform         register_custom_waveform( WaveformFunction func, const char *name ) {
    if ( num_custom_waveforms >= MAX_BASE_WAVEFORMS ) return INVALID_WAVEFORM;
    BaseWaveform new_type = (BaseWaveform) ( WAVEFORM_COUNT + num_custom_waveforms );
    custom_waveforms[num_custom_waveforms++] = (WaveformEntry) { new_type, func, name };
    return new_type;
}

/**
 * @brief [TODO:description]
 *
 * @param buffer [TODO:parameter]
 * @param length [TODO:parameter]
 * @param type [TODO:parameter]
 * @param frequency [TODO:parameter]
 * @param sample_rate [TODO:parameter]
 */
inline void generate_waveform(
  float *buffer, int length, BaseWaveform type, float frequency, float sample_rate
) {
    for ( int i = 0; i < length; i++ ) {
        float phase = fmodf( ( frequency * i ) / sample_rate, 1.0f );
        buffer[i]   = get_sample( type, phase );
    }
}

/**
 * @brief [TODO:description]
 *
 * @param buffer [TODO:parameter]
 * @param numSamples [TODO:parameter]
 * @param frequency [TODO:parameter]
 */
inline void generateTone( short *buffer, int numSamples, float frequency ) {
    for ( int i = 0; i < numSamples; i++ ) {
        float t   = (float) i / SAMPLE_RATE;
        buffer[i] = (short) ( 32767.0f * sin( 2.0f * 3.14159f * frequency * t ) );
    }
}

// Core synth functions
void  synth_init( Synthesizer *synth );

void  synth_process_buffer( Synthesizer *synth, float *buffer, int numSamples );
int   synth_trigger_note( Synthesizer *synth, float frequency, float amplitude );
void  synth_release_note( Synthesizer *synth, int voiceIndex );
void  synth_set_master_volume( Synthesizer *synth, float volume );

// Voice management
int   synth_get_free_voice( Synthesizer *synth );
void  synth_update_voice( Voice *voice, float deltaTime );

// Audio generation
float synth_generate_sample( BaseWaveform wf, float phase );

// define platform specific AudioContexts
#ifndef SYNTH_PLATFORM_H
  #define SYNTH_PLATFORM_H

// generic audio context
struct AudioContext {
    int    sampleRate;
    int    channels;
    size_t bufferSize;
  #ifdef AUDIO_API_WINDOWS
    WindowsAudioContext platformctx;
  #elif defined( AUDIO_API_ALSA )
    AlsaAudioContext platformctx;
  #elif defined( AUDIO_API_COREAUDIO )
    CoreAudioContext platformctx;
  #endif
};

  // platform specific implementations
  #ifdef AUDIO_API_WINDOWS
static inline int platform_audio_init( AudioContext *ctx ) {
    WAVEFORMATEX waveFormat = {
      .wFormatTag      = WAVE_FORMAT_PCM,
      .nChannels       = (WORD) ctx->channels,
      .nSamplesPerSec  = (DWORD) ctx->sampleRate,
      .wBitsPerSample  = (WORD) BIT_RATE,
      .nBlockAlign     = (WORD) ( ( ctx->channels * BIT_RATE ) / 8 ),
      .nAvgBytesPerSec = (DWORD) ctx->sampleRate * ( ctx->channels * BIT_RATE ) / 8,
      .cbSize          = 0
    };

    if ( waveOutOpen( &ctx->platformctx.hwaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL ) !=
         MMSYSERR_NOERROR ) {
        return -1;
    }

    ctx->platformctx.buffer            = (short *) malloc( ctx->bufferSize * sizeof( short ) );
    ctx->platformctx.bufferSize        = ctx->bufferSize;

    ctx->platformctx.waveHeader.lpData = (LPSTR) ctx->platformctx.buffer;
    ctx->platformctx.waveHeader.dwBufferLength = ctx->bufferSize * sizeof( short );
    ctx->platformctx.waveHeader.dwFlags        = 0;

    return 0;
}

static inline void platform_audio_write( AudioContext *ctx, float *samples, int numSamples ) {
    // convert float samples to short and write to buffer
    for ( int i = 0; i < numSamples; i++ ) {
        ctx->platformctx.buffer[i] = (short) ( samples[i] * 32767.0f );
    }

    waveOutPrepareHeader(
      ctx->platformctx.hwaveOut, &ctx->platformctx.waveHeader, sizeof( WAVEHDR )
    );
    waveOutWrite( ctx->platformctx.hwaveOut, &ctx->platformctx.waveHeader, sizeof( WAVEHDR ) );
}

static inline void platform_audio_close( AudioContext *ctx ) {
    waveOutUnprepareHeader(
      ctx->platformctx.hwaveOut, &ctx->platformctx.waveHeader, sizeof( WAVEHDR )
    );
    free( ctx->platformctx.buffer );
}

  #elif defined( AUDIO_API_ALSA )
// ALSA implementation
static inline int platform_audio_init( AudioContext *ctx ) { return 0; }

        // additonal ALSA functions...

  #elif defined( AUDIO_API_COREAUDIO )
// CoreAudio implementation
static inline int platform_audio_init( AudioContext *ctx ) { return 0; }

  // additonal CoreAudio functions...
  #endif

AudioContext *audio_init( int sample_rate, int channels ) {
    AudioContext *ctx = (AudioContext *) malloc( sizeof( AudioContext ) );
    ctx->sampleRate   = sample_rate;
    ctx->channels     = channels;
    ctx->bufferSize   = BUFFER_SIZE;

    if ( platform_audio_init( ctx ) != 0 ) {
        free( ctx );
        return NULL;
    }
    return ctx;
}

void audio_write( AudioContext *ctx, float *samples, int numSamples ) {
    platform_audio_write( ctx, samples, numSamples );
}

void audio_close( AudioContext *ctx ) {
    if ( ctx ) {
        platform_audio_close( ctx );
        free( ctx );
    }
}

#endif
#endif
