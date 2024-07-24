from urllib.request import urlopen, Request
from urllib.error import HTTPError, URLError

def make_request(url, headers=None):
    request = Request(url, headers or {})
    try:
        with urlopen(request, timeout=10) as response:
            print(response.status)
            return response.read(), response
    except HTTPError as error:
        print(error.status, error.reason)
    except URLError as error:
        print(error.reason)
    except TimeoutError:
        print("request timed out")
import urllib.parse
URL = urllib.parse.urlencode("localhost:4242/index.html")
HEADERS = {"celine":"Dion", "garou":"Loup"}
BODY = "Sous le vent"

make_request(URL, HEADERS)
