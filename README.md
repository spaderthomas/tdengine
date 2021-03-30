# Installation

## Windows

### 1: Install Visual Studio 2019'

Install the [community edition of Visual Studio 2019](https://visualstudio.microsoft.com/downloads/). When the installer prompts you to choose what components to install, make sure that "Desktop development with C++" is checked.

## 2: Install Python

If you don't have Python, install the latest stable release from [here](https://www.python.org/downloads/windows/) by selecting the latest version, scrolling to the bottom, and downloading + running the 64-bit installer.

In the installer:
- Make sure the option to install Pip is selected
- Make sure "Add Python to environment variables" is selected

## 3: Reboot

Adding things to your path can be finicky -- it's easiest to reboot to make sure that any command prompts you open have the updated path.

## 4: Install tdbuild

This project uses my simple build tool, [tdbuild](https://github.com/spaderthomas/tdbuild). You need to install tdbuild using Python's package manager, Pip. It's as easy as `pip install tdbuild` from any command prompt.

## 5: Build

To build this project, you need to open a Visual Studio command prompt. This command prompt has the C++ linker, compiler, etc. already inserted into the path. If you try to build in a regular command prompt, you will not be able to find the build tools, and it will not work.

To open a VS command prompt, press the Windows key and search for "x64 Native Tools Command Prompt for VS 2019". Selecting it should open a prompt. You'll use this prompt to build.

Change directories into the root of the project, and run `tdbuild setup`. This will generate a few machine-specific files you need to build the project. Then, run `tdbuild` to build the project. You'll get some messages from the build tool. Once it builds, run `tdbuild run` to run the engine.