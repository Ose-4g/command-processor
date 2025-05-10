# command-processor
This is a simple CPP program that provides a Command Line Interface. 
It is created to easily be used with other C++ applications that want to use an interactive Command Line Interface. 

![image](docs/Screenshot%202025-05-09%20at%2018.31.06.png)

## How to use in your projects
- Clone this repository to your project folder
- If using cmake, add this repsitory using `add_subdirectory` in your `CMakeLists.txt` file
- include `command-processor.h` and get access to all the functions. 

## Building this project. 
If you're building this project as a standalone project. 
- run 
```bash
./run.sh
```
in your terminal

## Testing
To run the unit tests in this project. Run 
```
./run.sh t
```