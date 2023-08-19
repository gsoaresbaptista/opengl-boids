#ifndef CORE_PRIMITIVES_HPP_
#define CORE_PRIMITIVES_HPP_

#include "glad/glad.h"

#include <vector>

class Primitives {
    public:
        static std::vector<float> circle(float radius, unsigned int points);
};

#endif  // CORE_PRIMITIVES_HPP_
