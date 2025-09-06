# command-processor
![Demo](docs/command-processor.gif)
This is a simple CPP program that provides a Command Line Interface. 
It is created to easily be used with other C++ applications that want to use an interactive Command Line Interface.    


## NOTE
This was built and tested with UNIX terminal. So windows default terminal (command prompt or powershell) may not work. It should work with Windows Subsystem for Linux(WSL) 

## How to use in your projects
- Clone this repository to your project folder
- If using cmake, add the following to your `CMakeLists.txt` file
```
include(FetchContent)

FetchContent_Declare(
  commandprocessor
  GIT_REPOSITORY https://github.com/Ose-4g/command-processor.git
  GIT_TAG v1.2.1
)

FetchContent_MakeAvailable(commandprocessor)
add_executable(YOUR_APP_EXECUTABLE_NAME ${CPP_FILES})
target_link_libraries(YOUR_APP_EXECUTABLE_NAME commandprocessor)
```
- include `command-processor.h` and get access to all the functions. 


## Testing
To run the unit tests in this project. Run 
```
./run.sh t
```
## Documentation
[Read docs here](docs/docs.md)

