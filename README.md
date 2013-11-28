To build (C)
============

```
cd c/
make
```

You need to create a user.in file to build the C code, as it gets
compiled remotely on Jotunn. This file is a text file with two
lines. The first line is your jotunn username, and the second line is
where you want to sync the code to.

Example:

```
myusername
/home/myusername/elevator/
```

To build (Java)
===============

You need to install ant and ivy. To build (on Debian/Ubuntu):

```
export CLASSPATH=/usr/share/java/ivy.jar
ant
```

You then need to copy the files to Futuregrid with scp, e.g. `scp -r
dist/ username@sierra.futuregrid.org:/N/u/username`.

To run it, drop the input files in the dist/ directory, and then qsub
the pbs-example.sh script. 