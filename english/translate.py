#!/usr/bin/python

import sys
import json
import requests
import re

if len(sys.argv) != 2:
    sys.exit( "usage: {cmd} <file name>".format(cmd=sys.argv[0]) )

# read text file
text_file = open(sys.argv[1], "r")

file_str = text_file.read()

# split it into sentences
sentences = re.split('\.|\n\n', file_str)
#sentences = file_str.split(".")

for sentence in sentences:
    sentence = sentence.replace("\n", "").strip()

    if len(sentence) == 0:
        continue

    url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=ru&dt=t&q={text}"\
        .format(text=sentence)

    resp = requests.get(url=url)
    data = json.loads(resp.text)

    print(sentence)
    print(data[0][0][0])
    print('\n')

