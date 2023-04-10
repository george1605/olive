import gg
import gx
#include "@VMODROOT/include/gfx.h"

type DrawFn = fn (mut ctx gg.Context)
struct OlWindow {
pub mut:
	front  &u8
	back   &u8
	width  int
	height int
}

pub fn C.ol_setup(x int, y int)
pub fn C.ol_new_win() OlWindow
pub fn C.ol_line_bres(win OlWindow, x int, y int, a int, b int)

struct Window
{
	mut:
		ctx &gg.Context = unsafe { voidptr(0) }// not public
		pub:
		func DrawFn
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
	image := win.ctx.create_image_from_memory(buffer, size)
	win.ctx.draw_image(0, 0, width, height, image)
}

pub fn ol_setup(x int, y int)
{
	C.ol_setup(x, y)
}

pub fn ol_new_win() OlWindow
{
	return C.ol_new_win()
}

pub fn ol_line_bres(win OlWindow, x int, y int, a int, b int)
{
	C.ol_line_bres(win, x, y, a, b)
}

fn main()
{
	mut win := Window{}
	win.new("Hello!", 400, 500)
	ol_setup(300, 400)
	olwin := ol_new_win()
	ol_line_bres(olwin, 30, 40, 50, 100)
	win.view_buf(olwin.front, 300, 400)
}