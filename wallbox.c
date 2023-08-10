#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#define BASE_URL "https://api.wall-box.com/"
#define AUTH_URL "https://user-api.wall-box.com/users/signin"
#define USERPWD "email:password"

typedef struct {
	char *string;
	size_t size;
} Response;


void die (const char *message);
size_t write_chunk (void *data, size_t size, size_t nmemb, void *userdata);


int main(void)
{
	CURL *curl;
	struct curl_slist *header = NULL;
	CURLcode result;

	curl = curl_easy_init();
	curl_global_init(CURL_GLOBAL_ALL);
	if (curl == NULL) {
		die("HTTP request fail\n");
	}

	Response response;
	response.string = malloc(1);
	response.size = 0;

	curl_easy_setopt(curl, CURLOPT_URL, AUTH_URL);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	//Headers, Partner header is important
	header = curl_slist_append(header, "Content-Type: application/json");
	header = curl_slist_append(header, "Accept: application/json");
	header = curl_slist_append(header, "Partner: wallbox");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

	//Authentication
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
	curl_easy_setopt(curl, CURLOPT_USERPWD, getenv("WALLBOX_USERPWD"));
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_chunk);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response); 


	result = curl_easy_perform(curl);

	if (result != CURLE_OK) {
		die(curl_easy_strerror(result));
	}

	printf("%s\n", response.string);

	//JSON parse
	struct json_object *parsed_json;
	struct json_object *data;
	struct json_object *attributes;
	struct json_object *user_id;
	struct json_object *token;
	parsed_json = json_tokener_parse(response.string);
	json_object_object_get_ex(parsed_json, "data", &data);
	json_object_object_get_ex(data, "attributes", &attributes);
	json_object_object_get_ex(attributes, "user_id", &user_id);
	json_object_object_get_ex(attributes, "token", &token);

	printf("user_id: %s\n", json_object_get_string(user_id));
	printf("token: %s\n", json_object_get_string(token));

	curl_easy_cleanup(curl);

	free(response.string);

	return 0;
}

size_t write_chunk (void *data, size_t size, size_t nmemb, void *userdata)
{
	size_t real_size = size * nmemb;

	Response *response = (Response *) userdata;
	
	char *ptr = realloc(response->string, response->size + real_size + 1);

	if (ptr == NULL) {
		return 0;
		// return CURL_WRITEFUNC_ERROR; ??
	}

	response->string = ptr;
	memcpy(&(response->string[response->size]), data, real_size);
	response->size += real_size;
	response->string[response->size] = '\0'; // =0


	return real_size;
}

void die (const char *message)
{
	fprintf(stderr, "ERROR: %s\n", message);
	exit(-1);
}