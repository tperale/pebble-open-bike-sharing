#pragma once

// int (*max)(int, int) = lambda (int, (int x, int y) { return x > y ? x : y; });
#define lambda(return_type, function_body) \
  ({ \
    return_type __fn__ function_body \
    __fn__; \
  })
