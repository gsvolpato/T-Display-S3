import os
from PIL import Image
import numpy as np

def convert_png_to_lvgl(png_path, output_dir):
    # Load the PNG image
    img = Image.open(png_path)
    
    # Convert to RGBA if not already
    if img.mode != 'RGBA':
        img = img.convert('RGBA')
    
    # Get image data as numpy array
    img_data = np.array(img)
    
    # Get base filename without extension
    base_name = os.path.splitext(os.path.basename(png_path))[0]
    
    # Create output C file path
    c_file_path = os.path.join(output_dir, f'{base_name}.c')
    
    with open(c_file_path, 'w') as f:
        # Write header
        f.write('#include "lvgl.h"\n\n')
        
        # Write image data array
        f.write(f'const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t {base_name}_map[] = {{\n')
        
        # Convert each pixel to LVGL format
        for y in range(img.height):
            f.write('    ')
            for x in range(img.width):
                r, g, b, a = img_data[y, x]
                # LVGL uses 16-bit color (RGB565) + 8-bit alpha
                # Convert RGB to RGB565 format
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                # Write as three bytes (RGB565 + alpha)
                f.write(f'0x{(rgb565 & 0xFF):02x}, ')
                f.write(f'0x{((rgb565 >> 8) & 0xFF):02x}, ')
                f.write(f'0x{a:02x}, ')
            f.write('\n')
        
        f.write('};\n\n')
        
        # Write LVGL image descriptor
        f.write(f'const lv_img_dsc_t {base_name} = {{\n')
        f.write('    .header.always_zero = 0,\n')
        f.write(f'    .header.w = {img.width},\n')
        f.write(f'    .header.h = {img.height},\n')
        f.write(f'    .data_size = {img.width * img.height * 3},\n')
        f.write('    .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,\n')
        f.write(f'    .data = {base_name}_map,\n')
        f.write('};\n')
    
    print(f'Converted {png_path} to {c_file_path}')

# List of PNG files to convert
png_files = [
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\bluetooth.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\folder.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\github.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\power.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\radio.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\server.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\settings.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\sun.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\tv.png',
    r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\png\wifi.png'
]

# Output directory for C files (use the src/icons directory)
output_dir = r'C:\Users\Giovane\Documents\PlatformIO\Projects\S3-Factory-Clock - Copy\src\icons'

# Create output directory if it doesn't exist
os.makedirs(output_dir, exist_ok=True)

# Convert each PNG file
for png_file in png_files:
    convert_png_to_lvgl(png_file, output_dir) 