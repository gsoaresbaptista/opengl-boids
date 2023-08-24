# Boids Simulation with OpenGL

A simple Boids simulation implemented in OpenGL, using GLFW and ImGui for configuration.
<p align="center">
  <img src="https://github.com/gsoaresbaptista/opengl-boids/blob/main/.github/simulation.gif" />
<!-- ![Boids Simulation](https://github.com/gsoaresbaptista/opengl-boids/blob/main/.github/simulation.gif) -->
</p>


## Features

- Real-time simulation of flocking behavior using the Boids algorithm.
- Interactive configuration using ImGui for adjusting parameters.
- Rendering of individual Boids as simple shapes.
- **TODO: Move simulation calculations to shaders to improve performance.**
- **TODO: Refactor the application into an Object-Oriented Programming (OOP) structure.**
- **TODO: Implement a cube that can be moved for the Boids to follow.**
- **TODO: Implement obstacle avoidance.**

## Getting Started

### Prerequisites

- G++ compiler
- Modern OpenGL
- GLFW3

### Installation

1. Clone this repository:

```shell
git clone https://github.com/gsoaresbaptista/opengl-boids
cd opengl-boids
```

2. Compile the code using the provided Makefile:
```shell
make
```

3. Run the simulation:
```shell
./boids
```

## Usage

1.  Launch the simulation executable.
2.  Use the ImGui interface to adjust parameters like flocking behavior, boid perception radius, etc.

## License

This project is licensed under the [MIT License](https://opensource.org/license/mit/).

## Credits

- The Boids algorithm was developed by Craig Reynolds. More details can be found [here](http://www.red3d.com/cwr/boids/).
- Additional resources on Boids:
  - [3 Simple Rules of Flocking Behaviors: Alignment, Cohesion, and Separation](https://gamedevelopment.tutsplus.com/3-simple-rules-of-flocking-behaviors-alignment-cohesion-and-separation--gamedev-3444t)
  - [Simulating Flocking with the Boids Algorithm](https://medium.com/fragmentblog/simulating-flocking-with-the-boids-algorithm-92aef51b9e00)
