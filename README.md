tasty-pastry
============

A 4k demo framework for Raspberry Pi

Introduction
==
Tasty-pastry is a work-in-progress demo framework for the Raspberry Pi. This is a long term project, but there are some useful gems already that might be of use to others. The size-oriented linking and fast ESSL trig primitives are believed to be state of the art on Raspberry Pi.

Implemented features
==
  * C framework to initialise OpenGL ES
  * 4k optimised link process
  * LZMA based executable packer
  * OpenGL ES shader language fast sin and cos primitives

Shader acceleration
==
In Videocore shader language, the following operations are quick:
  * Addition, subtraction
  * Multiplication

These are SLOOOOW.
  * Division
  * inversesqrt(), sqrt()
  * sin(), cos(), tan()
  * asin(), acos(), atan()

The sinf() implementation is much faster. The intrinsic takes 20 multiplies and 11 additions. sinf() takes 7 multiplies and 6 additions.


Credits
=======
  * OpenGL|ES init code based on [hello_triangle from Broadcom](https://github.com/raspberrypi/firmware/tree/master/opt/vc/src/hello_pi/hello_triangle)
  * Executable packer idea based on [You Massive Clod by parcelshit^fearmoths](http://www.pouet.net/prod.php?which=51762)
  * Sstrip from [ELFkickers by Brian Raiter of Muppetlabs](http://www.muppetlabs.com/~breadbox/software/elfkickers.html)
