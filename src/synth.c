float generate_sample( WaveformType wf, float phase ) {
    switch ( wf ) {
        case WAVEFORM_SINE: return sinf( phase );
        case WAVEFORM_SQUARE:
            return ( sinf( phase ) >= 0.0f ) ? 1.0f : -1.0f;             // revisit implementation
        case WAVEFORM_SAW: return 2.0f * fmodf( phase, 1.0f ) - 1.0f;    // revisit implementation
        case WAVEFORM_TRIANGLE:
            return 2.0f * fabsf( 2.0f * fmodf( phase, 1.0f ) - 1.0f ) -
                   1.0f;    // revisit implementation
    }
    return 0.0f;
}

// compute the frequency of a note from parameters
static float
  compute_frequency( int noteIndex, float baseFrequency, int baseIndex, float pitchOffset ) {
    int   clampedIndex     = CLAMP( noteIndex, 0, 107 );
    float semitoneDistance = (float) ( clampedIndex - baseIndex ) + pitchOffset;
    float freq             = baseFrequency * powf( 2.0f, semitoneDistance / 12.0f );
    return freq;
}
