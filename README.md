# mcpelauncher-shadersmod

This is a mod for mcpelauncher-manifest that allows for RenderDragon shaders.

Usage: 

Put your shaders in a folder called shaders in the data root folder. RenderDragon shaders have an extension of `.material.bin`.  

NewbX Future Shaders running with the mod on:
  ![NewbX Future](https://github.com/user-attachments/assets/586c472a-5d4d-4807-a7ad-bd50f324206c)


## Building
`PATH_TO_NDK="/path/to/ndk"`
- x86

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86 ..`
- x86_64

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 ..`
- armeabi-v7a

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a ..`
- arm64-v8a

  `cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a ..`
