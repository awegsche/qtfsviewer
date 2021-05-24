# qtfsviewer

A simple viewer of MADX TFS files using the Qt framework.

## Installation

### Build from source

To build from source, clone the repository and build it using your usual Qt build tools.
The easiest way is probaly getting the Qt SDK from [the Qt homepage](https://www.qt.io/download) and follow the installation instruction.

This project is released under Qt's open source rules, so it links dynamically to Qt. In linux this means that the Qt runtime has to be installed.
**Windows users** need to _deploy_ their built binaries first.

### Binary release

Check out my precompiled binaries in the [Releases](https://github.com/awegsche/qtfsviewer/releases) tab on the right. 

**Note**: Currently this project only supports Qt5 from Qt5.12 onwards. Qt6 is not supported but an upgrade is planned.



## Usage

[Optional] After successfull installation / build, add the binary's directory to your `PATH` (or use any other method to be able to conveniently launch the executable).

The executable can be launched in commandline like this

```
tfsviewer <PATH_TO_FILE>
```

which takes as optional argument the path to a TFS file to open. If no path is supplied, it will open an empty session.



The folder icon and [File->open] shows an open file dialog

The floppy icon and [File->save] does nothing for the moment (not yet implemented)

Right to the label `Data` there is a search box.
Typing a pattern will filter for this pattern in all String columns of the dataframe.

The "Save Compressed" button saves the tfs into a **highly experimental** binary format

The line plot icon and [Plotting->Line Plot] shows a line plot. For this, the plot data has to be selected first:
 - select a whole column: the values are plotted against an integer index
 - select more than one column: the values are plotted against the column that has been selected first (multi-select with Ctrl+click)
 - everything else will result in the error message: "wrong number of columns selected: 0"
 
 The scatter plot icon and [Plotting->Scatter Plot] does the same but shows a scatter plot (bug: no colors)
 
 issues and todos:
 
 - Editing! (for now, data can't be edited, added or removed)
 - saving standard tfs files
 - creating empty tfs
 
 - add more plot colors, make plots editable
 - save plots to various output formats (png, pdf, latex???)

Plotting uses the great [QCustomPlot](https://www.qcustomplot.com/).

## License

To comply with Qt's rules, this project is licensed under GPL.
