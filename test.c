/*
 * Test program for GLEQ
 * Copyright © Camilla Löwy <elmindreda@glfw.org>
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

#include <GLFW/glfw3.h>

#define GLEQ_IMPLEMENTATION
#include "gleq.h"

#include <stdio.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    gleqInit();

    GLFWwindow* window = glfwCreateWindow(640, 480, "Event Queue Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    gleqTrackWindow(window);

    while (!glfwWindowShouldClose(window))
    {
        GLEQevent event;

        glfwSwapBuffers(window);
        glfwWaitEvents();

        while (gleqNextEvent(&event))
        {
            switch (event.type)
            {
                case GLEQ_WINDOW_MOVED:
                    printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
                    break;

                case GLEQ_WINDOW_RESIZED:
                    printf("Window resized to %ix%i\n",
                           event.size.width,
                           event.size.height);
                    break;

                case GLEQ_WINDOW_CLOSED:
                    printf("Window close request\n");
                    break;

                case GLEQ_WINDOW_REFRESH:
                    printf("Window refresh request\n");
                    break;

                case GLEQ_WINDOW_FOCUSED:
                    printf("Window focused\n");
                    break;

                case GLEQ_WINDOW_DEFOCUSED:
                    printf("Window defocused\n");
                    break;

                case GLEQ_WINDOW_ICONIFIED:
                    printf("Window iconified\n");
                    break;

                case GLEQ_WINDOW_UNICONIFIED:
                    printf("Window uniconified\n");
                    break;

#if GLFW_VERSION_MINOR >= 3
                case GLEQ_WINDOW_MAXIMIZED:
                    printf("Window maximized\n");
                    break;

                case GLEQ_WINDOW_UNMAXIMIZED:
                    printf("Window unmaximized\n");
                    break;

                case GLEQ_WINDOW_SCALE_CHANGED:
                    printf("Window content scale %0.2fx%0.2f\n",
                           event.scale.x, event.scale.y);
                    break;
#endif // GLFW_VERSION_MINOR

                case GLEQ_FRAMEBUFFER_RESIZED:
                    printf("Framebuffer resized to %ix%i\n",
                           event.size.width,
                           event.size.height);
                    break;

                case GLEQ_BUTTON_PRESSED:
                    printf("Mouse button %i pressed (mods 0x%x)\n",
                           event.mouse.button,
                           event.mouse.mods);
                    break;

                case GLEQ_BUTTON_RELEASED:
                    printf("Mouse button %i released (mods 0x%x)\n",
                           event.mouse.button,
                           event.mouse.mods);
                    break;

                case GLEQ_CURSOR_MOVED:
                    printf("Cursor moved to %i,%i\n", event.pos.x, event.pos.y);
                    break;

                case GLEQ_CURSOR_ENTERED:
                    printf("Cursor entered window\n");
                    break;

                case GLEQ_CURSOR_LEFT:
                    printf("Cursor left window\n");
                    break;

                case GLEQ_SCROLLED:
                    printf("Scrolled %0.2f,%0.2f\n",
                           event.scroll.x, event.scroll.y);
                    break;

                case GLEQ_KEY_PRESSED:
                    printf("Key 0x%02x pressed (scancode 0x%x mods 0x%x)\n",
                           event.keyboard.key,
                           event.keyboard.scancode,
                           event.keyboard.mods);
                    break;

                case GLEQ_KEY_REPEATED:
                    printf("Key 0x%02x repeated (scancode 0x%x mods 0x%x)\n",
                           event.keyboard.key,
                           event.keyboard.scancode,
                           event.keyboard.mods);
                    break;

                case GLEQ_KEY_RELEASED:
                    printf("Key 0x%02x released (scancode 0x%x mods 0x%x)\n",
                           event.keyboard.key,
                           event.keyboard.scancode,
                           event.keyboard.mods);
                    break;

                case GLEQ_CODEPOINT_INPUT:
                    printf("Codepoint U+%05X input\n", event.codepoint);
                    break;

#if GLFW_VERSION_MINOR >= 1
                case GLEQ_FILE_DROPPED:
                {
                    int i;

                    printf("%i files dropped\n", event.file.count);
                    for (i = 0;  i < event.file.count;  i++)
                        printf("\t%s\n", event.file.paths[i]);

                    break;
                }
#endif // GLFW_VERSION_MINOR

                case GLEQ_MONITOR_CONNECTED:
                    printf("Monitor \"%s\" connected\n",
                           glfwGetMonitorName(event.monitor));
                    break;

                case GLEQ_MONITOR_DISCONNECTED:
                    printf("Monitor \"%s\" disconnected\n",
                           glfwGetMonitorName(event.monitor));
                    break;

#if GLFW_VERSION_MINOR >= 2
                case GLEQ_JOYSTICK_CONNECTED:
                    printf("Joystick %i \"%s\" connected\n",
                           event.joystick,
                           glfwGetJoystickName(event.joystick));
                    break;

                case GLEQ_JOYSTICK_DISCONNECTED:
                    printf("Joystick %i disconnected\n", event.joystick);
                    break;
#endif // GLFW_VERSION_MINOR

                default:
                    fprintf(stderr, "Error: Unknown event %i\n", event.type);
                    break;
            }

            gleqFreeEvent(&event);
        }

        // Workaround for msvcrt stdout not being properly flushed by
        // newlines when running inside mintty
        fflush(stdout);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}

