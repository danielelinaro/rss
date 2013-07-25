# RSS
rss is a command line RSS feed reader that runs on Unix based systems.

### Installation
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

### Usage
To initialize a directory, just type:
```
rss init
```
This will create a .rss directory, which will contain all your feed
information and related data.

To add a feed to the database, type:
```
rss add -f -a feed_alias feed_url
```
The -f option instructs rss to also fetch the feed after adding it to
the database. In all _rss_ commands, the alias and the full URL of a
feed can be used interchangeably.

To fetch the latest feed, type:
```
rss fetch feed_alias
```

To list all the feeds in the database, type:
```
rss list
```

To remove a feed from the database, type:
```
rss remove feed_alias
```

To display information about each command, type:
```
rss help command
rss command -h
```

The list of available commands can be obtained by typing
```
rss help
```

### Version
0.1

### License
GPL

#### Author
Daniele Linaro - danielelinaro@gmail.com
