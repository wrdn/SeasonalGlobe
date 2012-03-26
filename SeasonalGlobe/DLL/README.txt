The DLL files in this directory need to be copied to the executable directory
As part of the build process, Visual Studio will automatically do this as a post
build event

For libvorbis and OGGs to work correctly (without the application crashing), you need
the DLLs in DebugLibs for a Debug build, and in ReleaseLibs for a Release build