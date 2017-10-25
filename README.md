# GLEQ — GLFW Event Queue

**GLEQ** is a basic, header-only event queue library for GLFW 3.  It adds GLFW
events for monitors, joysticks and windows to a single global queue.  Nothing
more.

GLEQ is inspired by [SDL](http://www.libsdl.org/) and
[GLWT](https://github.com/rikusalminen/glwt), and is written _as an example_ for
people requesting that GLFW provide an event queue API, to show how easy it is
to implement on top of GLFW callbacks.

GLEQ is written in C and depends only on GLFW 3.0 or later.

GLEQ is a work in progress and the interface may change but it works as intended
and covers nearly all events up to and including GLFW 3.3.  Just drop it into
your project and include it after the GLFW header.

GLEQ is licensed under the [zlib/libpng
license](https://opensource.org/licenses/Zlib).


## Using GLEQ

To use GLEQ, include `gleq.h` after the GLFW header.  To add the implementation
of GLEQ, define `GLEQ_IMPLEMENTATION` before including the GLEQ header in
*exactly one* source file.

```c
#include <GLFW/glfw3.h>

#define GLEQ_IMPLEMENTATION
#include "gleq.h"
```

If you will only be using GLEQ in a single source file, you can make _all_ its
functions static by defining `GLEQ_STATIC` as well.

```c
#include <GLFW/glfw3.h>

#define GLEQ_IMPLEMENTATION
#define GLEQ_STATIC
#include "gleq.h"
```

After GLFW has been successfully initialized, call `gleqInit`.  This will
replace the monitor and joystick callbacks.

```c
if (!glfwInit())
    return false;

gleqInit();
```

Once a GLFW window is created, you can track it with `gleqTrackWindow`.  This
replaces all callbacks on that window.

```c
gleqTrackWindow(window);
```

Event processing is done as usual with `glfwPollEvents`, `glfwWaitEvents` or
`glfwWaitEventsTimeout`.  Events for monitors, joysticks and tracked windows are
added to the queue as GLFW reports them.

Event retrieval is done with `gleqNextEvent` and each event is then freed with
`gleqFreeEvent`.

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

The call to `gleqFreeEvent` frees any memory allocated for the event and clears
the event struct.  Currently only the file drop event allocates memory, but it's
recommended to call it for every event once it has been processed.


## FAQ

### Does GLEQ use the GLFW window user pointer?

No, only GLFW callbacks.


### Does GLEQ allocate memory?

Only to save a deep copy of the path list provided to the file drop callback.
The event queue itself is a global static array.


### Aren't static arrays bad?

It depends.  Also, the size of the queue can be controlled with `GLEQ_CAPACITY`.


### Isn't global data bad?

It depends.  The native event queue wrapped by GLFW is global, too.


### Why doesn't GLEQ provide one queue per window?

GLEQ is intended to be a simple example event queue.  Having a queue per window
would make it more complicated than it needs to be.


### Why isn't GLEQ thread safe?

GLEQ is intended to be a simple example event queue.  Making it thread safe
would make it more complicated than it needs to be.

