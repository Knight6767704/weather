#include <iostream>
#include <string>
#include "json.hpp"
#include <curl/curl.h>

using json = nlohmann::json;

const std::string API_KEY = "ffcec4f94f9aef958195120272c15811";
const std::string API_URL = "http://api.openweathermap.org/data/2.5/weather";

size_t curlCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int main() {
    std::string cityName;
    std::cout << "Enter the location (city name): ";
    std::getline(std::cin, cityName);

    std::string requestUrl = API_URL + "?q=" + cityName + "&appid=" + API_KEY;
    std::string response;

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing CURL." << std::endl;
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, requestUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error making HTTP request: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    json jsonData;
    try {
        jsonData = json::parse(response);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON data: " << e.what() << std::endl;
        return 1;
    }

    if (jsonData.find("main") == jsonData.end() || jsonData["main"].find("temp") == jsonData["main"].end() ||
        jsonData.find("weather") == jsonData.end() || jsonData["weather"].empty() ||
        jsonData["weather"][0].find("description") == jsonData["weather"][0].end()) {
        std::cerr << "Failed to fetch weather data." << std::endl;
        return 1;
    }

    std::string city = jsonData["name"];
    double temperature = jsonData["main"]["temp"];
    std::string weatherDescription = jsonData["weather"][0]["description"];

    std::cout << "Weather in " << city << std::endl;
    std::cout << "Temperature: " << temperature-273.15 << " Celsius" << std::endl;
    std::cout << "Weather Description: " << weatherDescription << std::endl;

    return 0;
}
