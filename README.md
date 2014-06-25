# GLEQ ̣̣— GLFW Event Queue

**GLEQ** is a simple, header-only event queue for GLFW 3.  It adds GLFW events
for tracked windows to a single global queue.  That's all.  No magic.

GLEQ is inspired by SDL and GLWT, and is written *as an example* for people
requesting that GLFW provide an event queue API.

GLEQ is written in C89 and depends only on GLFW 3.1 or later.

GLEQ is a work in progress but works just fine.


## Using GLEQ

To use GLEQ, include `gleq.h` after the GLFW header.  To add the implementation
of GLEQ, define `GLEQ_IMPLEMENTATION` before including the GLEQ header in
*exactly one* source file.

Once a GLFW window is created, you can track it with `gleqTrackWindow`.  This
replaces all callback on that window.

```c
gleqTrackWindow(window);
```

Event polling and waiting is done as usual with `glfwPollEvents` and
`glfwWaitEvents`.  Available events for tracked windows are added to the queue
when you call one of these.

Event retrieval is done with `gleqNextEvent`.

```c
GLEQevent event;

while (gleqNextEvent(&event))
{
    switch (event.type)
    {
        case GLEQ_WINDOW_RESIZED:
            printf("Window resized to %ix%i\n",
                   event.size.width, event.size.height);
            break;
    }

    gleqFreeEvent(&event);
}
```

