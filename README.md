# ropengl
OpenGL bindings for R

Most of the openGL functions work with their native syntax. Functions which are passed pointers to store data (e.g. glGenBuffers(int n, int * buffer)) will simply return the return value instead of storing it in a pointer, and no longer take a pointer as an argument. Functions such as texImage2D have been updated for easier implementation. glTexImage2D now simply takes a file name and performs the command as such.

You may ask: why do openGL bindings belong in R? 

Well, how else will we be able to port Minecraft to R? It's been far too long awaited.

To install binaries (located in binaries/), just launch R and use this command:

`install.packages("<filename>", repos = NULL)`

to install from source, first install devtools:

```
install.packages("devtools")
library(devtools)
```

and then use load_all("ropengl")
