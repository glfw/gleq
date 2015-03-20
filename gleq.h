/*
 * GLEQ - An event queue for GLFW 3
 * Copyright © Camilla Berglund <dreda@glfw.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#ifndef GLEQ_HEADER_FILE
#define GLEQ_HEADER_FILE

#if GLFW_VERSION_MAJOR != 3
    #error "This version of GLEQ only supports GLFW 3"
#endif

#if GLFW_VERSION_MINOR < 1
    #error "This version of GLEQ requires GLFW 3.1 or later"
#endif

#if GLFW_VERSION_MINOR != 1
    #warning "This version of GLEQ does not support events added after GLFW 3.1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GLEQ_NONE,
    GLEQ_WINDOW_MOVED,
    GLEQ_WINDOW_RESIZED,
    GLEQ_WINDOW_CLOSED,
    GLEQ_WINDOW_REFRESH,
    GLEQ_WINDOW_FOCUSED,
    GLEQ_WINDOW_DEFOCUSED,
    GLEQ_WINDOW_ICONIFIED,
    GLEQ_WINDOW_RESTORED,
    GLEQ_FRAMEBUFFER_RESIZED,
    GLEQ_BUTTON_PRESSED,
    GLEQ_BUTTON_RELEASED,
    GLEQ_CURSOR_MOVED,
    GLEQ_CURSOR_ENTERED,
    GLEQ_CURSOR_LEFT,
    GLEQ_SCROLLED,
    GLEQ_KEY_PRESSED,
    GLEQ_KEY_REPEATED,
    GLEQ_KEY_RELEASED,
    GLEQ_CHARACTER_INPUT,
    GLEQ_FILE_DROPPED

} GLEQtype;

typedef struct
{
    GLEQtype type;
    GLFWwindow* window;
    union {
        struct {
            double x;
            double y;
        } pos;
        struct {
            int width;
            int height;
        } size;
        struct {
            int key;
            int scancode;
            int mods;
        } key;
        struct {
            int button;
            int mods;
        } button;
        struct {
            unsigned int codepoint;
            int mods;
        } character;
        struct {
            char** paths;
            int count;
        } file;
    };

} GLEQevent;

extern void gleqTrackWindow(GLFWwindow* window);

extern int gleqNextEvent(GLEQevent* event);
extern void gleqFreeEvent(GLEQevent* event);

#ifdef __cplusplus
}
#endif

#ifdef GLEQ_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef GLEQ_CAPACITY
    #define GLEQ_CAPACITY 1024
#endif

static struct
{
    GLEQevent events[GLEQ_CAPACITY];
    size_t head;
    size_t tail;
} gleq = { {}, 0, 0 };

static GLEQevent* gleqNewEvent(void)
{
    GLEQevent* event = gleq.events + gleq.head;
    gleq.head = (gleq.head + 1) % GLEQ_CAPACITY;
    assert(gleq.head != gleq.tail);
    memset(event, 0, sizeof(GLEQevent));
    return event;
}

static void gleqWindowPosCallback(GLFWwindow* window, int x, int y)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_WINDOW_MOVED;
    event->window = window;
    event->pos.x = (double) x;
    event->pos.y = (double) y;
}

static void gleqWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_WINDOW_RESIZED;
    event->window = window;
    event->size.width = width;
    event->size.height = height;
}

static void gleqWindowCloseCallback(GLFWwindow* window)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_WINDOW_CLOSED;
    event->window = window;
}

static void gleqWindowRefreshCallback(GLFWwindow* window)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_WINDOW_REFRESH;
    event->window = window;
}

static void gleqWindowFocusCallback(GLFWwindow* window, int focused)
{
    GLEQevent* event = gleqNewEvent();
    event->window = window;

    if (focused)
        event->type = GLEQ_WINDOW_FOCUSED;
    else
        event->type = GLEQ_WINDOW_DEFOCUSED;
}

static void gleqWindowIconifyCallback(GLFWwindow* window, int iconified)
{
    GLEQevent* event = gleqNewEvent();
    event->window = window;

    if (iconified)
        event->type = GLEQ_WINDOW_ICONIFIED;
    else
        event->type = GLEQ_WINDOW_RESTORED;
}

static void gleqFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_FRAMEBUFFER_RESIZED;
    event->window = window;
    event->size.width = width;
    event->size.height = height;
}

static void gleqMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLEQevent* event = gleqNewEvent();
    event->window = window;
    event->button.button = button;
    event->button.mods = mods;

    if (action == GLFW_PRESS)
        event->type = GLEQ_BUTTON_PRESSED;
    else if (action == GLFW_RELEASE)
        event->type = GLEQ_BUTTON_RELEASED;
}

static void gleqCursorPosCallback(GLFWwindow* window, double x, double y)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_CURSOR_MOVED;
    event->window = window;
    event->pos.x = x;
    event->pos.y = y;
}

static void gleqCursorEnterCallback(GLFWwindow* window, int entered)
{
    GLEQevent* event = gleqNewEvent();
    event->window = window;

    if (entered)
        event->type = GLEQ_CURSOR_ENTERED;
    else
        event->type = GLEQ_CURSOR_LEFT;
}

static void gleqScrollCallback(GLFWwindow* window, double x, double y)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_SCROLLED;
    event->window = window;
    event->pos.x = x;
    event->pos.y = y;
}

static void gleqKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLEQevent* event = gleqNewEvent();
    event->window = window;
    event->key.key = key;
    event->key.scancode = scancode;
    event->key.mods = mods;

    if (action == GLFW_PRESS)
        event->type = GLEQ_KEY_PRESSED;
    else if (action == GLFW_RELEASE)
        event->type = GLEQ_KEY_RELEASED;
    else if (action == GLFW_REPEAT)
        event->type = GLEQ_KEY_REPEATED;
}

static void gleqCharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_CHARACTER_INPUT;
    event->window = window;
    event->character.codepoint = codepoint;
    event->character.mods = mods;
}

static void gleqFileDropCallback(GLFWwindow* window, int count, const char** paths)
{
    GLEQevent* event = gleqNewEvent();
    event->type = GLEQ_FILE_DROPPED;
    event->window = window;
    event->file.paths = malloc(count * sizeof(char*));
    event->file.count = count;

    while (count--)
        event->file.paths[count] = strdup(paths[count]);
}

void gleqTrackWindow(GLFWwindow* window)
{
    glfwSetWindowPosCallback(window, gleqWindowPosCallback);
    glfwSetWindowSizeCallback(window, gleqWindowSizeCallback);
    glfwSetWindowCloseCallback(window, gleqWindowCloseCallback);
    glfwSetWindowRefreshCallback(window, gleqWindowRefreshCallback);
    glfwSetWindowFocusCallback(window, gleqWindowFocusCallback);
    glfwSetWindowIconifyCallback(window, gleqWindowIconifyCallback);
    glfwSetFramebufferSizeCallback(window, gleqFramebufferSizeCallback);
    glfwSetMouseButtonCallback(window, gleqMouseButtonCallback);
    glfwSetCursorPosCallback(window, gleqCursorPosCallback);
    glfwSetCursorEnterCallback(window, gleqCursorEnterCallback);
    glfwSetScrollCallback(window, gleqScrollCallback);
    glfwSetKeyCallback(window, gleqKeyCallback);
    glfwSetCharModsCallback(window, gleqCharModsCallback);
    glfwSetDropCallback(window, gleqFileDropCallback);
}

int gleqNextEvent(GLEQevent* event)
{
    memset(event, 0, sizeof(GLEQevent));

    if (gleq.head != gleq.tail)
    {
        *event = gleq.events[gleq.tail];
        gleq.tail = (gleq.tail + 1) % GLEQ_CAPACITY;
    }

    return event->type != GLEQ_NONE;
}

void gleqFreeEvent(GLEQevent* event)
{
    if (event->type == GLEQ_FILE_DROPPED)
    {
        while (event->file.count--)
            free(event->file.paths[event->file.count]);

        free(event->file.paths);
    }

    memset(event, 0, sizeof(GLEQevent));
}

#endif /* GLEQ_IMPLEMENTATION */

#endif /* GLEQ_HEADER_FILE */

