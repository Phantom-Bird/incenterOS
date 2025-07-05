import struct

def show(value):
    for i in range(8):
        for j in range(8):
            print('#' if value & (1 << (i * 8 + j)) else ' ', end='')
        print()

def reverse_bits(int64_value):
    return int('{:064b}'.format(int64_value)[::-1], 2)

def load_psf_file(filename):
    with open(filename, "rb") as f:
        header = f.read(4)

        if header.startswith(b'\x36\x04'):  # PSF v1 magic
            mode = 'PSF1'
            f.seek(0)
            magic, mode, charsize = struct.unpack("2sBB", f.read(4))
            glyphs = 256 if not (mode & 0x01) else 512
            glyph_height = charsize
        elif header.startswith(b'\x72\xb5\x4a\x86'):  # PSF v2 magic
            mode = 'PSF2'
            f.seek(0)
            magic, version, headersize, flags, glyph_count, glyph_size, height, width = struct.unpack("4sIIIIIII", f.read(32))
            glyphs = glyph_count
            glyph_height = height
        else:
            raise ValueError("Unsupported PSF format")

        glyph_data = []
        for _ in range(glyphs):
            raw = f.read(glyph_height)
            value = 0
            for b in raw:
                value = (value << 8) | b

            value = reverse_bits(value)
            glyph_data.append(value)

            if 32 < _ < 127:
                show(value)

        return glyph_data, glyph_height, mode

def write_c_array(filename, glyph_data, height):
    with open(filename, "w") as f:
        f.write("// Generated font data from PSF\n")
        f.write(f"// Each glyph is {height} rows tall\n")
        f.write("unsigned long long ascii[] = {\n")
        for i, val in enumerate(glyph_data):
            f.write(f"    0x{val:016X}ULL")
            if i != len(glyph_data) - 1:
                f.write(",")
            f.write(f" // char {chr(i) if 32<i<127 else i}\n")
        f.write("};\n")

# 示例使用
glyph_data, height, mode = load_psf_file("Arabic-VGA8.psf")
write_c_array("ascii_font.c", glyph_data, height)
