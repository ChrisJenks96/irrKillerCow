# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( fmod
        SHARED
        IMPORTED )

set_target_properties( fmod
        PROPERTIES IMPORTED_LOCATION
        C:/sdk/fmodstudioapi20011android/api/core/lib/${ANDROID_ABI}/libfmod.a)

add_library( fmodL
        SHARED
        IMPORTED )

set_target_properties( fmodL
        PROPERTIES IMPORTED_LOCATION
        C:/sdk/fmodstudioapi20011android/api/core/lib/${ANDROID_ABI}/libfmodL.a )

add_library( # Sets the name of the library.
        app-glue

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        # Associated headers in the same location as their source
        # file are automatically included.
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c )

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             main.cpp
             Enemy.cpp
             Player.cpp
             BigEnemy.cpp
             EnemyOrb.cpp
             ER.h
             android_tools.cpp
        )

# Specifies a path to native header files.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${ANDROID_NDK}/sources/android/native_app_glue/)
include_directories(C:/sdk/fmodstudioapi20011android/api/core/inc/)
include_directories(C:/sdk/irrlicht-ogl-es/include/)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( native-lib
                       app-glue
                       android
                       EGL
                       log
                       GLESv1_CM
                       GLESv2
                       z
                       m
                       ${log-lib}
                        C:/sdk/irrlicht-ogl-es/source/Irrlicht/Android/obj/local/${ANDROID_ABI}/libIrrlicht.a)
#>>>    NOTE: Just like above, this should be your path. This one is mine.
#>>>          Links to the appropriate ABI. CAREFUL: You must have >ALL< Irrlicht ABIs built ("arm64-v8a", "armeabi", "armeabi-v7a", "mips64", "x86", "x86_64") otherwise you may run into installation errors on your virtual device.