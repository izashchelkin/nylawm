#pragma once

#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace nyla {

int main();

bool spawn(std::span<const char* const> cmd);

enum class Action {
    ExitWM,
    SpawnTerminal,
};

struct KeyboardState {
    std::unordered_map<const char*, nyla::Action> config;
    std::unordered_map<xcb_keycode_t, Action> keybinds;
    uint16_t modifier;
};

void init_keyboard(xcb_connection_t* conn);
void grab_keys(xcb_connection_t* conn, xcb_window_t root_window,
               KeyboardState& keyboard_state);

struct Rect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

struct ClientState {
    // xcb_window_t window;
    Rect geom;
    uint8_t workspace_index;
    uint8_t stack_pos;
    uint8_t index;
    uint8_t flags;

    static constexpr uint8_t Visible = 1 << 0;
};

struct WorkspaceStackState {
    std::vector<xcb_window_t> clients;
};

struct WorkspaceState {
    std::vector<WorkspaceStackState> stacks;
};

struct LayoutState {
    std::unordered_map<xcb_window_t, ClientState> clients;
    std::vector<WorkspaceState> workspaces;
    uint8_t active_workspace_index;
};

void event_loop(xcb_connection_t* conn, bool* running,
                KeyboardState& keyboard_state);

void handle_error(xcb_generic_error_t& error);
void handle_map_request(xcb_connection_t* conn, xcb_map_request_event_t& event);
void handle_configure_request(xcb_connection_t* conn,
                              xcb_configure_request_event_t& event);
void handle_key_press(xcb_connection_t* conn, KeyboardState keyboard_state,
                      bool* running, xcb_key_press_event_t& event);
void handle_key_release(xcb_connection_t* conn, xcb_key_release_event_t& event);

}    // namespace nyla
