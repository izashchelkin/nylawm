#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <cstdint>
#include <cstring>

#include "internal.hpp"

namespace nyla {

void layout_update(xcb_connection_t* conn, LayoutState& layout_state,
                   xcb_screen_t* screen) {
    for (auto& [client_window, client] : layout_state.clients) {
        while (client.workspace_index >= layout_state.workspaces.size())
            layout_state.workspaces.emplace_back(WorkspaceState{});

        auto& workspace = layout_state.workspaces.at(client.workspace_index);
        while (client.stack_pos >= workspace.stacks.size())
            workspace.stacks.emplace_back(WorkspaceStackState{});

        auto& stack = workspace.stacks.at(client.stack_pos);

        const auto expected_geom =
            Rect{.x = 0,
                 .y = 0,
                 .width = static_cast<int32_t>(screen->width_in_pixels /
                                               (stack.clients.size() + 1)),
                 .height = screen->height_in_pixels};

        if (memcmp(&client.geom, &expected_geom, sizeof(Rect))) {
            xcb_configure_window(
                conn, client_window,
                (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                 XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT),
                (int32_t[]){expected_geom.x, expected_geom.y,
                            expected_geom.width, expected_geom.height});
            client.geom = expected_geom;
        }
    }
}

}    // namespace nyla
