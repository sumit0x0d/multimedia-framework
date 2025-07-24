#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <linux/input-event-codes.h>

#include "video-playback-wayland.h"

typedef struct video_playback_wayland Wayland;
typedef struct video_playback_create_config CreateConfig;

static void *_wayland_create(
     const CreateConfig *cconfig);
static void _wayland_destroy(
     void *backend);

struct video_playback_interface video_playback_interface_wayland = {
     .create = _wayland_create,
     .destroy = _wayland_destroy,
     .dispatch = NULL
};

static void _wayland_wl_registry_global(void *data, struct wl_registry *wregistry, uint32_t name, const char *interface, uint32_t version);
static void _wayland_wl_registry_global_remove(void *data, struct wl_registry *wregistry, uint32_t name);

static void _wayland_wl_seat_capabilities(void *data, struct wl_seat *wseat, uint32_t capabilities);
static void _wayland_wl_seat_name(void *data, struct wl_seat *wseat, const char *name);

static void _wayland_wp_presentation_clock_id(void *data, struct wp_presentation *wpresentation, uint32_t clk_id);

static void _wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xwbase, uint32_t serial);

static void _wayland_wl_surface_enter(void *data, struct wl_surface *wsurface, struct wl_output *wl_output);
static void _wayland_wl_surface_leave(void *data, struct wl_surface *wsurface, struct wl_output *wl_output);

static void _wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xtoplevel, int32_t width, int32_t height, struct wl_array *states);
static void _wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xtoplevel);
static void _wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xtoplevel, int32_t width, int32_t height);
static void _wayland_xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xtoplevel, struct wl_array *capabilities);

static void _wayland_wl_shm_format(void *data, struct wl_shm *wshm, uint32_t format);

static void _wayland_wl_pointer_enter(void *data, struct wl_pointer *wpointer, uint32_t serial, struct wl_surface *wsurface,
                                      wl_fixed_t xsurface, wl_fixed_t ysurface);
static void _wayland_wl_pointer_leave(void *data, struct wl_pointer *wpointer, uint32_t serial, struct wl_surface *wsurface);
static void _wayland_wl_pointer_motion(void *data, struct wl_pointer *wpointer, uint32_t time, wl_fixed_t xsurface, wl_fixed_t ysurface);
static void _wayland_wl_pointer_button(void *data, struct wl_pointer *wpointer, uint32_t serial, uint32_t time, uint32_t button,
                                       uint32_t state);
static void _wayland_wl_pointer_axis(void *data, struct wl_pointer *wpointer, uint32_t time, uint32_t axis, wl_fixed_t value);
static void _wayland_wl_pointer_frame(void *data, struct wl_pointer *wpointer);
static void _wayland_wl_pointer_axis_source(void *data, struct wl_pointer *wpointer, uint32_t axis_source);
static void _wayland_wl_pointer_axis_stop(void *data, struct wl_pointer *wpointer, uint32_t time, uint32_t axis);
static void _wayland_wl_pointer_axis_discrete(void *data, struct wl_pointer *wpointer, uint32_t axis, int32_t discrete);

static void _wayland_wl_keyboard_keymap(void *data, struct wl_keyboard *wkeyboard, uint32_t format, int32_t fd, uint32_t size);
static void _wayland_wl_keyboard_enter(void *data, struct wl_keyboard *wkeyboard, uint32_t serial, struct wl_surface *wsurface,
                                       struct wl_array *keys);
static void _wayland_wl_keyboard_leave(void *data, struct wl_keyboard *wkeyboard, uint32_t serial, struct wl_surface *wsurface);
static void _wayland_wl_keyboard_key(void *data, struct wl_keyboard *wkeyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
static void _wayland_wl_keyboard_modifiers(void *data, struct wl_keyboard *wkeyboard, uint32_t serial, uint32_t dmods, uint32_t lamods,
                                           uint32_t lomods, uint32_t group);
static void _wayland_wl_keyboard_repeat_info(void *data, struct wl_keyboard *wkeyboard, int32_t rate, int32_t delay);

static void _wayland_xdg_surface_configure(void *data, struct xdg_surface *xsurface, uint32_t serial);

static void *_wayland_create(const CreateConfig *cconfig)
{
     Wayland *wayland = (Wayland *)malloc(sizeof (Wayland));
     assert(wayland);
     wayland->wl_display = wl_display_connect(NULL);
     assert((*wayland)->wl_display);
     struct wl_registry *wregistry = wl_display_get_registry(wayland->wl_display);
     static struct wl_registry_listener wrlistener = {
          .global = _wayland_wl_registry_global,
          .global_remove = _wayland_wl_registry_global_remove
     };
     wl_registry_add_listener(wregistry, &wrlistener, wayland);
     wl_display_dispatch(wayland->wl_display);
     wl_display_roundtrip(wayland->wl_display);
     wayland->wl_surface = wl_compositor_create_surface(wayland->wl_compositor);
     static struct wl_surface_listener wslistener = {
          .enter = _wayland_wl_surface_enter,
          .leave = _wayland_wl_surface_leave
     };
     wl_surface_add_listener(wayland->wl_surface, &wslistener, wayland);
     struct xdg_surface *xsurface = xdg_wm_base_get_xdg_surface(wayland->xdg_wm_base, wayland->wl_surface);
     static struct xdg_surface_listener xslistener = {
          .configure = _wayland_xdg_surface_configure
     };
     xdg_surface_add_listener(xsurface, &xslistener, wayland);
     struct xdg_toplevel *xtoplevel = xdg_surface_get_toplevel(xsurface);
     static struct xdg_toplevel_listener xtlistener = {
          .configure = _wayland_xdg_toplevel_configure,
          .close = _wayland_xdg_toplevel_close,
          .configure_bounds = _wayland_xdg_toplevel_configure_bounds
     };
     xdg_toplevel_add_listener(xtoplevel, &xtlistener, wayland);
     xdg_toplevel_set_title(xtoplevel, "video-playback");
     xdg_toplevel_set_app_id(xtoplevel, "video-playback");
     wayland->wl_egl_window = wl_egl_window_create(wayland->wl_surface, cconfig->width, cconfig->height);
     return wayland;
}

static void
_wayland_destroy(void *backend)
{
     Wayland *wayland = (Wayland *)backend;
     // xdg_surface_destroy(wayland->xdg_surface);
     // xdg_toplevel_destroy(wayland->xdg_toplevel);
     wl_shm_destroy(wayland->wl_shm);
     // wl_buffer_destroy(wayland->wl_cursor_buffer);
     // wl_surface_destroy(wayland->wl_cursor_surface);
     // wl_cursor_theme_destroy(wayland->wl_cursor_theme);
     // wl_pointer_destroy(wayland->wl_pointer);
     // wl_keyboard_destroy(wayland->wl_keyboard);
     // wl_touch_destroy(wayland->wl_touch);
     wl_seat_destroy(wayland->wl_seat);
     // wl_compositor_destroy(wayland->wl_compositor);
     // wl_registry_destroy(wayland->wl_registry);
     wl_egl_window_destroy(wayland->wl_egl_window);
     wl_surface_destroy(wayland->wl_surface);
     wl_display_disconnect(wayland->wl_display);
     free(wayland);
}

static void
_wayland_wl_registry_global(void *data, struct wl_registry *wregistry, uint32_t name, const char *interface, uint32_t version)
{
     Wayland *wayland = (Wayland *)data;

     if (strcmp(interface, wl_compositor_interface.name) == 0) {
          wayland->wl_compositor = (struct wl_compositor *)wl_registry_bind(wregistry, name, &wl_compositor_interface, version);
     } else if (strcmp(interface, wl_shm_interface.name) == 0) {
          wayland->wl_shm = (struct wl_shm *)wl_registry_bind(wregistry, name, &wl_shm_interface, version);
          static struct wl_shm_listener wshlistener = {
               .format = _wayland_wl_shm_format
          };
          wl_shm_add_listener(wayland->wl_shm, &wshlistener, wayland);
     } else if (strcmp(interface, wl_seat_interface.name) == 0) {
          wayland->wl_seat = (struct wl_seat *)wl_registry_bind(wregistry, name, &wl_seat_interface, version);
          static struct wl_seat_listener wselistener = {
               .capabilities = _wayland_wl_seat_capabilities,
               .name = _wayland_wl_seat_name
          };
          wl_seat_add_listener(wayland->wl_seat, &wselistener, wayland);
     } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
          wayland->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(wregistry, name, &xdg_wm_base_interface, version);
          static struct xdg_wm_base_listener xwblistener = {
               .ping = _wayland_xdg_wm_base_ping
          };
          xdg_wm_base_add_listener(wayland->xdg_wm_base, &xwblistener, NULL);
     } else if (strcmp(interface, wp_viewporter_interface.name) == 0) {
          wayland->wp_viewporter = (struct wp_viewporter *)wl_registry_bind(wregistry, name, &wp_viewporter_interface, version);
     } else if (strcmp(interface, wp_presentation_interface.name) == 0) {
          wayland->wp_presentation = (struct wp_presentation *)wl_registry_bind(wregistry, name, &wp_presentation_interface, version);
          static struct wp_presentation_listener wplistener = {
               .clock_id = _wayland_wp_presentation_clock_id
          };
          wp_presentation_add_listener(wayland->wp_presentation, &wplistener, NULL);
     }
     // else if (strcmp(interface, wp_drm_interface.name) == 0) {

     // }

     printf("[wl_registry_global] interface: %s | name: %d\n", interface, name);
}

static void
_wayland_wl_registry_global_remove(void *data, struct wl_registry *wregistry,
     uint32_t name)
{
}

static void
_wayland_wl_seat_capabilities(void *data, struct wl_seat *wseat, uint32_t capabilities)
{
     Wayland *wayland = (Wayland *)data;
     if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
          struct wl_pointer *wpointer = wl_seat_get_pointer(wseat);
          static struct wl_pointer_listener wplistener = {
               .enter = _wayland_wl_pointer_enter,
               .leave = _wayland_wl_pointer_leave,
               .motion = _wayland_wl_pointer_motion,
               .button = _wayland_wl_pointer_button,
               .axis = _wayland_wl_pointer_axis,
               .frame = _wayland_wl_pointer_frame,
               .axis_source = _wayland_wl_pointer_axis_source,
               .axis_stop = _wayland_wl_pointer_axis_stop,
               .axis_discrete = _wayland_wl_pointer_axis_discrete
          };
          wl_pointer_add_listener(wpointer, &wplistener, wayland);
     }
     if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
          struct wl_keyboard *wkeyboard = wl_seat_get_keyboard(wseat);
          static struct wl_keyboard_listener wklistener = {
               .keymap = _wayland_wl_keyboard_keymap,
               .enter = _wayland_wl_keyboard_enter,
               .leave = _wayland_wl_keyboard_leave,
               .key = _wayland_wl_keyboard_key,
               .modifiers = _wayland_wl_keyboard_modifiers,
               .repeat_info = _wayland_wl_keyboard_repeat_info
          };
          wl_keyboard_add_listener(wkeyboard, &wklistener, NULL);
     }
}

static void
_wayland_wl_seat_name(void *data, struct wl_seat *wseat, const char *name)
{
}

static void
_wayland_wl_keyboard_keymap(void *data, struct wl_keyboard *wkeyboard, uint32_t format,
     int32_t fd, uint32_t size)
{
     Wayland *wayland = (Wayland *)data;
}

static void
_wayland_wl_keyboard_enter(void *data, struct wl_keyboard *wkeyboard, uint32_t serial,
     struct wl_surface *wsurface, struct wl_array *keys)
{
}

static void
_wayland_wl_keyboard_leave(void *data, struct wl_keyboard *wkeyboard, uint32_t serial,
     struct wl_surface *wsurface)
{
}

static void
_wayland_wl_keyboard_key(void *data, struct wl_keyboard *wkeyboard, uint32_t serial,
     uint32_t time, uint32_t key, uint32_t state)
{
}

static void
_wayland_wl_keyboard_modifiers(void *data, struct wl_keyboard *wkeyboard,
     uint32_t serial, uint32_t dmods, uint32_t lamods, uint32_t lomods, uint32_t group)
{
}

static void
_wayland_wl_keyboard_repeat_info(void *data, struct wl_keyboard *wkeyboard, int32_t rate,
     int32_t delay)
{ 
}

static void
_wayland_pointer_enter(void *data, struct wl_pointer *wpointer, uint32_t serial,
     struct wl_surface *wsurface, wl_fixed_t xsurface, wl_fixed_t ysurface)
{
     printf("[wl_pointer_enter]\n");
     // Wayland *wayland = (Wayland *)data;
     // wayland->wl_cursor_theme = wl_cursor_theme_load(NULL, 24, wayland->wl_shm);
     // struct wl_cursor *cursor = wl_cursor_theme_get_cursor(wayland->wl_cursor_theme,
     //      "left_ptr");
     // struct wl_buffer *buffer = wl_cursor_image_get_buffer(cursor->images[0]);
     // wayland->wl_surface_cursor = wl_compositor_create_surface(wayland->wl_compositor);
     // wl_pointer_set_cursor(wpointer, serial, wayland->wl_surface_cursor,
     //      cursor->images[0]->hotspot_x, cursor->images[0]->hotspot_y);
     // wl_surface_attach(wayland->wl_surface_cursor, buffer, 0, 0);
     // wl_surface_damage(wayland->wl_surface_cursor, 0, 0, cursor->images[0]->width,
     //      cursor->images[0]->height);
     // wl_surface_commit(wayland->wl_surface_cursor);
}

static void
_wayland_pointer_leave(void *data, struct wl_pointer *wpointer, uint32_t serial,
     struct wl_surface *wsurface)
{
     printf("[wl_pointer_leave]\n");
     Wayland *wayland = (Wayland *)data;
     // wl_surface_destroy(wayland->wl_surface_cursor);
     // wl_cursor_theme_destroy(wayland->wl_cursor_theme);
}

static void
_wayland_pointer_motion(void *data, struct wl_pointer *wpointer, uint32_t time,
     wl_fixed_t surface_x, wl_fixed_t surface_y)
{
 
}

static void
_wayland_pointer_button(void *data, struct wl_pointer *wpointer, uint32_t serial,
     uint32_t time, uint32_t button, uint32_t state)
{
 
}

static void
_wayland_pointer_axis(void *data, struct wl_pointer *wpointer, uint32_t time,
     uint32_t axis, wl_fixed_t value)
{
 
}

static void
_wayland_pointer_frame(void *data, struct wl_pointer *wpointer)
{
 
}

static void
_wayland_pointer_axis_source(void *data, struct wl_pointer *wpointer,
     uint32_t axis_source)
{
 
}

static void
_wayland_pointer_axis_stop(void *data, struct wl_pointer *wpointer, uint32_t time,
     uint32_t axis)
{
 
}

static void
_wayland_pointer_axis_discrete(void *data, struct wl_pointer *wpointer, uint32_t axis,
     int32_t discrete)
{
 
}

static void
_wayland_xdg_surface_configure(void *data, struct xdg_surface *xsurface,
     uint32_t serial)
{
     xdg_surface_ack_configure(xsurface, serial);
}

static void
_wayland_wl_shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
     Wayland *wayland = (Wayland *)data;
     printf("format %d\n", format);
}

static void
_wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xtoplevel,
     int32_t width, int32_t height, struct wl_array *states)
{
     Wayland *wayland = (Wayland *)data;
     printf("[xdg_toplevel_configure] width : %d, height : %d\n", width, height);
}

static void
_wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xtoplevel)
{
}

static void
_wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xtoplevel,
     int32_t width, int32_t height)
{
     Wayland *wayland = (Wayland *)data;
     printf("[xdg_toplevel_configure_bounds] width : %d, height : %d\n", width, height);
}

static void
_wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xwbase, uint32_t serial)
{
     xdg_wm_base_pong(xwbase, serial);
}

static void
_wayland_wl_surface_enter(void *data, struct wl_surface *wsurface,
     struct wl_output *wl_output)
{
     printf("wl_surface " );
}

static void
_wayland_wl_surface_leave(void *data, struct wl_surface *wsurface,
     struct wl_output *wl_output)
{
}

static void
_wayland_wp_presentation_clock_id(void *data, struct wp_presentation *wpresentation,
     uint32_t clk_id)
{
     printf("clk_id : %d\n", clk_id);
}

static void
_wayland_wl_pointer_enter(void *data, struct wl_pointer *wpointer, uint32_t serial,
     struct wl_surface *wsurface, wl_fixed_t surface_x,
     wl_fixed_t surface_y)
{}

static void
_wayland_wl_pointer_leave(void *data, struct wl_pointer *wpointer, uint32_t serial,
     struct wl_surface *wsurface)
{}

static void
_wayland_wl_pointer_motion(void *data, struct wl_pointer *wpointer, uint32_t time,
     wl_fixed_t surface_x, wl_fixed_t surface_y)
{}

static void
_wayland_wl_pointer_button(void *data, struct wl_pointer *wpointer, uint32_t serial,
     uint32_t time, uint32_t button, uint32_t state)
{}

static void
_wayland_wl_pointer_axis(void *data, struct wl_pointer *wpointer, uint32_t time,
     uint32_t axis, wl_fixed_t value)
{}

static void
_wayland_wl_pointer_frame(void *data, struct wl_pointer *wpointer)
{}

static void
_wayland_wl_pointer_axis_source(void *data, struct wl_pointer *wpointer,
     uint32_t axis_source)
{}

static void
_wayland_wl_pointer_axis_stop(void *data, struct wl_pointer *wpointer, uint32_t time,
     uint32_t axis)
{}

static void
_wayland_wl_pointer_axis_discrete(void *data, struct wl_pointer *wpointer,
     uint32_t axis, int32_t discrete)
{}
