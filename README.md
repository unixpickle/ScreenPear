# Umbilical

Umbilical is my attempt at an easy-to-use system for remote displays on Mac. Right now, it consists of two drivers which are necessary to create "displays" on the go from a user-space client.

From here, I am going to make a simple Objective-C API for accessing `UBFramebuffer`s from userspace.  I will then put this API to work in a user-friendly application which will allow anonymous clients to connect to new displays and stream visual data with *h.264*. I am very excited for this project.

# Acknowledgements

I was very fortunate to have found [EWProxyFramebuffer](https://github.com/mkernel/EWProxyFramebuffer/).  This open source project, along with [IOProxyVideoFamily](https://code.google.com/p/ioproxyvideofamily/), were my introduction to subclassing `IOFramebuffer` for the driver.

# License

Copyright (c) 2014 Alex Nichol

This software is under the [GNU GPL](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
