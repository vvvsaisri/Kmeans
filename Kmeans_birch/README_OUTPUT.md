# K-means BIRCH Output Files Location

## About `/run/media/mmcblk0p1`

This directory is a **mounted storage device** (SD card or USB drive) on your Zynq embedded system. When you run the program, it executes from this mounted location, which is why the output files are saved there.

## Output File Locations

The program now tries to save files in this order:
1. **`$HOME/kmeans_birch_output/`** (if writable) - Recommended location
2. **Current working directory** (where program runs from) - Usually `/run/media/mmcblk0p1`

## Using Your Test Image

The program is looking for a **raw binary file**, not a JPEG. You need to convert your `test_image.jpg` first.

### Option 1: Use the conversion script (on host PC)

```bash
cd /home/ramu/workspace/kmeans_birch
./convert_image_to_raw.sh /home/ramu/Downloads/test_image.jpg data/test_image.raw
```

Then copy `data/test_image.raw` to your embedded device in the same location as the executable.

### Option 2: Convert manually

```bash
# Using ImageMagick
convert /home/ramu/Downloads/test_image.jpg -resize 493x612! -depth 8 rgb:data/test_image.raw

# OR using ffmpeg
ffmpeg -i /home/ramu/Downloads/test_image.jpg -vf scale=493:612 -pix_fmt rgb24 -f rawvideo data/test_image.raw
```

### Option 3: Copy raw file to embedded device

After converting, copy the file to your embedded device:
```bash
# Copy to SD card or wherever your executable is
scp data/test_image.raw user@zynq:/run/media/mmcblk0p1/data/
```

## Image File Search Locations

The program searches for `test_image.raw` in these locations (in order):
1. `data/test_image.raw` (relative to executable)
2. `../data/test_image.raw`
3. `../../data/test_image.raw`
4. `/home/ramu/workspace/kmeans_birch/data/test_image.raw`
5. `test_image.raw` (current directory)

## Output Files

After running, you'll find:
- `kmeans_output.ppm` - K-means clustered image (viewable)
- `birch_output.ppm` - BIRCH clustered image (viewable)
- `kmeans_output.raw` - Raw binary data
- `birch_output.raw` - Raw binary data

These will be in either:
- `$HOME/kmeans_birch_output/` (if available)
- Current working directory (usually `/run/media/mmcblk0p1`)

