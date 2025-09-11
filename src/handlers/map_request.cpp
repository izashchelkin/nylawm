#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "src/internal.hpp"

namespace nyla {

void handle_map_request(xcb_connection_t* conn, xcb_map_request_event_t& event,
                        LayoutState& layout_state) {
    xcb_map_window(conn, event.window);

    layout_state.clients.emplace(
        event.window,
        ClientState{
            .geom = Rect{},
            .workspace_index = layout_state.active_workspace_index,
            .stack_pos = layout_state.active_stack_index,
        });

    // TODO: move to workspace/stack switch logic place
    // while (layout_state.workspaces.size() <= layout_state.active_workspace_index)
    //     layout_state.workspaces.emplace_back();

    auto& workspace =
        layout_state.workspaces[layout_state.active_workspace_index];

    // TODO: same here
    // while (workspace.stacks.size() <= layout_state.active_stack_index)
    //     workspace.stacks.emplace_back();

    workspace.stacks.reserve(layout_state.active_stack_index);
    auto& stack = workspace.stacks[layout_state.active_stack_index];
    stack.clients.emplace(event.window);
}

void handle_unmap_notify(xcb_connection_t* conn,
                         xcb_unmap_notify_event_t& event,
                         LayoutState& layout_state) {
    if (!layout_state.clients.contains(event.window)) return;

    auto& client = layout_state.clients.at(event.window);
    layout_state.workspaces.at(client.workspace_index)
        .stacks.at(client.stack_pos)
        .clients.erase(event.window);

    layout_state.clients.erase(event.window);
}

}    // namespace nyla
