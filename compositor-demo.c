#include <stdio.h>

//wlroot headers
// #include <wlr/backend.h>

//wayland protocol
#include <wayland-server.h>

struct mcv_server {
    struct wl_display *wl_display;
    struct wl_event_loop *wl_event_loop;

};

int main(int argc, char **argv)
{
    struct mcv_server server;

    server.wl_display = wl_display_create();
    server.wl_event_loop = wl_display_get_event_loop(server.wl_display);

    return 0;
}



