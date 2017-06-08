# Tomasulo
Tomasulo algorithm simulator based on QT C++

## Algorithm
The [tomasulo algorithm](https://en.wikipedia.org/wiki/Tomasulo_algorithm) is a simple dynamic scheduling algorithm developed by [Robert Tomasulo](https://en.wikipedia.org/wiki/Robert_Tomasulo) in 1967. Its details can be found in many classical computer architecture text books.

The algorithm is implemented in the *model* layer of this software, which can be found in the /Model directory. It can compile and run as an independent model.

## Design
The application follows a MVP / MVVM design pattern. The view layer is implemented with Qt Widgets, and wired with the model layer, i.e. the algorithm, with a viewmodel layer.
