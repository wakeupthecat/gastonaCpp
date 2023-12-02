#  GastonaC++

## About Gastona and C++

Gastona is a scripting language to achieve Rapid Application Development (RAD). The original project is a java implementation for both desktop and Android applications that also can be found
in github in https://github.com/wakeupthecat/gastona.

## Archived at status "proof of concept"

This project was started with the intention of be developed but it will be archived for following reasons

The development in C++ is of course possible but it required too much developing effort, specially for the GUI elements 
native widgets in windows is hard and even if Gtk library is used (actually done in the project) the development is not trivial.

Furthermore there is another gastona port much more promising than in C++ and it is in Python language, in particular 
basing the widgets in the powerful package wxPython. Although wxPython is based on wxWidgets which is actually also
available in C++, the extra bonus of doing gastona in Python is the vast world of Python packages and of course being able to 
use Python as script language within gastona scripts and the ease of extending gastona itself in Python in comparison to Java or C++.

Very soon a gastonaPy project in github will be published

## The C++ vs the java version

Some differences between C++ and java implementations can be presented as pros/cons

For java pros (lack of in C++)

      + Benefit of java world: libraries (e.g. regexp, net http ..) and java open source
      + It does support javascript natively by including Rhino's original library
      + Extensible using java and javascript
         
and for GastonaC++ pros (lack of in java)

      + Benefit of C/C++ open source wordl
      + Native support of sqlite (included in code)
      + Native support of Lua (included in code)
      + Extensible using C/C++ and Lua
      + Edit component Scintilla (very serious editor, i.e. Notepad++)

Both are multiplataform since gastonaC++ is compiled with gcc using Win Api for windows
and Gtk for both windows and Linux. Right now Android is not in focus of gastonaC++ but it should
be the next step when the time and resources are there..

         
## Two implementations Win and Gtk

Since we want to have gastonaC++ in windows and in linux natively we will implement two
versions of gastonaC++: one using the Windows API and the second using GTK for both windows and linux.

It will be only a double effort for some few structural modules (e.g. WinMain etc) and
of course the wrappers of the native widgets (zWidget). But as it is done with gastona java for
desktop and for android, the two versions will share most of the base source code without
changes.


In order to achieve that we will have the folder structure

         base  for all the common stuff
         win   for the specific windows modules and widgets
         gtk   for the specific gtk modules and widgets

right now we have also sqlite and lua directories but actually they belong to base.


## Implementation guideline and goals


### GUI

EvaLayout manager exists already in C++ with composition and mask mechanism 
so many GUI dialogs can be written just using this powerful layout. Later 
can be added other kind of layout containers like splitters, tabs and menues etc.

Add more widgets and improve its functionality.


### Logic

A listix version in C++ would be desirable but it can take a little bit of effort.
The first idea is to use Lua with some special functions as only script language for the
logic. This is what is done in jGastona using javascript intead of Lua.
 

## Current status: Proof of concept

We have achieved a "proof of concept" initial status where we compile, link and execute
gastonaC++ in both architectures WinApi and Gtk linux. Having following initial capabilities

      - Skeleton of gastona that read the gast file and loads the GUI and the logic
      - Evalayout with composition and masking
      - Set of initial zWidgets including buttons, lists, tables, trees and Scintilla editor
      - Implementation of #javaj# unit, creating, layouting and handling the widgets
      - Included Lua version 5.3.4
      - Logic build in lua taken from the script unit #luaix# that react to "main0", "main" and all messages defined on it
      - Included Sqlite based on the original sqlite 3.24.0 plus few modifications

See WinGastonaCNotes.mktes and GtkGastonaCNotes.mktes for specific details and build instructions.
