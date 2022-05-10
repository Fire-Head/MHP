# MHP

![logo_1](https://user-images.githubusercontent.com/26774830/124336128-8907ab00-dba5-11eb-8481-5941025df10b.png)

# Intro

MHPatches is a plugin that brings some of PS2 features of Manhunt to the PC.


# Requirements

* UAL (https://github.com/ThirteenAG/Ultimate-ASI-Loader)
* debugmenu.dll (optional) (https://github.com/Fire-Head/debugmenu_mh)

# PS2 Alpha Test 

PS2 Alpha Test in short is a thing that emulates PS2 method of alpha rendering which is much better than what we have on PC and Xbox. In fact all R* PC\Xbox ports on RW engine have this problem. You probably saw this feature in SkyGfx if you are interested in GTA modding. If you want to know more about this you can read well written [article](http://gta.rockstarvision.com/skygfx/skygfx.html#SAatest) by aap, the creator of SkyGfx. 

![Dual pass](https://user-images.githubusercontent.com/26774830/124336151-9a50b780-dba5-11eb-8073-83b48e263a0d.png)

# PS2 halos 

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

![PS2 halos](https://user-images.githubusercontent.com/26774830/124336203-c9ffbf80-dba5-11eb-93bd-4715c4f6d42e.png)

# Usable objects highlighting fix 

Story with this thing is pretty short. On PC (and only on PC this time, everything works fine on consoles) light intensity value is always zero, and that's the reason why it didn't work. 

Demonstation: 

![Animation](https://user-images.githubusercontent.com/26774830/124336236-eef43280-dba5-11eb-830a-d7915539b2e1.gif)

# Crosshair Fix  

In the PC port static crosshair was added since you controlling player with the mouse now, closest analogy would be PC ports of GTA 3 and Vice City. So what is the problem? Well, the static crosshair will not disappear when you in the scope of sniper rifle or tranquilizer which looks wrong. This fix is turning it off when you in the scope.

Demonstation:

![Crosshair Fix](https://user-images.githubusercontent.com/26774830/167521073-81e4fc1a-90d3-4b6b-9332-af7a9c2a7d7d.gif)

# EnvMap

Learn more about environment mapping and why it is different on all platforms [here](http://gta.rockstarvision.com/skygfx/skygfx.html#VCvehicle). 

![EnvMap](https://user-images.githubusercontent.com/26774830/124336371-793c9680-dba6-11eb-8ad4-15223a9fe7ce.png)

# PS2 Matfx Dual Pass 

Replaces the dual texture render with a new one which is done via shaders copying PS2 blending as much as possible. 

Note: This option is related to the PS2 player damage.

![PS2 Matfx Dual Pass](https://user-images.githubusercontent.com/26774830/167521186-0b7f72c3-51b7-4142-b6b8-731a8725c3c4.png)


# PS2 player damage  

In the PS2 version blood/stamina(sweat on the back of a player when low on stamina)/bruises overlays have much more complex realization. The trick which was used is called [palette swap](https://en.wikipedia.org/wiki/Palette_swap) and allows you to recolor the image with minimal effort and no performance cost. This technique was used for damage parts in the game with one exception, the alpha channel is changed in the palette, each piece of damage is assigned to its own cells in the palette and by editing values of alpha channel on the fly by game code this whole effect is being achieved. On the demonstration GIF you can clearly notice how blood and sweat works separately. It was completely lost in PC and Xbox ports. 

PC and Xbox receive much cheaper form of it with multiple textures with different values of transparency, all parts at once will apply on the player if you low on health, sweat feature is not working at all for obvious reasons. 
Such choice was made due hardware limitations of GPU's which dropped support of paletted textures. 

So, how then MHP get it working on the PC you may ask. Well, it was done in a bit hacky way. Plugin moves palettes to the CPU side and rebuilds the textures every time the palette is updated by the game into the classic RGBA which all GPU's can understood. This of course a slightly more performance consuming method than just entrusting this to the GPU, but given the low frequency of palette changes, low texture resolution and small number of peds in the frame, this doesn't change much even for the CPU's of that time when the game was released.

Demonstation:

![Blood and Sweat](https://user-images.githubusercontent.com/26774830/167520954-a0d8bafd-8f87-43dc-9032-55b1c9a49158.gif)


# Lighting and vertex alpha fix 

Fixed-pipeline has major issues with drawing vertex color with lighting enabled,
mesh turns black and doesn't react to any light type except ambient.
The problem is also present on Xbox, PS2 is the only platform on which it works fine. 

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

PS2 analog colors on your PC, simple as one two three. 

Demonstation: 

![YCbCr](https://user-images.githubusercontent.com/26774830/124336715-b9e8df80-dba7-11eb-8892-df77bc8e73bb.gif)

# MSAA and MSAA RW support  

What is anti-aliasing is known to pretty much everyone, let's not talk about it much and get straight to the MSAA RW support. 

The problem is that Nvidia drivers are broken in regard of anti-aliasing and you may suffer from black screen or other graphical corruptions. This option is the solution for this problem, it adds full MSAA support to RenderWare, thus solves the problems of Nvidia GPU drivers. A bit performance consuming so bare that in mind. 

Note for ATI GPU users: This option is not needed thank to the driver developers, so leave it off.  

# Special Thanks
* aap - for providing his PS2 EnvMap pipeline and helping with PS2 RenderWare research.
* ermaccer - for making [video](https://www.youtube.com/watch?v=C8XR1K14h8A) about the plugin.







