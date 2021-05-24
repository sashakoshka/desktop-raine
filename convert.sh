echo $1
convert "${1}.png" temp.gif
convert temp.gif -background "#FF00FF" -flatten -alpha off -colors 256 "${1}.bmp"
rm temp.gif
rm "${1}.png"
