#include "internal.hpp"

namespace nyla {

void sync_layout(LayoutState& layout_state) {
    for (auto& [client_window, client] : layout_state.clients) {
        while (client.workspace_index >= layout_state.workspaces.size())
            layout_state.workspaces.emplace_back(WorkspaceState{});

        if (client.flags & ClientState::Visible &&
            client.workspace_index != layout_state.active_workspace_index) {
            // TODO: move offscreen
        }

        auto& workspace = layout_state.workspaces.at(client.workspace_index);
        while (client.stack_pos >= workspace.stacks.size())
            workspace.stacks.emplace_back(WorkspaceStackState{});

        auto& stack = workspace.stacks.at(client.stack_pos);
    }
}

}    // namespace nyla
