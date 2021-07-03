# MHPatches

![logo_1](https://user-images.githubusercontent.com/26774830/124336128-8907ab00-dba5-11eb-8481-5941025df10b.png)

# Intro

MHPatches is a plugin that brings some of PS2 features of Manhunt to the PC.


# Requirements

* UAL (https://github.com/ThirteenAG/Ultimate-ASI-Loader)
* debugmenu.dll (optional) (https://github.com/Fire-Head/debugmenu_mh)
* models with fixed alpha (optional) (see "Releases" section)

# Dual pass 

Dual pass in short is a thing that emulates PS2 method of alpha rendering which is much better than what we have on PC and XBOX. In fact all rockstar PC\Xbox ports on RW engine have this problem. You probably saw this feature in SkyGfx if you are interested in GTA modding. If you want to know more about this you can read well written [article](http://gta.rockstarvision.com/skygfx/skygfx.html#SAatest) by aap, the creator of SkyGfx. 

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

# EnvMap

Learn more about environment mapping and why it is different on all platforms [here](http://gta.rockstarvision.com/skygfx/skygfx.html#VCvehicle). 

![EnvMap](https://user-images.githubusercontent.com/26774830/124336371-793c9680-dba6-11eb-8ad4-15223a9fe7ce.png)

# Lighting and vertex alpha fix 

Fixed-pipeline has major issues with drawing vertex color with lighting enabled,
mesh turns black and doesn't react to any light type except ambient.
The problem is also present on Xbox, PS2 is the only platform on which it works fine. 

![Lighting and vertex alpha fix](https://user-images.githubusercontent.com/26774830/124336620-61194700-dba7-11eb-9ca4-a5b9c68e0489.png)

Hardware T&L implementation is different from DirectX standard on NVidia cards, as result spotlights are brighter then they should be.
The plugin replaces this light type with ATI's formula which is equivalent to PS2.

![spotlight](https://user-images.githubusercontent.com/26774830/124336705-adfd1d80-dba7-11eb-8a84-441dd5754568.gif)

Both things can only be done by replacing the default rendering pipeline using shaders instead of fixed states.
Luckily it can be done with DirectX 8.1, so wrappers are not necessary and compatibility with old hardware shouldn't be broken, except for some weak cards that don't support shaders well. 

# YCbCr

PS2 analog colors on your PC, simple as one two three. 

Demonstation: 

![YCbCr](https://user-images.githubusercontent.com/26774830/124336715-b9e8df80-dba7-11eb-8892-df77bc8e73bb.gif)


# Special Thanks
* aap - for providing his PS2 EnvMap pipeline and helping with PS2 RenderWare research.
* ermaccer - for making [video](https://www.youtube.com/watch?v=C8XR1K14h8A) about the plugin.







