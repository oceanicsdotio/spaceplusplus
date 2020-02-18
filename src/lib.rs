mod agent;
mod tessellate;
mod webgl;
mod stream;


pub use tessellate::tessellate::{make_torus, draw_hex_grid};
pub use webgl::graphics_system::{create_buffer, create_program, create_texture};
pub use agent::agent_system::{Cursor};


use wasm_bindgen::prelude::*;
use wasm_bindgen::{JsCast, JsValue, Clamped};
use web_sys::{HtmlCanvasElement, CanvasRenderingContext2d, ImageData};
use std::f32::consts::{PI};
use std::mem;
use std::os::raw::c_void;
use wasm_bindgen_futures::JsFuture;
use web_sys::{Request, RequestInit, RequestMode, Response};
use std::cell::RefCell;
use std::rc::Rc;

extern crate console_error_panic_hook;


#[wasm_bindgen]
pub fn panic_hook() {
    console_error_panic_hook::set_once();
}


fn context2d (canvas: HtmlCanvasElement) -> CanvasRenderingContext2d {
    canvas
        .get_context("2d")
        .unwrap()
        .unwrap()
        .dyn_into::<CanvasRenderingContext2d>()
        .unwrap()
}

fn context_ref(target: &str) -> CanvasRenderingContext2d {
    context2d(
        web_sys::
        window()
        .unwrap()
        .document()
        .unwrap()
        .get_element_by_id(target)
        .unwrap()
        .dyn_into::<HtmlCanvasElement>()
        .map_err(|_| ())
        .unwrap()
    )
}

#[wasm_bindgen]
pub fn create_color_map_canvas(_color: JsValue) -> CanvasRenderingContext2d {
    let document = web_sys::window().unwrap().document().unwrap();
    let canvas = document.create_element("canvas").unwrap();
    let canvas: HtmlCanvasElement = canvas
        .dyn_into::<HtmlCanvasElement>()
        .map_err(|_| ())
        .unwrap();

    canvas.set_width(256);
    canvas.set_height(1);

    let ctx = context2d(canvas);
    let gradient = ctx.create_linear_gradient(0.0, 0.0, 256.0, 0.0);
//    for (let stop in colors) {
//        gradient.add_color_stop(+stop, colors[stop]);
//    }
    ctx.set_fill_style(&gradient);
    ctx.fill_rect(0.0, 0.0, 256.0, 1.0);
    return ctx;
}

#[wasm_bindgen]
pub fn clear_rect_blending(ctx: &CanvasRenderingContext2d, w: f64, h: f64, color: JsValue) {
    ctx.begin_path();
    ctx.rect(0.0, 0.0, w, h);
    ctx.set_fill_style(&color);
    ctx.fill();
}

#[wasm_bindgen]
pub fn draw_fps(ctx: &CanvasRenderingContext2d, frames: u32, time: f64, color: JsValue ) -> u32 {

    let font_size = 20;
    let fps = js_sys::Math::floor((1000 *(frames+1)) as f64 / time);
    draw_caption(
        &ctx,
        fps.to_string()+" fps".into(),
        0.0,
        font_size as f64, color,
        font_size.to_string()+"px Arial".into()
    );
    return frames+1;
}


#[wasm_bindgen]
pub fn draw_caption(ctx: &CanvasRenderingContext2d, caption: String, x: f64, y: f64, color: JsValue, font: String) {
    ctx.set_fill_style(&color);
    ctx.set_font(&font);
    ctx.fill_text(&caption, x, y).unwrap();
}


#[wasm_bindgen]
pub fn draw_single_pixel(ctx: &CanvasRenderingContext2d, x: f64, y: f64, scale: f64, color: JsValue) {
    ctx.set_global_alpha(0.5);
    ctx.set_fill_style(&color);
    ctx.fill_rect(x, y, scale, scale);
}


fn signal (time: f64, period: f64) -> f64 {
    let _period = period * 1000.0;
    return (time % _period) / _period;
}


#[wasm_bindgen]
pub fn alloc(size: usize) -> *mut c_void {
    let mut buf = Vec::with_capacity(size);
    let ptr = buf.as_mut_ptr();
    mem::forget(buf);
    return ptr as *mut c_void;
}

#[wasm_bindgen]
pub fn dealloc(ptr: *mut c_void, cap: usize) {
    unsafe  {
        let _buf = Vec::from_raw_parts(ptr, 0, cap);
    }
}


#[wasm_bindgen]
pub async fn fetch_text(path: String) -> Result<JsValue, JsValue> {
    let mut opts = RequestInit::new();
    opts.method("GET");
    opts.mode(RequestMode::Cors);
    let request = Request::new_with_str_and_init(&path, &opts)?;
    request.headers().set("Accept", "application/txt")?;
    let window = web_sys::window().unwrap();
    let resp_value = JsFuture::from(window.fetch_with_request(&request)).await?;
    assert!(resp_value.is_instance_of::<Response>());
    let resp: Response = resp_value.dyn_into().unwrap();
    let text = JsFuture::from(resp.text()?).await?;
    Ok(text)
}


#[wasm_bindgen]
pub fn make_vertex_array (series: Vec<f64>) -> Vec<f64> {

    let mut vertices: Vec<f64> = vec![];
    let points: usize = series.len();
    for (ii, value) in series.iter().enumerate() {
        vertices.push( 2.0 * ii as f64 / (points-1) as f64 - 1.0 ); // x
        vertices.push( *value ); // y
    }
    return vertices;
}


pub fn rotate(angle: f32, delta: f32) -> f32 {
    return ((angle + delta) % 360.0) * 2.0*PI/360.0;
}


// Returns a transformation matrix as a flat array with 16 components
pub fn transformation_matrix (ox: f32, oy: f32, oz: f32, rx: f32, ry: f32, rz: f32, s: f32, d: f32, f: f32, n: f32, ar: f32) -> [f32; 16] {

    let cx = rx.cos();
    let sx = rx.sin();
    let cy = ry.cos();
    let sy = ry.sin();
    let cz = rz.cos();
    let sz = rz.sin();
    let a = d;
    let b = (n+f+2.0*d)/(f-n);
    let c = -(d*(2.0*n+2.0*f)+2.0*f*n+2.0*d*d)/(f-n);

    return [
        (cy*cz*s* a)/ar, cy*s*sz* a, -s*sy* b, -s*sy,
        (s*(cz*sx*sy-cx*sz)* a)/ar, s*(sx*sy*sz+cx*cz)* a, cy*s*sx* b, cy*s*sx,
        (s*(sx*sz+cx*cz*sy)* a)/ar, s*(cx*sy*sz-cz*sx)* a, cx*cy*s* b, cx*cy*s,
        (s*(cz*((-oy*sx-cx*oz)*sy-cy*ox)-(oz*sx-cx*oy)*sz)* a)/ar,
        s*(((-oy*sx-cx*oz)*sy-cy*ox)*sz+cz*(oz*sx-cx*oy))* a,
        c +(s*(ox*sy+cy*(-oy*sx-cx*oz))+d)* b, s*(ox*sy+cy*(-oy*sx-cx*oz))+d
    ];
}


pub fn calculate_rotation(ax: f32, ay: f32, az: f32, dx: f32, dy: f32, dz: f32, aspect: f32) -> [f32; 16] {

    let ax = rotate(ax, dx);
    let ay = rotate(ay, dy);
    let az = rotate(az, dz);
    let ox = 0.0;
    let oy = 0.0;
    let oz = 0.0;
    let s = 0.75;
    let d = 3.0;
    let f = 2.0;
    let n = -1.0;

    return transformation_matrix(ox, oy, oz, ax, ay, az, s, d, f, n, aspect);
}


#[wasm_bindgen]
pub fn mouse_move(x: f64, y: f64) {
    web_sys::console::log_1(&format!("{}, {}", x, y).into());
}


