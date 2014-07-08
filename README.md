What is it
==========
EFL-Egueb is a library that provides a set of useful APIs for easy integration of Egueb on [EFL](www.enlightenment.org)

Abstractions
============
+ `Efl_Egueb_Window`: `Ecore_Evas` alike window object. It allows you to create a window for displaying any `Egueb` based document.
+ `Efl_Egueb_Smart`: `Evas Smart` object for embedding an `Egueb` based document into an `Evas` canvas.

Supported Egueb Features
========================
+ Script Feature

  Documents that require scripting, either by `<script>` tag or by listeners like `onclick`, `onmouseover`, etc; are handled
  Currently JavaScript support is provided by using [egueb-js-sm](www.github.com/turran/egueb-js-sm)
  
+ IO Feature

  Image loading is done through `Enesim` asynchronous API
  File handling is done by using `Ecore_Con` for `http://` URIs, asynchronous too.

+ Animation Feature

  SMIL animations are handled by using an `Ecore_Timer` for the SMIL timeline.
  
+ UI Feature

  All UI events are handled or either by using the underlying Window System Ecore abstraction (`Ecore_X`, `Ecore_SDL`, ...) for the
  `Efl_Egueb_Window` API. Or by using the `Evas` events for the `Efl_Egueb_Smart` object.
