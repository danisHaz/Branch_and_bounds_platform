#pragma once

#if (DEBUG == 1)
    #define PRINT(X) std::cout << (X) << "\n"
#else
    #define PRINT(X)
#endif
