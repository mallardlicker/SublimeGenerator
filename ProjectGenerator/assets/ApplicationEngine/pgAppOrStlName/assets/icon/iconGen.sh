# iconGen.sh
#
# Will generate .ico and .icns for both Windows and Mac distributions of an application.
#  The generated png files will not be deleted, in case modification of them (and thus 
#  this script) is required to allow for user-created images (rather than magick resized).
# 
# Author: Justin Bunting
# Created: 2025/10/06
# Last Modified: 2025/10/06 11:50

# ! Generate iconset directory with all sizes
mkdir -p icon.iconset

# Generate all required sizes from source
magick icon.png -resize 16x16 icon.iconset/icon_16x16.png
magick icon.png -resize 32x32 icon.iconset/icon_16x16@2x.png
magick icon.png -resize 32x32 icon.iconset/icon_32x32.png
magick icon.png -resize 64x64 icon.iconset/icon_32x32@2x.png
magick icon.png -resize 128x128 icon.iconset/icon_128x128.png
magick icon.png -resize 256x256 icon.iconset/icon_128x128@2x.png
magick icon.png -resize 256x256 icon.iconset/icon_256x256.png
magick icon.png -resize 512x512 icon.iconset/icon_256x256@2x.png
magick icon.png -resize 512x512 icon.iconset/icon_512x512.png
magick icon.png -resize 1024x1024 icon.iconset/icon_512x512@2x.png

# ! Create MacOS .icns from iconset
iconutil -c icns icon.iconset

# ! Create Windows .ico from iconset files
magick \
  icon.iconset/icon_16x16.png \
  icon.iconset/icon_32x32.png \
  icon.iconset/icon_32x32@2x.png \
  icon.iconset/icon_128x128.png \
  icon.iconset/icon_256x256.png \
  icon.iconset/icon_512x512.png \
  icon.ico

# Clean up iconset directory
# rm -rf icon.iconset

echo "Generated icon.ico and icon.icns from iconset"