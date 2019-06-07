#!/bin/bash

filename=$1
folder=${filename%.*}

mkdir ${folder}

ffmpeg -i ${filename} -f s16le -c:a pcm_s16le -ac 1 -ar 48000 ${folder}/audio.raw
#aplay -c 1 -f S16_LE -r48000 ${folder}/audio.raw 

ffmpeg -i ${filename} -vf fps=12.5 ${folder}/img%06d.png
