#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <cstdint>

#include "internal.hpp"

namespace nyla {

void event_loop(xcb_connection_t* conn, xcb_screen_t* screen, bool* running,
                KeyboardState& keyboard_state, LayoutState& layout_state) {
    while (*running) {
        layout_update(conn, layout_state, screen);
        xcb_flush(conn);
        xcb_generic_event_t* event = xcb_wait_for_event(conn);
        if (!event) break;

        uint8_t event_type = event->response_type & ~0x80;
        switch (event_type) {
            case 0: {
                handle_error(*reinterpret_cast<xcb_generic_error_t*>(event));
                break;
            }
            case XCB_MAP_REQUEST: {
                handle_map_request(
                    conn, *reinterpret_cast<xcb_map_request_event_t*>(event),
                    layout_state);
                break;
            }
            case XCB_UNMAP_NOTIFY: {
                handle_unmap_notify(
                    conn, *reinterpret_cast<xcb_unmap_notify_event_t*>(event),
                    layout_state);
                break;
            }
            case XCB_CONFIGURE_REQUEST: {
                handle_configure_request(
                    conn,
                    *reinterpret_cast<xcb_configure_request_event_t*>(event));
                break;
            }
            case XCB_KEY_PRESS: {
                handle_key_press(
                    conn, keyboard_state, running,
                    *reinterpret_cast<xcb_key_press_event_t*>(event));
                break;
            }

                // case XCB_KEY_RELEASE: {
                //     handle_key_release(
                //         conn,
                //         *reinterpret_cast<xcb_key_release_event_t*>(event));
                //     break;
                // }
        }
    }
}

}    // namespace nyla
