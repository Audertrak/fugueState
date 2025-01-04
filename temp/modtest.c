#include <stdio.h>

int calc_octave( int input ) { return input / 12; }

int main() {
    int input;

    printf( "Enter number: " );
    scanf_s( "%d", &input );
    printf( "Number mod 12 = %d\n", calc_octave(input));

    return 0;
}
