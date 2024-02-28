#!/usr/local/bin/python3.10

# import os
import requests

# Function to get the horoscope sign based on the date of birth

url = "https://sameer-kumar-aztro-v1.p.rapidapi.com/"

querystring = {"sign":"aquarius","day":"today"}

headers = {
	"X-RapidAPI-Key": "1404c54e37msh9cab5db91e707adp19bd80jsn2fb1f0d497ee",
	"X-RapidAPI-Host": "sameer-kumar-aztro-v1.p.rapidapi.com"
}

response = requests.post(url, headers=headers, params=querystring)

print(response.json())