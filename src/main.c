#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "renderers/raylib/clay_renderer_raylib.c"
#include <stdint.h>

// global declarations
bool debugEnabled = true;

// rose pine color palette
#define COLOR_BASE    ( Clay_Color ){ 25, 25, 36, 255 }
#define COLOR_SURFACE ( Clay_Color ){ 31, 290, 46, 255 }
#define COLOR_OVERLAY ( Clay_Color ){ 38, 35, 58, 255 }
#define COLOR_MUTED   ( Clay_Color ){ 110, 106, 134, 255 }
#define COLOR_SUBTLE  ( Clay_Color ){ 144, 140, 170, 255 }
#define COLOR_TEXT    ( Clay_Color ){ 224, 222, 244, 255 }
#define COLOR_LOVE    ( Clay_Color ){ 235, 111, 146, 255 }
#define COLOR_GOLD    ( Clay_Color ){ 246, 193, 119, 255 }
#define COLOR_ROSE    ( Clay_Color ){ 235, 111, 146, 255 }
#define COLOR_PINE    ( Clay_Color ){ 49, 116, 143, 255 }
#define COLOR_FOAM    ( Clay_Color ){ 156, 207, 216, 255 }
#define COLOR_IRIS    ( Clay_Color ){ 196, 167, 231, 255 }
#define COLOR_LOW     ( Clay_Color ){ 33, 32, 46, 255 }
#define COLOR_MED     ( Clay_Color ){ 64, 61, 82, 255 }
#define COLOR_HIGH    ( Clay_Color ){ 82, 79, 103, 255 }

typedef enum {
    A,
    A_FLAT,
    A_SHARP,
    B,
    B_FLAT,
    B_SHARP,
    C,
    C_FLAT,
    C_SHARP,
    D,
    D_FLAT,
    D_SHARP,
    E,
    E_FLAT,
    E_SHARP,
    F,
    F_FLAT,
    F_SHARP,
    G,
    G_FLAT,
    G_SHARP
} Note;

typedef struct {
    Note  note;
    int   octave;
    float frequency;
    int   velocity;
} MidiNote;

// re-usable components
void RenderHeaderButton( Clay_String text ) {
    CLAY(
      CLAY_LAYOUT( {
        .padding = { 16, 8 }
    } ),
      CLAY_RECTANGLE( { .color = Clay_Hovered() ? COLOR_ROSE : COLOR_GOLD, .cornerRadius = 5 } )
    ) {
        CLAY_TEXT( text, CLAY_TEXT_CONFIG( { .fontSize = 16, .textColor = COLOR_TEXT } ) );
    }
}

void RenderDropdownTextItem( Clay_String text ) {
    CLAY( CLAY_LAYOUT( {
      .padding = { 16, 16 }
    } ) ) {
        CLAY_TEXT( text, CLAY_TEXT_CONFIG( { .fontSize = 16, .textColor = COLOR_TEXT } ) );
    }
}

Clay_LayoutConfig dropdownTextItemLayout = (Clay_LayoutConfig) {
  .padding = { 8, 4 }
};
Clay_RectangleElementConfig dropdownRectangleConfig =
  (Clay_RectangleElementConfig) { .color = COLOR_MED };
Clay_TextElementConfig dropdownTextElementConfig =
  (Clay_TextElementConfig) { .fontSize = 24, .textColor = COLOR_TEXT };

Clay_RenderCommandArray CreateLayout() {
    Clay_BeginLayout();
    // main container
    CLAY(
      CLAY_ID( "OuterContainer" ),
      CLAY_LAYOUT( {
        .sizing   = {.width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_GROW()},
        .padding  = {16,                          16                          },
        .childGap = 16
    } ),
      CLAY_RECTANGLE( { .color = COLOR_BASE } )
    ) {
        // menu bar
        CLAY(
          CLAY_ID( "MenuBar" ),
          CLAY_LAYOUT( {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing          = {.width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_FIXED( 48 )},
            .padding         = {16,                          16                               },
            .childGap        = 16
        } ),
          CLAY_RECTANGLE( { .color = COLOR_LOW } )
        ) {
            RenderHeaderButton( CLAY_STRING( "File" ) );
            RenderHeaderButton( CLAY_STRING( "Edit" ) );
            RenderHeaderButton( CLAY_STRING( "View" ) );
        }
    }
    return Clay_EndLayout();
}

void UpdateDrawFrame( void ) {
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float   mouseWheelX     = mouseWheelDelta.x;
    float   mouseWheelY     = mouseWheelDelta.y;

    if ( IsKeyPressed( KEY_D ) ) {
        debugEnabled = !debugEnabled;
        Clay_SetDebugModeEnabled( debugEnabled );
    }

    Clay_RenderCommandArray renderCommands = CreateLayout();

    BeginDrawing();
    ClearBackground( BLACK );
    Clay_Raylib_Render( renderCommands );
    EndDrawing();
}

int main( void ) {

    // initialize memory allocation
    uint64_t   totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory      = ( Clay_Arena ) {
        .label = CLAY_STRING( "Clay Memory Arena" )
        , .memory = malloc( totalMemorySize ), .capacity = totalMemorySize
    };
    Clay_SetMeasureTextFunction( Raylib_MeasureText );
    Clay_Initialize(
      clayMemory, (Clay_Dimensions) { (float) GetScreenWidth(), (float) GetScreenHeight() }
    );
    // initialize raylib
    Clay_Raylib_Initialize(
      1024, 768, "Window Title",
      FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT
    );

    // main loop
    while ( !WindowShouldClose() ) { UpdateDrawFrame(); }

    return 0;
}
