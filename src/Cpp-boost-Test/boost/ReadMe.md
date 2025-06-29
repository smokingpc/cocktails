
# Build boost

## Build Boost 1_65_0 for MacOS using command line MAC tools:

* Download boost_1_65_0.tar.bz2.
http://sourceforge.net/projects/boost/files/boost/1.65.0/boost_1_65_0.tar.bz2/download

* In the directory where you want to put the Boost installation, execute

* $tar --bzip2 -xf <download path>/boost_1_65_0.tar.bz2

* $ Issue following commands in the shell

* $ cd <path_to_boost_1_65_0>
* $ ./bootstrap.sh
* $ ./b2 threading=multi link=static toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --prefix=<boost install path> install

* This will generate boost static libraries (.a) statically linked with libc with multi-thread support to be used in Xcode 4.6.

For other general build instructions refer:
http://www.boost.org/doc/libs/1_65_0/more/getting_started/unix-variants.html

## M1 Universal build
* Boost version boost_1_78_0, download https://www.boost.org/users/history/version_1_78_0.html
* boost_1_78_0 supports XCode 12. That's why boost is upgraded
* download and extract the boost source code zip file
* M1 Build  steps
* ./bootstrap.sh
* ./b2 threading=multi link=static toolset=clang cxxflags="-stdlib=libc++ -arch arm64" linkflags="-stdlib=libc++" --prefix=../arm64 install
* x86 build
*  ./bootstrap.sh
*  ./b2 threading=multi link=static toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --prefix=../x8664 install
* Universal libs: Once all the libs are cerated, use following code to build universal libs
```
for lib in arm64/lib/*; do
  lipo -create -arch arm64 $lib -arch x86_64 lib/$(basename $lib) -output universal/lib/$(basename $lib);
done
for lib in universal/lib*; do
  lipo $lib -info;
done
```
* Please note, due to unkonw reason boost could not be built for x86 on M1.It was built on x86 machine.



## Build Boost for Windows 32/64 bit using VS 2015

*  download boost_1_65_0.7z or boost_1_65_0.zip.
http://www.boost.org/users/history/version_1_65_0.html

* Extract zip to a folder boost_1_65_0.

* From the VS 2015 developer command prompt issue following commands:

> cd <path_to_boost_1_65_0>
> ./bootstrap.bat
> b2 toolset=msvc-14.0 variant=<debug,release> threading=multi link=static address-model=32 --prefix=<boost install folder/x86> install runtime-link=static 
> b2 toolset=msvc-14.0 variant=<debug,release> threading=multi link=static address-model=64 --prefix=<boost install folder/x64> install runtime-link=static

This will generate boost static libraries (.lib) statically linked with CRT with multi-thread support.

For other general build instructions refer:
http://www.boost.org/doc/libs/1_65_0/more/getting_started/windows.html


## Build Boost for Android using NDK r10e on MacOS

1. Download boost_1_65_0.tar.bz2.
https://sourceforge.net/projects/boost/files/boost/1.65.0/boost_1_65_0.tar.bz2/download

2. In the directory where you want to put the Boost installation, execute

$tar --bzip2 -xf <download path>/boost_1_65_0.tar.bz2

3.Copy boost_1_65_0/tools/build/example/user-config.jam to boost_1_65_0/tools/build/src

4.Add following to boost_1_65_0/tools/build/src/user-config.jam

import os;

androidNDKRoot = <Android NDK home Dir> ; # put the relevant path
 using gcc : android : $(androidNDKRoot)/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64/bin/aarch64-linux-android-g++ :
     <compileflags>--sysroot=$(androidNDKRoot)/platforms/android-9/arch-arm
     <compileflags>-Os
     <compileflags>-fno-strict-aliasing
     <compileflags>-O2
     <compileflags>-DNDEBUG
     <compileflags>-g
     <compileflags>-lstdc++
     <compileflags>-I$(androidNDKRoot)/sources/cxx-stl/gnu-libstdc++/4.9/include
     <compileflags>-I$(androidNDKRoot)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include
     <compileflags>-D__GLIBC__
     <compileflags>-D_GLIBCXX__PTHREADS
     <compileflags>-D__arm__
     <compileflags>-D_REENTRANT
     <archiver>$(androidNDKRoot)/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64/bin/aarch64-linux-android-ar
     <ranlib>$(androidNDKRoot)/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64/bin/aarch64-linux-android-ranlib
         ;


$ Issue following commands in the shell

$ cd <path_to_boost_1_65_0>
$ ./bootstrap.sh
$ ./b2 threading=multi link=static toolset=gcc-android variant=release --prefix=<boost install path> install

For boost 1.78 for 32bit we have to use -fpic while building!
b2 cxxflags=-fPIC address-model=32 threading=multi link=static toolset=gcc-android variant=release --prefix=/Users/abhilashkumarjaiswal/boost_1.78_21e_32_fpic install
This will generate boost static libraries (.a) statically linked with libc with multi-thread support to be used in ndk r10e
