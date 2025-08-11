# CS 381 - Grass Racer: Turf Wars 3.0
- Author: Z He
- Date: April 13th, 2025

## Objective
Stay on the grass as all cars' speed relentlessly climbs. The longer you hold on, the higher your score—but there’s a catch. You get only ONE shot at setting the highest score. No second chances. Think you can handle the pressure? Good luck!

## Controls 
- `W`: Smoothly increases the selected entity’s velocity (speed) in the direction it is facing.
- `S`: Smoothly decreases the selected entity’s velocity (speed) in the direction it is facing.
- `A`: Smoothly increases the selected entity’s heading (turns left).
- `D`: Smoothly decreases the selected entity’s heading (turns right).
- `Tab`: Cycles through the entities, selecting the next one.

## Fetching Git Submodules

### Cloning a Repository with Submodules
```bash
git clone --recursive --depth=1 https://github.com/zhaohenghe/CS381
git submodule update --init --recursive  
```
- This downloads the submodule's content into the directory.
- ```--init``` ensure that any submodules that haven't been initialized are initialized.
- ```--recursive``` ensures that if the submodule itself has submodules, they are also fetched.

- This clones the repository and initializes all submodules automatically.

### `[OPTIONAL]` Add the Submodule if There Are Issues with ```raylib-cpp```
```bash
git submodule add https://github.com/joshuadahlunr/raylib-cpp.git
git submodule update --init --recursive  
```
- This command adds the ```raylib-cpp``` repository as a submodule.
- `git submodule update --init --recursive` ensures the submodule is initialized and fully updated.

## Configuration Change
To enable support for JPG file format in `raylib`, you need to uncomment a specific line in the configuration file. Follow these steps:
1. Open the file `../raylib-cpp/raylib/src/config.h` in a text editor.
2. Find the lines that contain `#define SUPPORT_FILEFORMAT_JPG 1`.
3. Uncomment this line by removing the `//` at the beginning of the line.

The line should look like this after uncommenting:
```c
#define SUPPORT_FILEFORMAT_JPG      1
```

## Compiling the Code 
While inside the `turfwars` directory, run the following commands:
- Just in case `.gitignore` isn't set up properly, run the following command:
```bash
rm -rf build
```
- Continue running the following commands: 
```bash
mkdir build
cd build
cmake ..
make  
```

## Running the Code  
While inside the `build` directory, run the following command:
```bash
./turfwars
```  

## Extra Credit
-
- 
