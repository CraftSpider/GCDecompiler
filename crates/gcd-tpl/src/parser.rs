use bitvec::field::BitField;
use bitvec::order::Msb0;
use bitvec::slice::BitSlice;
use gcd_parser::{Reader, Action};
use gcd_parser::primitives::*;
use gcd_utils::Endian;
use itertools::Itertools;
use crate::{Color, Format, Image, MipMapImage, Tpl};

#[derive(Copy, Clone)]
struct NumImages(u32);

#[derive(Clone)]
struct GCImageTableEntry {
    format: u32,
    offset: u32,
    width: u16,
    height: u16,
    mipmaps: u16,
}

fn parse_i4(bytes: &[u8], pixel: usize) -> Color {
    let data = bytes[pixel / 2];
    let which = pixel % 2 == 0;
    let tone = (data >> (4 * which as usize)) & 0xF * 11;
    Color::new_tone(tone)
}

fn parse_i8(bytes: &[u8], pixel: usize) -> Color {
    Color::new_tone(bytes[pixel])
}

fn parse_rgb565(bytes: &[u8], pixel: usize) -> Color {
    let start = pixel * 2;
    let rgb = BitSlice::<_, Msb0>::from_slice(&bytes[start..=start+1]);
    let red = 0x8 * rgb.get(0..5).unwrap().load_be::<u8>();
    let green = 0x4 * rgb.get(5..11).unwrap().load_be::<u8>();
    let blue = 0x8 * rgb.get(11..16).unwrap().load_be::<u8>();
    Color::new_rgb(red, green, blue)
}

fn parse_cmpr(bytes: &[u8], pixel: usize, palettes: &[[Color; 4]; 4]) -> Color {
    let extra = if pixel > 31 { 2 } else { 0 };
    let palette_idx = if pixel % 8 > 3 { 1 } else { 0 } + extra;
    let palette = palettes[palette_idx];
    
    let bl_pix = if pixel % 8 > 3 {
        (pixel % 32 - 4) / 8 * 4
    } else {
        (pixel % 32) / 8 * 4
    };
    
    let index = bytes[(palette_idx + 4) + (bl_pix / 4)];
    let which = 3 - (bl_pix % 4);
    let index = (index >> 2 * which) & 0x3;
    palette[index as usize]
}

fn cmpr_palette(bytes: &[u8]) -> [[Color; 4]; 4] {
    let mut out = [[Color::new_tone(0); 4]; 4];
    
    for palette in 0..4 {
        let palette_start = palette * 8;
        
        let first = parse_rgb565(bytes, palette_start / 2);
        let second = parse_rgb565(bytes, (palette_start + 2) / 2);

        let first_raw = BitSlice::<_, Msb0>::from_slice(&bytes[palette_start..=palette_start+1])
            .load_be::<u16>();
        let second_raw = BitSlice::<_, Msb0>::from_slice(&bytes[palette_start+2..=palette_start+3])
            .load_be::<u16>();

        let lerp_1;
        let lerp_2;
        if first_raw >= second_raw {
            lerp_1 = Color::lerp_colors(first, second, 1./3.);
            lerp_2 = Color::lerp_colors(first, second, 2./3.);
        } else {
            lerp_1 = Color::lerp_colors(first, second, 0.5);
            lerp_2 = Color::new_rgba(0, 0, 0, 0);
        }

        out[palette] = [
            first,
            second,
            lerp_1,
            lerp_2,
        ];
    }
    out
}

fn decode_image_data(bytes: &[u8], format: Format, height: usize, width: usize) -> Image {
    // println!("Decoding Image: {:?} {}/{}", format, height, width);
    
    let (h, w) = format.height_width();
    let chunk_pixels = h * w;
    let block_size = chunk_pixels * format.bits() / 8;
    
    // println!("  Chunk Pixels: {}", chunk_pixels);
    // println!("  Block Size: {}", block_size);
    
    let width_chunks = width / w;
    let height_chunks = height / h;
    
    // println!("  Chunks: {}/{}", height_chunks, width_chunks);
    
    let mut image = Image {
        data: vec![Color::new_rgb(0, 0, 0); height * width],
        height,
        width,
        format,
    };
    
    // println!("  Byte Len: {}", bytes.len());
    
    bytes.chunks(block_size)
        .enumerate()
        .for_each(|(chunk, data)| {
            // println!("    Chunk {}", chunk);
            
            let w_pos = chunk % width_chunks;
            let h_pos = chunk / width_chunks;
            
            // println!("    Chunk H/W: {}/{}", h_pos, w_pos);
            
            let w_pixels = if w_pos == width_chunks-1 {
                width % w
            } else {
                w
            };
            
            let h_pixels = if h_pos == height_chunks-1 {
                height % h
            } else {
                h
            };
            
            // println!("Block Size: {}:{}", h_pixels, w_pixels);
            
            let top = h_pos * h;
            let left = w_pos * w;
            
            let palettes: [[Color; 4]; 4] = if let Format::CMPR = format {
                cmpr_palette(bytes)
            } else {
                [[Color::new_tone(0); 4]; 4]
            };
            
            (top..top+h_pixels)
                .cartesian_product(left..left+w_pixels)
                .enumerate()
                .for_each(|(idx, (h, w))| {
                    image.data[h + w * height] = match format {
                        Format::I4 => parse_i4(data, idx),
                        Format::I8 => parse_i8(data, idx),
                        Format::RGB565 => parse_rgb565(data, idx),
                        Format::CMPR => parse_cmpr(data, idx, &palettes),
                        // TODO
                        _ => Color::new_tone(0),
                    };
                });
        });
    
    image
}

pub fn gc_parser() -> impl Reader<Output = Tpl> {
    let num_images = u32(Endian::Big).map(NumImages);
    
    let table_entry = group((
        u32(Endian::Big),
        u32(Endian::Big),
        u16(Endian::Big),
        u16(Endian::Big),
        u16(Endian::Big),
        magic([0x12, 0x34]),
    )).map(|(format, offset, width, height, mipmaps, _)| {
        GCImageTableEntry {
            format,
            offset,
            width,
            height,
            mipmaps,
        }
    });
    
    let image = u8()
        .repeated()
        .exactly(current::<GCImageTableEntry>().map(|i| {
            let format = Format::from_u32(i.format).unwrap();
            let (h, w) = format.height_width();
            let num_pixels = h * w;
            let block_size = num_pixels * format.bits() / 8;
            block_size * i.height as usize / h * i.width as usize / w
        }))
        .map_with(
            current::<GCImageTableEntry>(),
            |bytes, entry| decode_image_data(
                &bytes,
                Format::from_u32(entry.format).unwrap(),
                entry.height as usize,
                entry.width as usize,
            )
        );
    
    // Read the header, and store a Vec<GcImageTableEntry>
    let header = num_images
        .memorize()
        .and(
            table_entry
                .repeated()
                .exactly(memorized::<NumImages>().map(|ni| ni.0 as usize))
                .memorize()
        );
    
    // For each header, jump to offset, read N images and decode them
    let images = header
        .and(
            jump_read(
                current::<GCImageTableEntry>().map(|i| i.offset as usize),
                image
                    .repeated()
                    .exactly(current::<GCImageTableEntry>().map(|i| i.mipmaps as usize))
                    .map(|images| MipMapImage { images })
            )
                .repeated()
                .for_each(memorized::<Vec<GCImageTableEntry>>().cloned())
                .memorize()
        );
        
    images.retrieve::<Tpl, _>(memorized::<Vec<MipMapImage>>().map(|images| Tpl { images: images.clone() }))
}
