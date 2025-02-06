Sublime Text Project Generator
====================================================================
Justin Bunting

Creates a brand new project given a project name and type. Uses the sublime text editor's project format, along with the cmake build system, in order to create a repeatable and pre-built environment for future projects.

Specifically, all of the resources for templates found in the ./SublimeGenerator/assets folder are copied into the specified directory, and every instance of variables such as sgPath or sgName are replaced with the inputted values. Certain files are ommitted from the GitHub repo, most importantly the .sublime-project files contained in each of the project type templates, which allow projects to have custom CMake generation and build/run actions.

Utilizes custom CppUtils library, which in turn utilizes NFD ( https://github.com/mlabbe/nativefiledialog ).