import gg
import gx
import os
// #flag windows -l olive
// link against the olive dll

type DrawFn = fn (mut ctx gg.Context)

struct Window
{
	mut:
		ctx &gg.Context = unsafe { voidptr(0) }
	pub mut:
		func DrawFn
}

pub fn(mut win Window) from(ctx &gg.Context) 
{
	win.ctx = ctx
}

pub fn(mut win Window) new(name string, width u16, height u16) 
{
	win.ctx = gg.new_context(
		bg_color: gx.rgb(255, 255, 255)
		width: width
		height: height
		window_title: name
		frame_fn: win.func
	)
	win.ctx.run()
}

pub fn(mut win Window) view_buf(buffer &u8, width int, height int)
{
	size := width * height * 4
	image := win.ctx.create_image_from_memory(buffer, size) or { gg.Image{} }
	win.ctx.draw_image(0, 0, width, height, &image)
}

pub fn(mut win Window) view_img(filename string)
{
	if !os.exists(filename) {
		panic("Image file $filename does not exist.")
	}
	image := win.ctx.create_image(filename) or { gg.Image{width:-1} }
	println("$image.width x $image.height")
	win.ctx.draw_image(0, 0, image.width, image.height, &image)
}

pub fn(mut win Window) begin()
{
	win.ctx.begin()
}

pub fn(mut win Window) end()
{
	win.ctx.end()
}

pub fn(mut win Window) circle(x f32, y f32, radius f32, color gx.Color, filled bool)
{
	if filled {
		win.ctx.draw_circle_filled(x, y, radius, color)
	} else {
		win.ctx.draw_circle_empty(x, y, radius, color)
	}
}

pub fn(mut win Window) view_ppm(filename string)
{
	// TO DO
}

fn draw(mut ctx gg.Context)
{
	mut win := Window{}
	win.from(ctx)
	win.begin()
	win.view_img("flower.jpg")
	win.end()
}

fn main()
{
	mut win := Window{}
	win.func = draw
	win.new("Hello!", 600, 700)
}