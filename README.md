# Jeex
**Jeex** is an open source Hexadecimal Editor written with Gtk+ and released under GNU GPL 3 license. 
It allow you to open any file, view it in hexadecimal format and make several operations and statistics also 
using other views like **ASCII**, **decimal**, **octal** and **binary**.

You can do search and replace actions, strings extraction, view the file as list of structures, with several data 
types, or add bookmarks to save position where an relevant information was located. On the file you can get informations 
on file strings, frequency analysis on characters, hash or show differrences with others files.

Many others are the Jeex functionality, increasable with **built-in plugin**. For more informations _read more_!

## Install & Download
Jeex can be installed on your system directly by your default package manager. For example on Debian derivated 
distributions ( like Ubuntu ) you can install it from terminal with following command:

```
apt-get install jeex
```

However you can install Jeex manually compiling the sources with the usual commands:

```
tar -xjvf 'jeex-package'.tar.bz2
cd 'jeex-package'
./configure
make
make install
```

## Menu

### File
In **FILE** you can find tools to create a new file, open an existed one or open all files in specified path. 
If a file is already opened, you can choose to add another file's content to it.

After this actions, there is a list of last opened files and other actions to save, save as.. or export an 
[hexadecimal view in HTML](http://www.neckersbox.eu/var/www/html/neckersbox/jeex_export/strish_1.html). 
At the end you can find functions to show properties like _permiss_, _size_, _**mime-type**_, _modified dates_ 
or _last access_.

### Edit
In **EDIT** there are the usally actions to copy, paste, cut and selection, in addition to these there are more 
specified actions like **insert characters from other format** such ASCII, hexadecimal, decimal, octal or binary. 
These formats will be automatically convert by **Jeex** in hexadecimal and formatted. Then there is an actions to 
insert a value from a **bit operation**, i.e. given two binary numbers you can do logic operations like AND, OR and XOR.

As last there are actions to reload the file, discarding the changes, open _Jeex preferences_ to change aspect or 
some functionalities.

### View
Inside this menu you can add a view to opened file, i.e. show its content with offset references and ascii for each 
line of hexadecimal bytes. You can also get file's hash to check its integrity with **MD5**, **SHA-1** and 
**SHA-256** algorithm.

Jeex allow you to do automatic statistics on current document, on occourrences number for each single byte, show the 
differences with another chosen file and show its content in others formats.

### Search
Search action inside the file will be done either for hexadecimal values or others formats previously listed. In the 
same way can be done search and replacing actions.

You can do, also, **search by regex** inside file's content.

### Bookmarks
Bookmarks functions it useful if you want to save informations there are in _differents positions_ on current file. In 
this way you must only positioning the cursors in chosen position, or choose the offset, to save them and add a name 
and description to bookmark to get it easy later.

### Tools
In this menu you can found useful general actions like **strings extraction** in the current file, join more file in one 
or viceversa: split current file in more files with fixed size.

A important tools in this part of Jeex is the **structures view**. If you are doing actions on raw data, in which was 
saved a series of structures with the same data content, you can re-creating this structures specifing the structure's 
types, such int, double, char, etc...

### Plugin
**Jeex plugins**, as said previously, are _built-in_, i.e. they are internal of program's code. There are not possible 
adding dynamics plugins ( unless you create a plugin to allow this function ;-) ).

By the way, inside the sources you can find **examples plugins** to understand, in easy way, as Jeex gived tools to access 
to internal components without editing its internal code.
