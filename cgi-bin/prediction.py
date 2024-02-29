#!/usr/local/bin/python3.10

# import cgi
import os
import requests
import json

# Function to send GET request to the API
def get_horoscope(date, lang):
    url = f"https://newastro.vercel.app/libra/?date={date}&lang={lang}"
    headers = {"accept": "application/json"}
    response = requests.get(url, headers=headers)
    return response.json()

# Main CGI script
print("Content-Type: text/html\r\n\r\n")

# form = cgi.FieldStorage()

# Get date and language from the query parameters
# date = form.getvalue('date')
# lang = form.getvalue('lang')

query_string = os.getenv('QUERY_STRING')
lang = 'en'

if query_string:
	params = query_string.split('&')
	dob = None
	for param in params:
		key, value = param.split('=')
		if key == 'dob':
			dob = value
			break
    
	if dob:
		# Send GET request to the API
		try:
			horoscope_data = get_horoscope(dob, lang)
			horoscope = horoscope_data.get('horoscope', '')
			icon_url = horoscope_data.get('icon', '')

			# Print HTML with horoscope output
			print(f"""
				<html>
				<head>
					<title>Horoscope</title>
				</head>
				<body>
					<h1>Your Horoscope Prediction:</h1>
					<p>Date: {date}</p>
					<p>Horoscope: {horoscope}</p>
					<img src="{icon_url}" alt="Zodiac Icon">
				</body>
				</html>
			""")
		except Exception as e:
			print(f"<h1>Error: {e}</h1>")
else:
    print("<h1>Error: Date or language not provided</h1>")
