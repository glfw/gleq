/*
 * Test program for GLEQ
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

#include <GLFW/glfw3.h>

#define GLEQ_IMPLEMENTATION
#include "gleq.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

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
        int i;
        GLEQevent event;

        glfwSwapBuffers(window);
        glfwWaitEvents();

        while (gleqNextEvent(&event))
        {
            switch (event.type)
            {
                case GLEQ_WINDOW_MOVED:
                    printf("Window moved to (%.0f %.0f)\n", event.pos.x, event.pos.y);
                    break;
                case GLEQ_WINDOW_RESIZED:
                    printf("Window resized to (%i %i)\n", event.size.width, event.size.height);
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
                case GLEQ_WINDOW_RESTORED:
                    printf("Window restored\n");
                    break;
                case GLEQ_FRAMEBUFFER_RESIZED:
                    printf("Framebuffer resized to (%i %i)\n", event.size.width, event.size.height);
                    break;
                case GLEQ_BUTTON_PRESSED:
                    printf("Button %i pressed\n", event.button.button);
                    break;
                case GLEQ_BUTTON_RELEASED:
                    printf("Button %i released\n", event.button.button);
                    break;
                case GLEQ_CURSOR_MOVED:
                    printf("Cursor moved to (%0.2f %0.2f)\n", event.pos.x, event.pos.y);
                    break;
                case GLEQ_CURSOR_ENTERED:
                    printf("Cursor entered window\n");
                    break;
                case GLEQ_CURSOR_LEFT:
                    printf("Cursor left window\n");
                    break;
                case GLEQ_SCROLLED:
                    printf("Scrolled (%0.2f %0.2f)\n", event.pos.x, event.pos.y);
                    break;
                case GLEQ_KEY_PRESSED:
                    printf("Key 0x%02x pressed\n", event.key.key);
                    break;
                case GLEQ_KEY_REPEATED:
                    printf("Key 0x%02x repeated\n", event.key.key);
                    break;
                case GLEQ_KEY_RELEASED:
                    printf("Key 0x%02x released\n", event.key.key);
                    break;
                case GLEQ_CHARACTER_INPUT:
                    printf("Character 0x%08x input\n", event.character.codepoint);
                    break;
                case GLEQ_FILE_DROPPED:
                    printf("%i files dropped\n", event.file.count);
                    for (i = 0;  i < event.file.count;  i++)
                        printf("\t%s\n", event.file.paths[i]);
                    break;
            }

            gleqFreeEvent(&event);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}

