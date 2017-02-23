#include "Pass.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <iostream>

Pass::Pass(const std::string &path)
{
     void *handle;
     char *error;

     handle = dlopen (path.c_str(), RTLD_LAZY);
     if (!handle) {
         std::cerr << "ERROR: " << dlerror() << std::endl;
         throw PassLoadingError();
     }

     checkCall = reinterpret_cast<plugin_callback>(dlsym(handle, "check"));
     if ((error = dlerror()) != NULL)  {
         std::cerr << "ERROR: " << dlerror() << std::endl;
         throw PassLoadingError();
     }

     transformCall = reinterpret_cast<plugin_callback>(dlsym(handle, "transform"));
     if ((error = dlerror()) != NULL)  {
         std::cerr << "ERROR: " << dlerror() << std::endl;
         throw PassLoadingError();
     }

     dlclose(handle);
}

bool Pass::check(const std::string& path) {
    return (*checkCall)(path.c_str());
}

bool Pass::transform(const std::string& path) {
    return (*transformCall)(path.c_str());
}
