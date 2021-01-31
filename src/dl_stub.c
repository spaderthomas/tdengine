// We're statically linking Lua, but to load C modules it uses ldl. But linking ldl statically is bad (from a semantic standpoint + I believe that it requires the same version of libc that it was compiled with to be available when loaded).

// In any case, we don't care if we can load C modules into Lua, because that's not how we're doing things. So just mock up ldl here so we can compile.

#include <dlfcn.h> // This declares everything as extern C (important)
#include <link.h>
#include <stdlib.h>

void* dlopen(const char*, int) {
  return nullptr;
}
char* dlerror() {
  return nullptr;
}
void* dlsym(void*, const char*) {
  return nullptr;
}
void* dlvsym(void*, const char*, const char*) {
  return nullptr;
}
int dladdr(const void*, Dl_info*) {
  return 0;
}
int dlclose(void*) {
  return -1;
}
