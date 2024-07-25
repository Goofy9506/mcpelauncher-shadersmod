# MCPELauncher Shader Mod

This is a mod for [mcpelauncher-manifest](https://mcpelauncher.readthedocs.io/en/latest/getting_started/index.html) that allows loading RenderDragon shaders.

## Usage

‌1. Download .so file for your cpu architecture from [releases](https://github.com/goofy9506/mcpelauncher-shadersmod/releases/latest).<br><br>

![image](https://github.com/user-attachments/assets/e2cb4255-8339-48b5-9ff2-c366448a69c7)<br> 2. Open data root from here.<br><br>

![image](https://github.com/user-attachments/assets/84da268f-561a-4667-88e1-b265a7c99ce6)<br> 3. Make a folder named `mods` and place the extracted .so file there.<br><br>

<hr>

NewbX Future Shaders running with the mod installed:
  ![NewbX Future](https://github.com/user-attachments/assets/586c472a-5d4d-4807-a7ad-bd50f324206c)

## Building

Prefered Android NDK version: r21e

`PATH_TO_NDK="/path/to/ndk"`

- **x86**

  ```
  cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/x64/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86 "-DCMAKE_CXX_FLAGS=-Wl,--strip-all -fvisibility=hidden"
  cmake --build ./build --config Release --target mcpelaunchershadersmod
  ```

- **x86_64**

  ```
  cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/x64/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 "-DCMAKE_CXX_FLAGS=-Wl,--strip-all -fvisibility=hidden"
  cmake --build ./build --config Release --target mcpelaunchershadersmod
  ```

- **armeabi-v7a**

  ```
  cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/x64/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a "-DCMAKE_CXX_FLAGS=-Wl,--strip-all -fvisibility=hidden"
  cmake --build ./build --config Release --target mcpelaunchershadersmod
  ```

- **arm64-v8a**

  ```
  cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/x64/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a "-DCMAKE_CXX_FLAGS=-Wl,--strip-all -fvisibility=hidden"
  cmake --build ./build --config Release --target mcpelaunchershadersmod
  ```
