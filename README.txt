A tool for CI purpose. Not really a package manager.

Usage:
    ppkg.exe archivename.zip

Extract content to pre-defined paths in environment variables.

Assume this is an archive's file tree:

%TEST%/
%TEST%/somefolder/
%TEST%/somefolder/somefile.ext
%TEST%/someotherfile.ext
regularfolder/
regularfile.ext

Assume there is an environment variable called TEST was set to C:\Test\ and such folder exists, then this tool would extract all the files under %TEST%/ to C:\Test\ and leave other files unextracted:

C:\Test\somefolder\somefile.ext
C:\Test\someotherfile.ext

Building:

Both CMake and Meson project files are provided.

- CMakeLists.txt: for IDE integration.
- meson.build: for CI build, link everything staticly to produce a single binary for CI.

Regular CMake or Meson build steps applied, nothing special.

Notice:

1. Only check the folders in the root of the archive with its name starts and ends with '%'.
2. If matched, the value of the associated environment variable must be an existing path.
3. All *matched* folder names will be checked, and this tool will do nothing if any check failed.
4. If all check passed, this tool will start extracting the contents inside these matched folder. Other regular file and folder will not get extracted.
5. While extracting, existing files will be *overrided*.
