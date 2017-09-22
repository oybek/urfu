#!/usr/bin/python

import sys
import json
import requests

if len(sys.argv) != 2:
    sys.exit( "usage: {cmd} \"english text\"".format(cmd=sys.argv[0]) )

url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=en&tl=ru&dt=t&q={text}"\
    .format(text=sys.argv[1])

resp = requests.get(url=url)
data = json.loads(resp.text)

print(data[0][0][0])

