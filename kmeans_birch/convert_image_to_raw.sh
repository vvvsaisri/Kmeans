#!/bin/bash
# Script to convert JPEG image to raw binary format for kmeans_birch
# Usage: ./convert_image_to_raw.sh <input_image.jpg> [output.raw]

INPUT_IMAGE="${1:-data/test_image.jpg}"
OUTPUT_RAW="${2:-data/test_image.raw}"
WIDTH=493
HEIGHT=612

echo "Converting $INPUT_IMAGE to raw format..."
echo "Output: $OUTPUT_RAW"
echo "Dimensions: ${WIDTH}x${HEIGHT}"

# Check if ImageMagick is available
if command -v convert &> /dev/null; then
    echo "Using ImageMagick convert..."
    convert "$INPUT_IMAGE" -resize ${WIDTH}x${HEIGHT}! -depth 8 rgb:"$OUTPUT_RAW"
    echo "Conversion complete!"
elif command -v ffmpeg &> /dev/null; then
    echo "Using ffmpeg..."
    ffmpeg -i "$INPUT_IMAGE" -vf scale=${WIDTH}:${HEIGHT} -pix_fmt rgb24 -f rawvideo "$OUTPUT_RAW"
    echo "Conversion complete!"
else
    echo "ERROR: Neither ImageMagick nor ffmpeg found!"
    echo "Please install one of them:"
    echo "  sudo apt-get install imagemagick"
    echo "  OR"
    echo "  sudo apt-get install ffmpeg"
    exit 1
fi

# Verify file size
EXPECTED_SIZE=$((WIDTH * HEIGHT * 3))
ACTUAL_SIZE=$(stat -c%s "$OUTPUT_RAW" 2>/dev/null || stat -f%z "$OUTPUT_RAW" 2>/dev/null)
if [ "$ACTUAL_SIZE" -eq "$EXPECTED_SIZE" ]; then
    echo "✓ File size correct: $ACTUAL_SIZE bytes"
else
    echo "✗ WARNING: File size mismatch! Expected: $EXPECTED_SIZE, Got: $ACTUAL_SIZE"
fi

