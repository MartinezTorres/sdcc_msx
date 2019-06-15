# User editable settings:
fileName = 'cbios-logo.png'
logoArea = (40, 40, 216, 104)

# Python Imaging Library
# http://www.pythonware.com/products/pil/
import Image

image = Image.open(fileName)

# Limit to area of interest.
logo = image.crop(logoArea)

# Check remaining size.
assert logo.size[0] <= 256
assert logo.size[1] <= 192
assert logo.size[0] % 8 == 0
assert logo.size[1] % 8 == 0

# Get R/G/B bands.
if logo.mode != 'RGB': logo = logo.convert('RGB')
redBand, greenBand, blueBand = logo.split()
# Create monochrome image from blue band.
# Note: This just happens to be valid for our current logo.
logo = blueBand.point(lambda i: i >= 128 and 255)

# Convert pixels to blocks.
blocks = {}
patternTable = []
nameTable = []
for blockY in range(logo.size[1] / 8):
	for blockX in range(logo.size[0] / 8):
		block = []
		for y in range(blockY * 8, (blockY + 1) * 8):
			pattern = 0
			for x in range(blockX * 8, (blockX + 1) * 8):
				pattern <<= 1
				if logo.getpixel( (x, y) ):
					pattern |= 1
			block.append(pattern)
		block = tuple(block)
		blockNr = blocks.get(block)
		if blockNr is None:
			blockNr = len(blocks)
			blocks[block] = blockNr
			patternTable.append(block)
		nameTable.append(blockNr)

patternOffset = 131
assert patternOffset + len(patternTable) < 256
print 'logo_patoffset: equ     %d' % patternOffset
print 'logo_npatterns: equ     %d' % len(patternTable)
print 'logo_patterns:'
for block in patternTable:
	print ' ' * 16 + 'db' + ' ' * 6 + ','.join([
		'$%02X' % line for line in block
		])
print

print 'logo_namwidth:  equ     %d' % (logo.size[0] / 8)
print 'logo_namheight: equ     %d' % (logo.size[1] / 8)
print 'logo_namoffset: equ     %d' % (
	(logoArea[0] / 8) + 32 * (logoArea[1] / 8)
	)
print 'logo_names:'
nameIndex = 0
for blockY in range(logo.size[1] / 8):
	row = []
	for blockX in range(logo.size[0] / 8):
		row.append(nameTable[nameIndex])
		nameIndex += 1
	print ' ' * 16 + 'db' + ' ' * 6 + ','.join([
		'$%02X' % (patternOffset + item) for item in row
		])
print

blackness = Image.new('L', redBand.size)
sprite = Image.composite(blackness, redBand, blueBand)
sprite = sprite.point(lambda i: i >= 128 and 255)
spriteBox = sprite.getbbox()

sprite1 = Image.composite(blackness, greenBand, blueBand)
sprite1 = sprite1.point(lambda i: i >= 128 and 255)
sprite1Box = sprite1.getbbox()

sprite = sprite.crop(spriteBox)
assert sprite.size[0] < 16
assert sprite.size[1] < 16
spritePattern = []
for col in range(2):
	for y in range(16):
		pattern = 0
		for colX in range(8):
			pattern <<= 1
			x = col * 8 + colX
			if x < sprite.size[0] and y < sprite.size[1] \
			and sprite.getpixel( (x, y) ):
				pattern |= 1
		spritePattern.append(pattern)

print 'logo_spritepat:'
for line in range(0, len(spritePattern), 16):
	print ' ' * 16 + 'db' + ' ' * 6 + ','.join([
		'$%02X' % pattern for pattern in spritePattern[line : line + 16]
		])
print

spriteAttr = [
	logoArea[1] + spriteBox[1], # Y
	logoArea[0] + spriteBox[0], # X
	0, # pattern
	8, # colour: red
	logoArea[1] + sprite1Box[1], # Y
	logoArea[0] + sprite1Box[0], # X
	0, # pattern
	2, # colour: green
	]
print 'logo_spriteattr:'
print ' ' * 16 + 'db' + ' ' * 6 + ','.join([
		'$%02X' % byte for byte in spriteAttr
		])

