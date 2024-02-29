#!/usr/local/bin/python3.9

# install googletans
# pip3.9 install googletrans==4.0.0-rc1 
# pip3.9 install certifi

import googletrans
import cgi

html_template = """
<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>webserv</title>
	<link rel="stylesheet" href="styles.css">
</head>

<body>
	<div class="header">
		<div class="project-name">WEBSERV</div>
		<div class="logo"><a href="/">
				<img alt="Home School 42" src="https://42.fr/wp-content/uploads/2021/05/42-Final-sigle-seul.svg">
			</a></div>
	</div>
	<div class="content">
		<form id="myForm" action="/cgi-magic" method="POST">
			<div class="custom-input">
				<textarea class="text-area" class="input-text" placeholder="Enter your text here..."
					name="text">{}</textarea>
			</div>
			<button type="submit">Translate</button>
			<div class="text-area output">{}
			</div>
		</form>
	</div>
</body>

</html>
"""

from googletrans import Translator
def translate_text(text, dest_language='th'):
    translator = googletrans.Translator()
    translated = translator.translate(text, dest=dest_language)
    return translated.text

# Parse form data
form = cgi.FieldStorage()

# Get the English text from the form
if "text" in form:
	src_txt = form.getvalue("text")
else:
	src_txt = ""

# Translate the English text to Thai
thai_text = translate_text(src_txt)

# CGI response
print("Content-type: text/html\r\n\r\n")
print(html_template.format(src_txt, thai_text))