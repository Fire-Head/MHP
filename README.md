# MHP

![logo_1](https://user-images.githubusercontent.com/26774830/124336128-8907ab00-dba5-11eb-8481-5941025df10b.png)

# Intro

MHPatches is a plugin that brings some of PS2 graphics and features of Manhunt to the PC.


# Requirements

* [UAL](https://github.com/ThirteenAG/Ultimate-ASI-Loader)
* [debugmenu.dll](https://github.com/Fire-Head/debugmenu_mh) (optional)

# PS2 Alpha Test 

PS2 Alpha Test in short is a thing that emulates PS2 method of alpha rendering which is much better than what we have on PC and Xbox. In fact all R* PC\Xbox ports on RW engine have this problem. You probably saw this feature in SkyGfx if you are interested in GTA modding. If you want to know more about this you can read well written [article](http://gta.rockstarvision.com/skygfx/skygfx.html#SAatest) by aap, the creator of SkyGfx. 

![Dual pass](https://user-images.githubusercontent.com/26774830/124336151-9a50b780-dba5-11eb-8073-83b48e263a0d.png)

# PS2 Halos 

In general, the pickup consists of three parts:

* Model
* Corona
* Wire sphere

The model is alright, let's go straight to the corona and the wire sphere.
They are simple primitives that are drawn on top of the model.

* Corona is a 2D sprite that is always facing the player, the texture is set from TXD.
* Wire sphere is a 3D primitive that is generated during runtime and depends on the timer, it has no texture, only color.

What happens in the render and why is it bugged?

Before drawing the corona, a texture is set to it, by setting the render state (RwRenderStateSet).
After it is done rendering, the texture must be turned off (zeroed) by setting NULL.
Rockstar did not do this and corona texture is set on the sphere, making it invisble.

Why does it work on PS2?
PS2 has smarter RenderWare, it itself is able to zero the render states after rendering if required. Thanks to this texture will be NULL before drawing the sphere and it will be displayed correctly.

The plugin simply turns off the texture before drawing the wire sphere, which simulates PS2 behavior. 

![PS2 Halos](https://user-images.githubusercontent.com/26774830/174457720-e351a7c9-8f32-4835-8307-295000760593.png)

# Usable objects highlighting fix 

Story with this thing is pretty short. On PC (and only on PC this time, everything works fine on consoles) light intensity value is always zero, and that's the reason why it didn't work. This fix making it work as intended.

Demonstation: 

![Animation](https://user-images.githubusercontent.com/26774830/124336236-eef43280-dba5-11eb-830a-d7915539b2e1.gif)

# Crosshair Fix  

In the PC port static crosshair was added since you controlling player with the mouse now, closest analogy would be PC ports of GTA 3 and Vice City. So what is the problem? Well, the static crosshair will not disappear when you in the scope of sniper rifle or tranquilizer which looks wrong. This fix is turning it off when you in the scope.

Demonstation:

![Crosshair Fix](https://user-images.githubusercontent.com/26774830/167521073-81e4fc1a-90d3-4b6b-9332-af7a9c2a7d7d.gif)

# Skin Fallback Fix
The game doesn't remember the default texture when using a randomized skin, as result characters without randomized look will use a texture that was picked last time for that model.
However, cases of using the same dff for several characters with and without randomized skin at the same time are quite rare in the game, and the only known one is [Cerberus Soldiers/Cerberus Leader](https://www.youtube.com/watch?v=9eA-QAMVXtY).

The fix sets default texture to model if there no random is used.

# EnvMap

Learn more about environment mapping and why it is different on all platforms [here](http://gta.rockstarvision.com/skygfx/skygfx.html#VCvehicle). 

![EnvMap](https://user-images.githubusercontent.com/26774830/124336371-793c9680-dba6-11eb-8ad4-15223a9fe7ce.png)

# PS2 Matfx Dual Pass 

Replaces the dual texture render with a new one which is done via shaders copying PS2 blending as much as possible. 

Note: This option is related to the PS2 player damage.

![PS2 Matfx Dual Pass](https://user-images.githubusercontent.com/26774830/174457748-631b4859-bf44-40cf-b7b5-b84fb5410531.png)


# PS2 Player Damage  

In the PS2 version blood/stamina(sweat on the back of a player when low on stamina)/bruises overlays have much more complex realization. The trick which was used is called [palette swap](https://en.wikipedia.org/wiki/Palette_swap) and allows you to recolor the image with minimal effort and no performance cost. This technique was used for damage parts in the game with one exception, the alpha channel is changed in the palette, each piece of damage is assigned to its own cells in the palette and by editing values of alpha channel on the fly by game code this whole effect is being achieved. On the demonstration GIF you can clearly notice how blood and sweat works separately. It was completely lost in PC and Xbox ports. 

PC and Xbox receive much cheaper form of it with multiple textures with different values of transparency, all parts at once will apply on the player if you low on health, sweat feature is not working at all for obvious reasons. 
Such choice was made due hardware limitations of GPU's which dropped support of paletted textures. 

So, how then MHP get it working on the PC you may ask. Well, it was done in a bit hacky way. Plugin moves palettes to the CPU side and rebuilds the textures every time the palette is updated by the game into the classic RGBA which all GPU's can understood. This of course a slightly more performance consuming method than just entrusting this to the GPU, but given the low frequency of palette changes, low texture resolution and small number of peds in the frame, this doesn't change much even for the CPU's of that time when the game was released.

Demonstation:

![Blood and Sweat](https://user-images.githubusercontent.com/26774830/167520954-a0d8bafd-8f87-43dc-9032-55b1c9a49158.gif)


# Lighting and vertex alpha fix 

Fixed-pipeline has major issues with drawing vertex color with lighting enabled,
mesh turns black and doesn't react to any light type except ambient.
The problem is also present on Xbox, PS2 is the only platform on which it works as intended.

![Lighting and vertex alpha fix](https://user-images.githubusercontent.com/26774830/124336620-61194700-dba7-11eb-9ca4-a5b9c68e0489.png)

Hardware T&L implementation is different from DirectX standard on NVidia cards, as result spotlights are brighter than they should be.
The plugin replaces this light type with ATI's formula which is equivalent to PS2.

![spotlight](https://user-images.githubusercontent.com/26774830/124336705-adfd1d80-dba7-11eb-8a84-441dd5754568.gif)

Both things can only be done by replacing the default rendering pipeline using shaders instead of fixed states.
Luckily it can be done with DirectX 8.1, so wrappers are not necessary and compatibility with old hardware shouldn't be broken, except for some weak cards that don't support shaders well. 

# PS2 Brightness 

In the PC port brightness changes ambient light and gamma at the same time, if you set high values it will not look good. It would seem like bright white filter just covers the screen. Straight port of the PS2 code makes it a lot better, it changes only ambient light and at the end of day it looks a whole lot better since game itself becoming brighter.

![PS2 Brightness](https://user-images.githubusercontent.com/26774830/167521090-d4b3aaf3-9b3f-4b7f-bc03-d7508aa7da8f.png)


# PS2 CameraFX 

PS2 port has all time working filter, "trails" like effect (kinda what GTA 3 and Vice City have) and it's called "FX ArtFilter" in the debug menu. Code of it is available in the PC and Xbox ports but doesn't have support for varios resolutions, R* decided to turn it off. Now you can enable it back + with resolution support being added to it.

![PS2 CameraFX](https://user-images.githubusercontent.com/26774830/167521142-6cbc031f-7d13-4680-b01e-af3b04e7c90a.png)


# YCbCr

PS2 analog colors on your PC, simple as one-two-three.

Demonstation: 

![YCbCr](https://user-images.githubusercontent.com/26774830/124336715-b9e8df80-dba7-11eb-8892-df77bc8e73bb.gif)

# MSAA and MSAA RW support  

What is [anti-aliasing](https://en.wikipedia.org/wiki/Spatial_anti-aliasing) is known to pretty much everyone, let's not talk about it much and get straight to the MSAA RW support. 

The problem is that Nvidia drivers are broken in regard of anti-aliasing and you may suffer from black screen or other graphical corruptions. This option is the solution for this problem, it adds full MSAA support to RenderWare, thus solves the problems of Nvidia GPU drivers. A bit performance consuming so bare that in mind. 

Note for ATI GPU users: This option is not needed thank to the driver developers, so leave it off.  

# Display

Video mode got improvements and QoL changes like windowed/borderless mode with multi-monitor support or proper cursor lock.
Besides, any windowed mode now can be placed and sized manually in the config.

![VideoMode](https://github.com/Fire-Head/MHP/assets/26774830/889b06f4-7a2d-4abf-94d1-f3f274f86dfb)


# Special Thanks
* aap - for providing his PS2 EnvMap pipeline and helping with PS2 RenderWare research.
* ermaccer - for making [video](https://www.youtube.com/watch?v=C8XR1K14h8A) about the plugin.







