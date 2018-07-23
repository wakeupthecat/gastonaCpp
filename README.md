#  GastonaC++

## About Gastona

Gastona is a scripting language to make applications easily. The original project
which is a java implementation for both desktop and Android applications can be found 
at https://github.com/wakeupthecat/gastona.

This repository is for the C++ version of gastona and starts now in July 2018.

## The C++ vs the java version

Some differences between C++ and java implementations can be presented as pros/cons

For java pros (C++ cons)

      + Direct benefit from all open source written in java
      + It has access to the powerful and mature native java libraries (i.e. regexp, http, and many more ...)
      + It does support javascript natively (it includes the open source Rhino)
      + Extensible (zWidgets and listix commands) via java
      + It can be used as library for developing java native applications (GPLv3)
         
and for GastonaC++ pros (java cons)

      + It can benefit from all open source written in C/C++
      + It can support more natively sqlite since the original code is C
      + It can support more natively Lua since the original code is C
      + Extensible (zWidgets and listix commands) via C++ (Win Api or Gtk)
      + It can be used as library for developing C++ native applications (GPLv3)
         
## Two implementations Win and Gtk

Since we want to have gastonaC++ in windows and in linux natively we will implement two
versions of gastonaC++: one using the Windows API and the second using GTK for linux.

It will be only a double effort for some few structural modules (e.g. WinMain etc) and
of course the wrappers of the native widgets (zWidget). But as it is done with gastona java for
desktop and for android, the two versions will share most of the base source code without
changes.


In order to achieve that we will have the folder structure

         base  for all the common stuff
         win   for the specific windows modules and widgets
         gtk   for the specific gtk modules and widgets

right now we have also sqlite and lua directories but they can be considered base.


## Implementation guideline and goals

We already have the EvaLayout manager translated in C++ with composition and mask mechanism 
so many GUI dialogs can be written just using this powerful layout. Later as secondary priority
can be added other kind of layout containers like splitters, tabs and menues.

Of course we need a minimum set of zWidgets in both versions with similar or better functionality as 
the counterparts in java. While this seems to me an easy task in Gtk, as I learn it, it will be ardous
in Windows, I guess.

For the logic, since developing a listix like engine can take longer, I am about to build all the logic
based on Lua, as it is done in jGastona using javascript. Of course extending lua to access the data
structures of the widgets implemented with EvaUnits and in many other useful tasks for the GUI logic.


## Current status: Proof of concept

We have achieved a "proof of concept" initial status where we compile, link and execute
gastonaC++ in both architectures WinApi and Gtk linux. Having following initial capabilities

      - Skeleton of gastona that read the gast file and loads the GUI and the logic
      - Evalayout with composition and masking
      - Set of demo zWidgets
      - Implementation of #javaj unit, creating, layouting and handling the widgets
      - Included Lua version 5.3.4
      - Logic build in lua taken from the script unit #luauix# that react to "main0", "main" and all messages defined on it
      - Included Sqlite based on the original sqlite 3.24.0 plus few modifications
        that can be used (all functionality as the original command line or shell.c)

See WinGastonaCNotes.mktes and GtkGastonaCNotes.mktes for specific details and build instructions.
