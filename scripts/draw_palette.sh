#!/bin/bash

# Output image parameters
box_width=800
box_height=100
margin=10
text_size=40
current_height=0
output_image="output_palette.png"
FONT=Arial

# Check if "magick" command is available
if ! command -v magick &> /dev/null; then
    echo "Error: ImageMagick ('magick' command) is not installed or not in the PATH."
    exit 1
fi

# Check if input file is provided and exists
if [ -z "$1" ] || [ ! -f "$1" ]; then
    echo "Error: Input file not provided or does not exist."
    exit 1
fi

# Read the header and validate it
header=$(head -1 "$1")
expected_header="number,name,r,g,b,lower,upper"
if [ "$header" != "$expected_header" ]; then
    echo "Error: Input file header does not match expected format."
    exit 1
fi

# Validate each line for correct number of elements
while IFS=, read -r number name r g b lower upper; do
    if [ -z "$upper" ] || [ ! -z "$(cut -d',' -f8- <<<"$REPLY")" ]; then
        echo "Error: Line does not have the correct number of elements: $REPLY"
        exit 1
    fi
done < <(tail -n +2 "$1")  # Skip header line

# Read the palette file and skip the header line
tail -n +2 "$1" | while IFS=, read -r number name r g b lower upper
do
  # Calculate color in hex format
  hex_color=$(printf "#%02x%02x%02x" "$r" "$g" "$b")
  
  # Create a temporary image for this line
  temp_image=$(mktemp /tmp/palette_line.XXXXXX.png)

  label="${number}:${name} (${r},${g},${b}) [${lower},${upper}]"

  # Draw the box with the specified fill color and black border
  magick -size ${box_width}x${box_height} canvas:none \
    -fill "$hex_color" -draw "rectangle 0,0 $box_width,$box_height" \
    -stroke black -strokewidth 1 -draw "rectangle 0,0 $box_width,$box_height" \
    -font $FONT -pointsize $text_size -fill black -stroke none -gravity center \
    -annotate +0+2 "$label" \
    "$temp_image"
  
  # Composite the line onto the output image
  if [ $current_height -eq 0 ]; then
    # First line, just rename the temp image to our output
      mv "$temp_image" "$output_image"
  else
      # Append the temp image below the current output image
      magick "$output_image" "$temp_image" -append "$output_image"
      # Remove the temporary line image
      rm "$temp_image"
  fi
  
  # Update the current height of the output image
  current_height=$((current_height + box_height + margin))
  
done
