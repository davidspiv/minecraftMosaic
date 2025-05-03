Help I can't stop coding! Here is some of the results from the a program I'm making creates a "mosaic" from discrete textures...in this case, from Minecraft.

**The Problem**
In the most recent Minecraft version, Mojang stores the texture as separate PNG files, not as a single [texture atlas](<https://en.wikipedia.org/wiki/Texture_atlas>) AND there are a lot of transparent textures I didn't want.

**The Solution**
So, it was a matter of modifying the "picture" class to accept the Alpha (transparency) channel, detect the textures I wanted, and storing them in memory.

- First I take an image and hit it with a a little [Gaussian Blur](<https://en.wikipedia.org/wiki/Gaussian_blur>)

- Then, I shrink it down with a little [Bilinear Interpolation](<https://en.wikipedia.org/wiki/Bilinear_interpolation>).

- Then, I convert all the color data to [OkLab](<https://bottosson.github.io/posts/oklab/>) colors.

Finally, I take each pixel color and match it the closest averaged block, building a lookup table. I use the lookup table to make the image at the end.

This is my [repo](<https://github.com/davidspiv/minecraft_mosaic>):

Here are photos with / without the extra conversions.


**Future Changes**
For those interested, here's a relevant video to color theory, taught using Minecraft: [How to average color](<https://www.youtube.com/watch?v=e0HM_vfSuDw&t=46s>).
