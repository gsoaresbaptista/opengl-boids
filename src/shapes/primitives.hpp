#ifndef CORE_PRIMITIVES_HPP_
#define CORE_PRIMITIVES_HPP_

#include "glad/glad.h"

#include "core/mesh.hpp"

#include <vector>
#include <memory>

class Primitives {
    public:
        static std::shared_ptr<Mesh> circle(float radius, unsigned int points);
        static std::shared_ptr<Mesh> grid(float size, unsigned int spaces);
};

#endif  // CORE_PRIMITIVES_HPP_
