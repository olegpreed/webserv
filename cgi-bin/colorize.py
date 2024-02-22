#!/usr/bin/env python3

import cgi
import random

# HTML template
html_template = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Formatted Text</title>
    <style>
        .random-color {{
            display: inline-block;
            color: rgb({}, {}, {});
        }}
    </style>
</head>
<body>
    <h1>{}</h1>
</body>
</html>
"""

# CGI headers
print("Content-type:text/html\r\n\r\n")

# Get input from form
form = cgi.FieldStorage()
if "text" in form:
    input_text = form.getvalue("text")
else:
    input_text = ""

# Convert text to uppercase
uppercase_text = input_text.upper()

# Generate HTML with formatted text
formatted_text = ""
for char in uppercase_text:
    # Generate random RGB values for color
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)
    # Create span element with random color for each character
    formatted_text += '<span class="random-color">{}</span>'.format(char)

# Print the formatted HTML page
print(html_template.format(r, g, b, formatted_text))
