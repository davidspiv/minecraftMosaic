This is my most useful header—under 60 lines! It’s a simple program timer that works in scopes and loops. Use it like this:
```
#include "../include/Timer.h"

int main() {
  for (size_t i = 0; i < 100; i++) {
    for (size_t j = 0; j < 100; j++) {

      Timer timer("label_A");
      /* do_something_A */

    }
  }

  for (size_t i = 0; i < 100; i++) {

    Timer timer("label_B");
    /* do_something_B */

  }

  Timer::printData();
}
```

It automatically sums all durations with the same label. Instead of printing thousands of lines, it outputs a summary like:
```
---------------------
label_B:  6167.068 ms
label_A:   989.974 ms
---------------------
```

Note: A timer must go out of scope before its time is recorded. So create timers at the start of loops or functions, and call `Timer::printData()` at the end of main for best results.
