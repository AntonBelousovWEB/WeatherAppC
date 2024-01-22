#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "env.h"

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata);

int main() {
	CURL* curl;
	CURLcode response;

	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "https://api.openweathermap.org/data/2.5/weather?lat=33.44&lon=94.04&appid=%s", WEATHER_API_KEY);

	curl = curl_easy_init();
	curl_easy_setopt(
		curl, 
		CURLOPT_URL, 
		buffer
	);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
	response = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return 0;
}

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata) {
	printf("\n\n");
	for (size_t i = 0; i < nmemb; i++) {
		printf("%c", ((char*)data)[i]);
	}
	printf("\n\n");
	return nmemb;
}