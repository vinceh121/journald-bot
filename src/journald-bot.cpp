#include <fstream>
#include <iostream>
#include <exception>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <chrono>

#include <nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "journald-bot.hpp"

#define CONFIG_PATH "/etc/journald-bot/config.json"

jdb::Config loadConfig() {
	std::ifstream streamConfig(CONFIG_PATH);
	json jsonConfig;
	streamConfig >> jsonConfig;
	return jsonConfig.get<jdb::Config>();
}

void sendMessage(jdb::Config config, json log, std::vector<jdb::Criteria> group) {
	json payload;
	payload["topic"] = config.ntfyTopic;
	payload["title"] = "journald-bot";
	payload["markdown"] = true;

	std::string msg;
	msg += "*`";
	msg += log["MESSAGE"].get<std::string>();
	msg += "`*\n";
	for (jdb::Criteria crit : group) {
		msg += "Field: `";
		msg += crit.field;
		msg += "`\n";

		msg += "Field value: `"; 
		msg += log[crit.field].get<std::string>();
		msg += "`\n";

		if (crit.inverted) {
			msg += "\\!";
		}
		msg += "Regex: `";
		msg += crit.regex;
		msg += "`\n\n";
	}

	payload["message"] = msg;

	std::stringstream buf(payload.dump());

	std::list<std::string> headers{};

	if (!config.ntfyToken.empty()) {
		headers.push_back("Authorization: Bearer " + config.ntfyToken);
	}

	curlpp::Easy request;
	request.setOpt(curlpp::Options::Url(config.ntfyUrl));
	request.setOpt(curlpp::Options::ReadStream(&buf));
	request.setOpt(curlpp::Options::InfileSize(buf.str().size()));
	request.setOpt(curlpp::Options::Upload(true));
	request.setOpt(curlpp::Options::HttpHeader(headers));
	request.setOpt(curlpp::Options::WriteFunction([](char* ptr, size_t size, size_t nmemb) {
		return size * nmemb;
	}));

	request.perform();
}

bool doesCriteriaMatch(jdb::Criteria crit, json log) {
	json value = log[crit.field];
	if (value.is_null()) {
		return false;
	}

	std::string str;
	if (value.is_string()) {
		str = value.get<std::string>();
	} else {
		str = value.dump();
	}
	bool match = std::regex_match(str, std::regex(crit.regex));
	if (crit.inverted) {
		return !match;
	} else {
		return match;
	}
}

bool doesCriteriaMatch(std::vector<jdb::Criteria> group, json log) {
	for (jdb::Criteria crit : group) {
		if (!doesCriteriaMatch(crit, log)) {
			return false;
		}
	}
	return true;
}

void handleLine(jdb::Config& config, std::string line) {
	json jsonLog;

	try {
		jsonLog = json::parse(line);
	} catch (const json::parse_error& e) {
		std::cerr << "Failed to parse JSON entry: " << e.what() << std::endl;
		return;
	} catch (...) {
		std::cerr << "Unexpected parse exception of entry: \"" << line << "\"" << std::endl;
		return;
	}

	for (std::vector<jdb::Criteria> group : config.criterias) {
		try {
			if (doesCriteriaMatch(group, jsonLog)) {
				sendMessage(config, jsonLog, group);
			}
		} catch (const std::regex_error& e) {
			std::cerr << "Failed to parse regex in criteria group: " << e.what() << std::endl <<
		       		"In group: " << json(group).dump(2) << std::endl;
		} catch (const std::runtime_error& e) {
			std::cerr << "Failed to send message: " << e.what() << std::endl;
		}
	}
}

int main() {
	std::cout << "journald-bot henlo" << std::endl; 

	jdb::Config config;
	try {
		config = loadConfig();
	} catch (const std::exception& e) {
		std::cerr << "Failed to read " << CONFIG_PATH << ": " << e.what() << std::endl;
		return -1;
	}

	auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

	curlpp::Easy request;
	request.setOpt(curlpp::options::Url(config.gatewayUrl));
	request.setOpt(curlpp::options::HttpHeader({
		"Accept: application/json",
		"Range: realtime=" + std::to_string(now) + ":",
	}));
	request.setOpt(curlpp::options::WriteFunction([&config](char* ptr, size_t size, size_t nmemb) {
		size_t realsize = size * nmemb;

		handleLine(config, std::string(ptr, realsize));

		return realsize;
	}));

	request.perform();

	return 0;
}

