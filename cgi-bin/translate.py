# !/usr/bin/env python3.10

# install googletans

# pip3 install googletrans==4.0.0-rc1 
# pip3 install certifi

import cgi
import os
import sys

site_packages_path = '/Users/preed/Library/Python/3.11/lib/python/site-packages'
site_packages_path2 = '/usr/local/lib/python3.11/site-packages'
sys.path.append(site_packages_path)
sys.path.append(site_packages_path2)

from googletrans import Translator
def translate_text(text, dest_language='th'):
    translator = Translator()
    translated = translator.translate(text, dest=dest_language)
    return translated.text

def main():
    # Parse form data
    form = cgi.FieldStorage()

    # Get the English text from the form
    english_text = form.getvalue("text")

    # Translate the English text to Thai
    thai_text = translate_text(english_text)

    # CGI response
    print("Content-type: text/html\n")
    print("<html><head><title>Translation Result</title></head><body>")
    print("<h1>Translation Result</h1>")
    print("<p><strong>English Text:</strong> {}</p>".format(english_text))
    print("<p><strong>Thai Translation:</strong> {}</p>".format(thai_text))
    print("</body></html>")

if __name__ == "__main__":
    main()