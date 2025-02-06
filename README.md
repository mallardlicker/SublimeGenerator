Sublime Text Project Generator
====================================================================
Justin Bunting

Creates a brand new project given a (project-name, dir) pair. Uses the sublime text editor's project format, along with the cmake build system, in order to create a repeatable and pre-built environment for future projects.

Specifically, all of the resources found in the ./SublimeGenerator/assets folder are copied into the specified directory, and every instance of either sgpath or sgname is replaced with the inputted values.