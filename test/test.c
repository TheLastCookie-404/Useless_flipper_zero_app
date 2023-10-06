#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "test_icons.h"
#define TAG "testApp"

typedef struct {
    uint8_t x, y;
} ImageSize;

ImageSize arrows_size = {.x = 24, .y = 13};
const Icon* icons[] = {
    &I_Meme,
    &I_Meme2,
    &I_Meme3,
    &I_Meme4,
    &I_Meme5,
    &I_Meme6,
    &I_Rei,
    &I_Meme7,
    &I_Meme8,
    &I_Meme9,
    &I_Meme10,
    &I_Meme11,
    &I_Meme12};
uint iconsSize = (sizeof(icons) / sizeof(icons[0])) - 1; // -1 Because array start with 0
uint iconsCounter = 0;

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, icons[iconsCounter]);
    canvas_draw_icon(canvas, 128 - arrows_size.x, 64 - arrows_size.y, &I_Arrows);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t test_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    FURI_LOG_E(TAG, "Size of array %i", iconsSize);

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                if(event.key == InputKeyRight && iconsCounter < iconsSize) iconsCounter++;
                if(event.key == InputKeyLeft && iconsCounter > 0) iconsCounter--;
                if(event.key == InputKeyBack) running = false;
            }
        }
    }
    view_port_update(view_port);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
