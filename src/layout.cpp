#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <cstdint>

#include "internal.hpp"

namespace nyla {

void layout_place_new_client(xcb_connection_t* conn, LayoutState& layout_state,
                             xcb_window_t client_window) {
    auto& active_workspace =
        layout_state.workspaces.at(layout_state.active_workspace_index);
    auto& active_stack =
        active_workspace.stacks.at(layout_state.active_stack_index);

    layout_state.clients.emplace(
        client_window,
        ClientState{.workspace_index = layout_state.active_workspace_index,
                    .stack_pos = active_stack.clients.size() < 4
                                     ? layout_state.active_stack_index
                                     : });
}

void layout_update(xcb_connection_t* conn, LayoutState& layout_state,
                   xcb_screen_t& screen) {
    for (auto& [client_window, client] : layout_state.clients) {
        while (client.workspace_index >= layout_state.workspaces.size())
            layout_state.workspaces.emplace_back(WorkspaceState{});

        auto& workspace = layout_state.workspaces.at(client.workspace_index);
        while (client.stack_pos >= workspace.stacks.size())
            workspace.stacks.emplace_back(WorkspaceStackState{});

        auto& stack = workspace.stacks.at(client.stack_pos);

        bool must_be_visible =
            client.workspace_index == layout_state.active_workspace_index &&
            client.stack_pos == layout_state.active_stack_index;

        if (client.flags & ClientState::Visible) {
            if (!must_be_visible) {
                xcb_configure_window(conn, client_window,
                                     XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                                     (uint32_t[]){screen.width_in_pixels,
                                                  screen.height_in_pixels});
                client.flags &= ~ClientState::Visible;
            }
        } else {
            if (must_be_visible) {
                xcb_configure_window(conn, client_window,
                                     XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                                     (uint32_t[]){0, 0});
                client.flags |= ClientState::Visible;
            }
        }
    }
}

}    // namespace nyla
