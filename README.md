# WireFS
WireFS is a FUSE-based filesystem written for the CS444 - Operating Systems class at Clarkson University.
It is not a stable file system, nor very efficient.

## Files
- dir.\* - Contains the functions for maintaining the main root directory (as subdirectories are not allowed).
- file.\* - Contains metadata about the files, that do not necessarily pertain to the data itself (that is, the mode, owner, etc.).
- inode.\* - Contains functions that manage writing to and reading from the inodes to the disk image file.

## Bugs
- Many FS options not implemented.
- Probably crashes at many points.
- Truncate is untested.
- Writing really large files doesn't work
