# Converts a 256x64 image to assembly (db) font data.

import sys
# Python Imaging Library
# http://www.pythonware.com/products/pil/
import Image

if len(sys.argv) != 2:
	print >>sys.stderr, 'Usage: python convert_font.py <image_file>'
	sys.exit(2)
else:
	fileName = sys.argv[1]

image = Image.open(fileName)

# Check size.
assert image.size == (256, 64)

# Convert to monochrome.
image = image.convert('L').point(lambda i: i >= 128 and 255)

# Output assembly.
print 'B_Font:'
for char in range(256):
	bx = (char % 32) * 8
	by = (char / 32) * 8
	pattern = []
	for y in range(by, by + 8):
		byte = 0
		for x in range(bx, bx + 8):
			byte <<= 1
			if image.getpixel( (x, y) ):
				byte |= 1
		pattern.append(byte)
	print ' ' * 16 + 'db' + ' ' * 6 + ','.join([
		'$%02X' % byte for byte in pattern
		]) + ' ' * 9 + '; ' + str(char)

