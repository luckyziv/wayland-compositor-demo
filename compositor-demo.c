#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define WLR_USE_UNSTABLE
//wlroot headers
#include <wlr/backend.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_primary_selection_v1.h>
#include <wlr/types/wlr_idle.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/backend/drm.h>

//wayland protocol
#include <wayland-server.h>

struct mcw_server {
    struct wl_display *wl_display;
    struct wl_event_loop *wl_event_loop;

    struct wlr_backend *backend;

    struct wl_listener new_outputs; //output wl_signal listener
    struct wl_list outputs; //save output some states ???
};

struct mcw_output {
    struct wlr_output *wlr_output;  //current output ptr
    struct mcw_server *server;  //output owner
    struct timespec last_frame;

    struct wl_listener destroy; //output destroy listener
    struct wl_listener frame;

    struct wl_list link;    //???
};

static void output_destroy_notify(struct wl_listener *listener, void *data)
{
    struct mcw_output *output = wl_container_of(listener, output, destroy);
    wl_list_remove(&output->link);
    wl_list_remove(&output->destroy.link);
    // wl_list_remove(&output->last_frame);
    free(output);
};

static void output_frame_notiry(struct wl_listener *listener, void *data)
{
    struct mcw_output *output = wl_container_of(listener, output, destroy);
    struct wlr_output *wlr_output = data;

    wlr_output_attach_render(wlr_output, NULL);
    struct wlr_renderer *renderer = wlr_backend_get_renderer(wlr_output->backend);

    // wlr_output_make_current(wlr_output, NULL);
    // wlr_renderer_begin(renderer, wlr_output);
    wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);

    float color[4] = {0, 1.0, 0, 1.0};
    wlr_renderer_clear(renderer, color);

    // wlr_output_swap_buffers(wlr_output, NULL, NULL);
    wlr_renderer_end(renderer);
    wlr_output_commit(wlr_output);
}


static void new_output_notify(struct wl_listener *listener, void *data)
{
    struct mcw_server *server = wl_container_of(listener, server, new_outputs);
    struct wlr_output *wlr_output = data;

    // set output mode
    if(!wl_list_empty(&wlr_output->modes)) {
        struct wlr_output_mode *mode = wl_container_of(wlr_output->modes.prev, mode, link);
        wlr_output_set_mode(wlr_output, mode);
    }

    // save output state
    struct mcw_output *output = calloc(1, sizeof(struct mcw_output));
    clock_gettime(CLOCK_MONOTONIC, &output->last_frame);
    output->server = server;
    output->wlr_output = wlr_output;
    wl_list_insert(&server->outputs, &output->link);

    // add listener for output destroy
    output->destroy.notify = output_destroy_notify;
    wl_signal_add(&wlr_output->events.destroy, &output->destroy);

    // add listener for new frame
    output->frame.notify = output_frame_notiry;
    wl_signal_add(&wlr_output->events.frame, &output->frame);
};

int main(int argc, char **argv)
{
    struct mcw_server server;

    server.wl_display = wl_display_create();
    server.wl_event_loop = wl_display_get_event_loop(server.wl_display);

    server.backend = wlr_backend_autocreate(server.wl_display, NULL);

    wl_list_init(&server.outputs);

    // add listener for new output of server.backend->events.new_output
    server.new_outputs.notify = new_output_notify;  //new output notify
    wl_signal_add(&server.backend->events.new_output, &server.new_outputs); //add listener of output wl_signal

    const char *socket = wl_display_add_socket_auto(server.wl_display);

    if (!wlr_backend_start(server.backend)) {
        fprintf(stderr, "Failed to start backedn\n");
        wl_display_destroy(server.wl_display);
        return 1;
    }

    printf("Running compositor on wayland display '%s'\n", socket);
    setenv("WAYLAND_DISPLAY", socket, true);

    wl_display_init_shm(server.wl_display);
    wlr_gamma_control_manager_v1_create(server.wl_display);
    wlr_screencopy_manager_v1_create(server.wl_display);
    wlr_primary_selection_v1_device_manager_create(server.wl_display);
    wlr_idle_create(server.wl_display);

    wl_display_run(server.wl_display);
    wl_display_destroy(server.wl_display);

    return 0;
}



