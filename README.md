RSS
===

RSS is a command line RSS feed reader that runs on Unix based systems.

Installation
------------
Compiling and installing RSS is straightforward. Just clone the
repository from GitHub and run make in the base directory: this will
create a _rss_ executable that you can then copy wherever you like in
your path. Make sure to have libcurl and libxml2 installed, or the
compilation will fail.
```
git clone https://github.com/danielelinaro/rss.git rss
cd rss
make
```

Usage
-----
To initialize a directory, just type:
```
rss init
```
This will create a .rss directory, which will contain all your feed
information and related data.

Version
-------
0.1

License
-------
GPL

Author: Daniele Linaro - danielelinaro@gmail.com

