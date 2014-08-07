What is it
==========
EFL-Egueb is a library that provides a set of useful APIs for easy integration of Egueb on [EFL](http://www.enlightenment.org).
The main abstractions provided are:

+ `Efl_Egueb_Window`: `Ecore_Evas` alike window object. It allows you to create a window for displaying any `Egueb` based document.
+ `Efl_Egueb_Smart`: `Evas Smart` object for embedding an `Egueb` based document into an `Evas` canvas.

Dependencies
============
+ [EFL](http://www.enlightenment.org) (Mainly ecore and evas for the smart object, optionally edje for the external object)
+ [Egueb](https://www.github.com/turran/egueb) (Mainly egueb-dom and optionally egueb-smil or egueb-svg)
+ [Gst-Egueb](https://www.github.com/turran/gst-egueb) (Optional: In case you want multimedia support using GStreamer) 
+ [Egueb-JS-SM](https://www.github.com/turran/egueb-js-sm) (Optional: In case you want JavaScript scripting support)

Features
========
Scripting
---------
Documents that require scripting, either by `<script>` tag or by listeners like `onclick`, `onmouseover`, etc; are supported. Currently JavaScript support is provided by using [Egueb-JS-SM](https://www.github.com/turran/egueb-js-sm)
  
Input/Output
------------
Image loading is done through `Enesim` asynchronous API. File handling with `http://` URis scheme is done by using `Ecore_Con`.

Animation
---------
SMIL animations are handled by using an `Ecore_Timer` for the SMIL timeline.

User input
----------
All UI feature/events are handled or either by using the underlying Window System Ecore abstraction (`Ecore_X`, `Ecore_SDL`, ...) for the
`Efl_Egueb_Window` abstraction; or by using the `Evas` events for the `Efl_Egueb_Smart` abstraction.

Multimedia
----------
The multimedia feature/events are handled by using [Gst-Egueb](https://www.github.com/turran/gst-egueb)

Communication
=============
In case something fails, use this github project to create an issue, or if you prefer, you can go to #enesim on the freenode IRC server.
