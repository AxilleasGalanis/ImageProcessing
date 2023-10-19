# Image Processing
An image processing program made in C++. The following commands are supported:

● i \<filename> as <$token>. Import an image file named "filename" from
the filesystem, which corresponds to the unique
identifier "$token".

● e <$token> as /<filename>. Export the image associated with the
"$token" identifier to a file clarified in the "filename" path.
If the image is black and white it is exported in PGM format,
while if the image is in color it is exported in PPM format.

● d <$token>. Deletes the unique identifier "$token" from the
memory along with the image corresponding to it.
