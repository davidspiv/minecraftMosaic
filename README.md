Help I can't stop coding! Here is some of the results from the a program I'm making creates a "mosaic" from discrete textures...in this case, from Minecraft.

**The Problem**
In the most recent Minecraft version, Mojang stores the texture as separate PNG files, not as a single [texture atlas](<https://en.wikipedia.org/wiki/Texture_atlas>) AND there are a lot of transparent textures I didn't want.

**The Solution**
So, it was a matter of modifying the "picture" class to accept the Alpha (transparency) channel, detect the textures I wanted, and storing them in memory.

Then, for each 16x16 block of photo, I computed the average color, and found the nearest neighbor in sRGB color space using the euclidean distance formula. Lots of words, but basically just treated the RGB color like a 3D coordinate.

**Future Changes**
Change to a different color space to get more accurate results, and audit the minecraft texture map more to exclude things like command blocks. And to make it several orders of magnitude faster, obviously. Possibly make it into a map art schematic generator...

For those interested, here's a relevant video to color theory, taught using Minecraft: [How to average color](<https://www.youtube.com/watch?v=e0HM_vfSuDw&t=46s>).

- First I take an image and hit it with a a little:
[Gaussian Blur](<https://en.wikipedia.org/wiki/Gaussian_blur>)

- Then, I shrink it down with a little [Bilinear Interpolation](<https://en.wikipedia.org/wiki/Bilinear_interpolation>).

- Then, I convert all the color data to [OkLab](<https://bottosson.github.io/posts/oklab/>) colors.

Finally, I take each pixel color and match it the closest averaged block, building a lookup table. I  use the lookup table to make the image at the end.
