#ifndef SHAPES_VISUALIZATION_HPP_
#define SHAPES_VISUALIZATION_HPP_

#include "core/mesh.hpp"

#include <memory>

class Visualization {
    public:
        static std::shared_ptr<Mesh> halfCube(float size);
        static std::shared_ptr<Mesh> halfCubeGrid(float size, unsigned int spaces);
        static std::shared_ptr<Mesh> halfCubeBorders(float size);
};

#endif  // SHAPES_VISUALIZATION_HPP_