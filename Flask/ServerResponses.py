#####################################################################
#                                                                   #
#   `ServerResponses.py` is an index of all server responses for    #
#   the KIP API.                                                    #
#   This software is for educational purposes.                      #
#                                                                   #
#   License: https://www.mozilla.org/en-US/MPL/                     #
#   Written by GowanR <g@gmichaelrowland.com>                       #
#                                                                   #
#####################################################################


import json

SUCCESS = json.dumps({'message':'success', 200, {'Content-Type':'application/json; charset=utf-8'}
BAD_QUERYSTRING = json.dumps({'message':'incorrect querystrings'}), 202, {'Content-Type': 'application/json; charset=utf-8'}