#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "env.h"
#include "cJSON.h"

typedef struct {
	char *string;
	size_t size;
} Response;

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata);

int main() {
	CURL* curl;
	CURLcode response;

	char buffer[256];
	sprintf_s(
		buffer, 
		sizeof(buffer), 
		"https://api.openweathermap.org/data/2.5/weather?q=Kyiv&appid=%s", 
		WEATHER_API_KEY
	);

	curl = curl_easy_init();

	Response result;
	result.string = malloc(1);
	result.size = 0;

	curl_easy_setopt(
		curl, 
		CURLOPT_URL, 
		buffer
	);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) & result);

	response = curl_easy_perform(curl);

	cJSON* json = cJSON_Parse(result.string);

	if (json != NULL) {
		cJSON* main = cJSON_GetObjectItem(json, "main");
		cJSON* temp = cJSON_GetObjectItem(main, "temp");
		
		int TempValue = temp->valueint;
		printf("Temp: %d F\n", TempValue);

		cJSON_Delete(json);
	}
	else {
		printf("Failed to parse JSON.\n");
	}

	curl_easy_cleanup(curl);

	free(result.string);

	return 0;
}

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata) {
	size_t real_size = size * nmemb;
	Response *response = (Response *)userdata;

	char *ptr = realloc(response->string, response->size + real_size + 1);
	if (ptr == NULL) {
		return CURL_WRITEFUNC_ERROR;
	}

	response->string = ptr;
	memcpy(&(response->string[response->size]), data, real_size);
	response->size += real_size;
	response->string[response->size] = 0;

	return real_size;
}