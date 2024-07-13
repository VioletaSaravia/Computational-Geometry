from PIL import Image
import fontforge


def extract_tiles(image_path: str, tile_size: int = 8):
    image = Image.open(image_path)
    width, height = image.size
    tiles = []

    for y in range(0, height, tile_size):
        for x in range(0, width, tile_size):
            tile = image.crop((x, y, x + tile_size, y + tile_size))
            tiles.append(tile)

    return tiles


def create_font_from_tiles(tiles, output_path: str = "custom_font.ttf"):
    font = fontforge.font()
    font.encoding = "ISO8859-1"

    for i, tile in enumerate(tiles):
        glyph = font.createChar(i)
        pen = glyph.glyphPen()

        for y in range(tile.height):
            for x in range(tile.width):
                if tile.getpixel((x, y)) != (
                    0,
                    0,
                    0,
                    0,
                ):  # Assuming the background is transparent
                    pen.moveTo((x, y))
                    pen.lineTo((x + 1, y))
                    pen.lineTo((x + 1, y + 1))
                    pen.lineTo((x, y + 1))
                    pen.closePath()

    font.generate(output_path)


tilesets = [
    ("assets/tilesets/microbe-2.png", 6),
    ("assets/tilesets/MRMOTEXT-EX.png", 8),
    ("assets/tilesets/MRMOTEXT-x3.png", 8 * 3),
]

extracted = [extract_tiles(path, tile_width) for path, tile_width in tilesets]

[
    create_font_from_tiles(tiles, f"{file}.ttf")
    for (file, _), tiles in zip(tilesets, extracted)
]
