# Image Processing
An image processing program made in C++. The following commands are supported:

● ```i \<filename> as <$token>```. Import an image file named "filename" from
the filesystem, which corresponds to the unique
identifier "$token".

● ```e <$token> as /<filename>```. Export the image associated with the
"$token" identifier to a file clarified in the "filename" path.
If the image is black and white it is exported in PGM format,
while if the image is in color it is exported in PPM format.

● ```d <$token>```. Deletes the unique identifier "$token" from the
memory along with the image corresponding to it.

● ```n <$token>```.  Reverses the brightness of the image corresponding
to the unique identifier "$token".

● ```z <$token>```. Histogram equaalization of the image corresponding
to the unique identifier "$token" is performed.

● ```m <$token>```. The image corresponding
to the unique identifier "$token" is reversed (mirrored) along its vertical axis.

● ```g <$token>```. If The original image corresponding
to the unique identifier "$token" is black and white, no action is taken.
If it is in color, then the original image is replaced by 
the corresponding black and white, which
binds to the same "$token" id. The original color image is deleted.

● ```s <$token> by \<factor>```. The image corresponding
to the unique identifier "$token" is scaled the "factor" floating point number.

● ```r <$token> clockwise /<Χ> times```. The image corresponding
to the unique identifier "$token" is rotated clockwise as many times as it is described by the integer parameter "X". If "X" is
a negative number then the image is rotated 
counterclockwise as many times as it is described by the absolute value of integer parameter "X".

●  ```q```. Terminates the program. Before termination all memory that was allocated is freed.
