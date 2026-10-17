
#include "crow.h"
#include <curl/curl.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

static size_t WriteCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp)
{
    size_t bytes = size * nmemb;
    string* response = static_cast<string*>(userp);

    response->append(
        static_cast<char*>(contents),
        bytes
    );

    return bytes;
}

int main()
{
    const char* apiKey = getenv("GOOGLE_MAPS_API_KEY");

    if (apiKey == nullptr || *apiKey == '\0')
    {
        cerr << "Error: GOOGLE_MAPS_API_KEY is not set."
             << endl;
        return 1;
    }

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
    {
        cerr << "Failed to initialize libcurl." << endl;
        return 1;
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/hello")
    ([]() {
        return "Hello from my C++ API!";
    });

    CROW_ROUTE(app, "/route")
        .methods(crow::HTTPMethod::POST)
    ([apiKey](const crow::request& req) {
        auto input = crow::json::load(req.body);

        if (!input)
        {
            return crow::response(400, "Invalid JSON request.");
        }

        if (!input.has("origin") || !input.has("destination"))
        {
            return crow::response(
                400,
                "Provide both origin and destination."
            );
        }

        string origin = input["origin"].s();
        string destination = input["destination"].s();

        crow::json::wvalue payload;
        payload["origin"]["address"] = origin;
        payload["destination"]["address"] = destination;
        payload["travelMode"] = "DRIVE";

        string requestBody = payload.dump();
        string responseBody;

        CURL* curl = curl_easy_init();

        if (!curl)
        {
            return crow::response(
                500,
                "Failed to initialize HTTP request."
            );
        }

        curl_slist* headers = nullptr;

        headers = curl_slist_append(
            headers,
            "Content-Type: application/json"
        );

        string keyHeader = string("X-Goog-Api-Key: ") + apiKey;

        headers = curl_slist_append(
            headers,
            keyHeader.c_str()
        );

        headers = curl_slist_append(
            headers,
            "X-Goog-FieldMask: routes.duration,routes.distanceMeters,routes.polyline.encodedPolyline"
        );

        curl_easy_setopt(
            curl,
            CURLOPT_URL,
            "https://routes.googleapis.com/directions/v2:computeRoutes"
        );

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(
            curl,
            CURLOPT_POSTFIELDS,
            requestBody.c_str()
        );
        curl_easy_setopt(
            curl,
            CURLOPT_POSTFIELDSIZE,
            static_cast<long>(requestBody.size())
        );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        CURLcode result = curl_easy_perform(curl);

        long statusCode = 0;

        if (result == CURLE_OK)
        {
            curl_easy_getinfo(
                curl,
                CURLINFO_RESPONSE_CODE,
                &statusCode
            );
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (result != CURLE_OK)
        {
            cerr << "HTTP request failed: "
                 << curl_easy_strerror(result) << endl;

            return crow::response(
                502,
                "Failed to contact Google Routes API."
            );
        }

        crow::response response(
            static_cast<int>(statusCode),
            responseBody
        );

        response.set_header("Content-Type", "application/json");

        return response;
    });

    cout << "API running at http://localhost:18080" << endl;

    app.port(18080).multithreaded().run();

    curl_global_cleanup();

    return 0;
}


