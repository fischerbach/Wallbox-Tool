#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "dbg.h"
#define SV_IMPLEMENTATION
#include "sv.h"

#define BASE_URL "https://api.wall-box.com/"
#define AUTH_URL "https://user-api.wall-box.com/users/signin"

typedef struct {
	char *string;
	size_t size;
} Response;

typedef struct {
	String_View token;
	time_t token_expiration_date;
} Wallbox;

time_t wallbox_get_expiration_time_from_now(void)
{
	time_t currentTime;
	time(&currentTime);
	return currentTime + 14 * 24 * 3600;
} 


size_t write_chunk (void *data, size_t size, size_t nmemb, void *userdata)
{
	size_t real_size = size * nmemb;

	Response *response = (Response *) userdata;
	
	char *ptr = realloc(response->string, response->size + real_size + 1);

	check_mem(ptr);

	response->string = ptr;
	memcpy(&(response->string[response->size]), data, real_size);
	response->size += real_size;
	response->string[response->size] = '\0'; // =0


	return real_size;

error:
	return 0;
}

String_View wallbox_get_token(char *user_pwd)
{

	CURL *curl;
	struct curl_slist *header = NULL;
	CURLcode result;

	curl = curl_easy_init();
	curl_global_init(CURL_GLOBAL_ALL);
	check(curl != NULL, "HTTP request fail\n");


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
	curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd);
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_chunk);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response); 


	result = curl_easy_perform(curl);

	check(result == CURLE_OK, "%s", curl_easy_strerror(result));

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

	String_View token_sv = sv_from_cstr(json_object_get_string(token));

	curl_easy_cleanup(curl);

	free(response.string);

	return token_sv;

error:
	curl_easy_cleanup(curl);
	exit(-1);
}

int main(void)
{

	Wallbox wallbox = {
		.token = wallbox_get_token(getenv("WALLBOX_USERPWD")),
		.token_expiration_date = wallbox_get_expiration_time_from_now()
	};

	printf("token: " SV_Fmt "\n", SV_Arg(wallbox.token));
	printf("expiration date: %d", (uint32_t)wallbox.token_expiration_date);

}
