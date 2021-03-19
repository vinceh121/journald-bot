#include <fstream>
// #include <ostream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "curl_easy.h"
#include "curl_ios.h"
#include "curl_exception.h"
#include "curl_header.h"
#include "curl_receiver.h"

#include "journald-bot.hpp"

using curl::curl_easy;
using curl::curl_ios;
using curl::curl_easy_exception;
using curl::curl_header;
using curl::curl_receiver;

jdb::Config loadConfig() {
	std::ifstream streamConfig("config.json");
	json jsonConfig;
	streamConfig >> jsonConfig;
	return jsonConfig.get<jdb::Config>();
}

json tgGetMe(std::string token) {
	std::stringstream buf;
	curl_ios<std::stringstream> writer(buf);
	
	curl_header headers;
	headers.add("Content-Type: application/json");

	curl_easy easy(writer);
	easy.add<CURLOPT_HTTPHEADER>(headers.get());
	easy.add<CURLOPT_URL>(("https://api.telegram.org/bot" + token + "/getMe").c_str());
	easy.perform();
	json jsonResponse = json::parse(buf.str());
	return jsonResponse;
}

void sendMessage(jdb::Config config, json log, jdb::Criteria crit) {
	json payload;
	payload["chat_id"] = config.chatId;
	payload["parse_mode"] = "MarkdownV2";

	std::string msg;
	msg += "Match\n\n";
	msg += "Field: `";
	msg += crit.field + "`\n";
	msg += "Field value: `"; 
	msg += log[crit.field].get<std::string>();
	msg += "`\n";
	msg += "Regex: `";
	msg += crit.regex;
	msg += "`\n";
	msg += "Message: `";
	msg += log["MESSAGE"].get<std::string>();
	msg += "`\n";

	payload["text"] = msg;

	std::stringstream buf;
	curl_ios<std::stringstream> writer(buf);

	curl_header headers;
	headers.add("Content-Type: application/json");

	std::string strBody = payload.dump();

	curl_easy easy(writer);
	easy.add<CURLOPT_HTTPHEADER>(headers.get());
//	easy.add<CURLOPT_VERBOSE>(1L);
	easy.add<CURLOPT_URL>(("https://api.telegram.org/bot" + config.token + "/sendMessage").c_str());
	easy.add<CURLOPT_POSTFIELDS>(strBody.c_str());
	easy.add<CURLOPT_POSTFIELDSIZE>(strBody.size());
	easy.perform();
}

static std::stringstream stream;

void readStream() {
	std::string line;
	for (;;) {
	std::chrono::milliseconds timespan(10);
	std::this_thread::sleep_for(timespan);
//	while (std::getline(stream, line)) {
	if(std::getline(stream, line))
		std::cout << "OWO: " << line << std::endl;
	}
}

int main() {
	std::cout << "journald-bot henlo" << std::endl; 

	jdb::Config config = loadConfig();
	json me = tgGetMe(config.token);
	if (!me["ok"].get<bool>()) {
		std::cerr << "Login failed: " << me["error_code"]
			<< ": " << me["description"] << std::endl;
		return -1;
	}

	std::cout << "Logged in as " << me["result"]["username"] << std::endl;

//	std::stringstream stream;

	std::thread readThread(readStream);

	curl_ios<std::stringstream> writer(stream);

	curl_header headers;
	headers.add("Accept: application/json");
//	headers.add("Range: entries=:-1:");

	curl_easy easy(writer);
	easy.add<CURLOPT_HTTPHEADER>(headers.get());
	easy.add<CURLOPT_URL>(config.url.c_str());
//	easy.add<CURLOPT_VERBOSE>(true);
	easy.perform();

	return 0;
}

