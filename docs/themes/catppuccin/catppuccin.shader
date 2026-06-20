// Catppuccin theme shaders (generated). 'hidden' draws nothing so menu
// decorations can be erased; the rest are alpha-blended widget art.

theme/catppuccin/hidden
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/blank.tga
		blendFunc GL_ZERO GL_ONE
	}
}

theme/catppuccin/angle
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/angle.tga
		blendfunc blend
	}
}

theme/catppuccin/angle_on
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/angle_on.tga
		blendfunc blend
	}
}

theme/catppuccin/scrollbar
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/scrollbar.tga
		blendfunc blend
	}
}

theme/catppuccin/scrollbar_thumb
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/scrollbar_thumb.tga
		blendfunc blend
	}
}

theme/catppuccin/arrow_up
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/arrow_up.tga
		blendfunc blend
	}
}

theme/catppuccin/arrow_dwn
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/arrow_dwn.tga
		blendfunc blend
	}
}

theme/catppuccin/arrow_left
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/arrow_left.tga
		blendfunc blend
	}
}

theme/catppuccin/arrow_right
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/arrow_right.tga
		blendfunc blend
	}
}

theme/catppuccin/slider
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/slider.tga
		blendfunc blend
	}
}

theme/catppuccin/sliderbutt
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/sliderbutt.tga
		blendfunc blend
	}
}

theme/catppuccin/backarrow
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/backarrow.tga
		blendfunc blend
	}
}

theme/catppuccin/backarrow_on
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/backarrow_on.tga
		blendfunc blend
	}
}

theme/catppuccin/acceptarrow
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/acceptarrow.tga
		blendfunc blend
	}
}

theme/catppuccin/acceptarrow_on
{
	nopicmip
	nomipmaps
	{
		map theme/catppuccin/acceptarrow_on.tga
		blendfunc blend
	}
}


// --- Forced-color fills (rgbGen identity ignores the VM vertex tint, so these
// override VM-coloured elements like the server-browser headers/selection). ---
theme/catppuccin/probe
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/magenta.tga
		rgbGen identity
	}
}

theme/catppuccin/fill_surface0
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/surface0.tga
		rgbGen identity
	}
}

theme/catppuccin/fill_surface1
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/surface1.tga
		rgbGen identity
	}
}

theme/catppuccin/fill_blue
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/blue.tga
		rgbGen identity
	}
}

theme/catppuccin/fill_base
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/base.tga
		rgbGen identity
	}
}

// Semi-transparent Catppuccin panel for in-game menus (rgbGen identity ignores
// the VM's blue tint; alphaGen const keeps a slight see-through so the game
// stays visible behind the menu).
theme/catppuccin/panel
{
	nopicmip nomipmaps
	{
		map theme/catppuccin/base.tga
		blendfunc blend
		rgbGen identity
		alphaGen const 0.90
	}
}
