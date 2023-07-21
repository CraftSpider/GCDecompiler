use std::error::Error;
use std::{fs, io};
use std::path::Path;
use png::Encoder;
use gcd_parser::error::ParseError;
use gcd_parser::Reader;
use gcd_parser::file::ParseFile;

mod parser;

#[derive(Debug)]
pub enum TplError {
    Cause(/*Backtrace, */Box<dyn Error>),
    Custom(String),
}

impl From<ParseError> for TplError {
    #[track_caller]
    fn from(value: ParseError) -> Self {
        TplError::Cause(/*Backtrace::capture(), */Box::new(value))
    }
}

impl From<io::Error> for TplError {
    #[track_caller]
    fn from(value: io::Error) -> Self {
        TplError::Cause(/*Backtrace::capture(), */Box::new(value))
    }
}

pub enum TplKind {
    Gc,
    Wii,
    XBox,
    Headerless,
}

impl TplKind {
    fn guess_kind<P: AsRef<Path>>(input: P) -> Result<TplKind, TplError> {
        let file = std::fs::File::open(input)?;
        
        Err(TplError::Custom(String::from("TODO")))
    }
}

#[derive(Clone, Copy, Debug)]
#[non_exhaustive]
pub enum Format {
    I4 = 0,
    I8 = 1,
    IA4 = 2,
    IA8 = 3,
    RGB565 = 4,
    RGB5A3 = 5,
    RGBA32 = 6,
    C4 = 8,
    C8 = 9,
    C14x2 = 10,
    CMPR = 14,
}

impl Format {
    fn from_u32(val: u32) -> Option<Format> {
        Some(match val {
            0 => Format::I4,
            1 => Format::I8,
            2 => Format::IA4,
            3 => Format::IA8,
            4 => Format::RGB565,
            5 => Format::RGB5A3,
            6 => Format::RGBA32,
            8 => Format::C4,
            9 => Format::C8,
            10 => Format::C14x2,
            14 => Format::CMPR,
            _ => return None,
        })
    }

    /// Returns the number of bits per pixel of this format
    fn bits(&self) -> usize {
        match self {
            Format::I4 => 4,
            Format::I8 => 8,
            Format::IA4 => 8,
            Format::IA8 => 16,
            Format::RGB565 => 16,
            Format::RGB5A3 => 16,
            Format::RGBA32 => 32,
            Format::C4 => 4,
            Format::C8 => 8,
            Format::C14x2 => 16,
            Format::CMPR => 4,
        }
    }

    /// Returns the height and width, in pixels, of a given chunk for this format
    fn height_width(&self) -> (usize, usize) {
        match self {
            Format::I4 => (8, 8),
            Format::I8 => (4, 8),
            Format::IA4 => (4, 8),
            Format::IA8 => (4, 4),
            Format::RGB565 => (4, 4),
            Format::RGB5A3 => (4, 4),
            Format::RGBA32 => (4, 4),
            Format::C4 => (8, 8),
            Format::C8 => (4, 8),
            Format::C14x2 => (4, 4),
            Format::CMPR => (8, 8),
        }
    }
}

#[derive(Clone, Copy, PartialEq)]
pub struct Color {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
}

impl Color {
    fn new_tone(tone: u8) -> Color {
        Color { r: tone, g: tone, b: tone, a: 255 }
    }
    
    fn new_rgb(r: u8, g: u8, b: u8) -> Color {
        Color { r, g, b, a: 255 }
    }
    
    fn new_rgba(r: u8, g: u8, b: u8, a: u8) -> Color {
        Color { r, g, b, a }
    }
    
    fn lerp_colors(left: Color, right: Color, factor: f32) -> Color {
        let Color { r: lr, g: lg, b: lb, a: la } = left;
        let Color { r: rr, g: rg, b: rb, a: ra } = right;
        let r = lr as f32 + (rr as f32 - lr as f32) * factor;
        let g = lg as f32 + (rg as f32 - lg as f32) * factor;
        let b = lb as f32 + (rb as f32 - lb as f32) * factor;
        let a = la as f32 + (ra as f32 - la as f32) * factor;
        Color::new_rgba(r as u8, g as u8, b as u8, a as u8)
    }
}

#[derive(Clone)]
pub struct Image {
    data: Vec<Color>,
    format: Format,
    height: usize,
    width: usize,
}

impl Image {
    pub fn write_png<P: AsRef<Path>>(&self, path: P) -> Result<(), png::EncodingError> {
        let out = fs::File::create(path)?;
        let mut encoder = Encoder::new(out, self.width as u32, self.height as u32);
        
        encoder.set_color(png::ColorType::Rgba);
        encoder.set_depth(png::BitDepth::Eight);
        
        let mut writer = encoder.write_header()?;
        let data = self.data.iter()
            .flat_map(|c| [c.r, c.g, c.b, c.a])
            .collect::<Vec<_>>();
        writer.write_image_data(&data)?;
        
        Ok(())
    } 
    
    pub fn height(&self) -> usize {
        self.height
    }
    
    pub fn width(&self) -> usize {
        self.width
    }
    
    pub fn format(&self) -> Format {
        self.format
    }
}

#[derive(Clone)]
pub struct MipMapImage {
    images: Vec<Image>,
}

impl MipMapImage {
    pub fn images(&self) -> &[Image] {
        &self.images
    }
}

pub struct Tpl {
    images: Vec<MipMapImage>,
}

impl Tpl {
    pub fn read<P: AsRef<Path>>(input: P, kind: TplKind) -> Result<Tpl, TplError> {
        let tpl = match kind {
            TplKind::Gc => parser::gc_parser()
                .parse(ParseFile::open(input)?)?,
            TplKind::Wii => todo!(),
            TplKind::XBox => todo!(),
            TplKind::Headerless => todo!(),
        };
        Ok(tpl)
    }
    
    pub fn mipmaps(&self) -> &[MipMapImage] {
        &self.images
    }
}
