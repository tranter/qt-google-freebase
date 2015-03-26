# Introduction #

Information about use Google API, testing platforms, Qt versions.

# Details #

Project **qt-google-freebase** uses Google Freebase API v1.

### Google API ###

Project **qt-google-freebase** uses Google Freebase API v1.

How it works:
Work with Google-API perfomed by send https-request with using QNetworkAccessManager (see files _freebase\_data\_manager.h_ and _freebase\_data\_manager.cpp_)

API features used in this project:
|Search service|
|:-------------|
|MQLRead service|
|Text service|
|Image service|
|Freebase suggest|

MQLWrite service not implemented yet due to migration procedure to a new API on Google.

File **[HowToRegisterYourAppIicationInGoogle](http://code.google.com/p/qt-google-freebase/wiki/HowToRegisterYourApplicationInGoogle)** describes how register your own application on Google.

### Tested platforms ###
Project was tested on:
| **OS** | **Qt version** |
|:-------|:---------------|
|Windows 7 64bit|Qt 4.8.0|

# Various comments #

This application can be compiled on Linux, MacOS, Windows. For Windows you can use either MingW compiler or MSVC compiler.