#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include <regex>

#include <cpr/cpr.h>
#include <cpr/callback.h>

#include "journald-bot.hpp"

jdb::Config loadConfig() {
	std::ifstream streamConfig("/etc/journald-bot/config.json");
	json jsonConfig;
	streamConfig >> jsonConfig;
	return jsonConfig.get<jdb::Config>();
}

json tgGetMe(jdb::Config config) {
	cpr::Response res = cpr::Get(
			cpr::Url{"https://api.telegram.org/bot" + config.token + "/getMe"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::VerifySsl(config.verifySsl));
	if (res.error.code != cpr::ErrorCode::OK) {
		throw std::runtime_error(res.error.message);
	}
	json jsonResponse = json::parse(res.text);
	return jsonResponse;
}

void sendMessage(jdb::Config config, json log, std::vector<jdb::Criteria> group) {
	json payload;
	payload["chat_id"] = config.chatId;
	payload["parse_mode"] = "MarkdownV2";

	std::string msg;
	for (jdb::Criteria crit : group) {
		msg += "Match\n\n";
		msg += "Field: `";
		msg += crit.field + "`\n";
		// if we're checking against MESSAGE, prevent duplicate field value
		if (crit.field.compare("MESSAGE") != 0) {
			msg += "Field value: `"; 
			msg += log[crit.field].get<std::string>();
			msg += "`\n";
		}
		msg += "Regex: `";
		msg += crit.regex;
		msg += "`\n";
		msg += "Message: `";
		msg += log["MESSAGE"].get<std::string>();
		msg += "`\n\n\\-\\-\\-\\-\\-\\-\\-\\-\n\n";
	}

	payload["text"] = msg;

	cpr::Response res = cpr::Post(
				cpr::Url{"https://api.telegram.org/bot" + config.token + "/sendMessage"},
				cpr::Body{payload.dump()},
				cpr::Header{{"Content-Type", "application/json"}},
				cpr::VerifySsl(config.verifySsl)
			);
	if (res.error.code != cpr::ErrorCode::OK) {
		throw std::runtime_error(res.error.message);
	}
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
	return std::regex_match(str, std::regex(crit.regex));
}

bool doesCriteriaMatch(std::vector<jdb::Criteria> group, json log) {
	for (jdb::Criteria crit : group) {
		if (!doesCriteriaMatch(crit, log)) {
			return false;
		}
	}
	return true;
}

int main() {
	std::cout << "journald-bot henlo" << std::endl; 

	jdb::Config config;
	try {
		config = loadConfig();
	} catch (const std::exception& e) {
		std::cerr << "Failed to read /etc/journald-bot/config.json: " << e.what() << std::endl;
		return -1;
	}

	json me;
	try {
		me = tgGetMe(config);
	} catch (const std::exception& e) {
		std::cerr << "Failed to getMe: " << e.what() << std::endl;
		return -2;
	}
	if (!me["ok"].get<bool>()) {
		std::cerr << "Login failed: " << me["error_code"]
			<< ": " << me["description"] << std::endl;
		return -3;
	}

	std::cout << "Logged in as " << me["result"]["username"] << std::endl;


	cpr::Response res = cpr::Get(
			cpr::Url{config.url},
			cpr::Header{{"Accept", "application/json"},
				{"Range", "entries=:-1:"}},
			cpr::WriteCallback(
				[config](std::string data) {
					json jsonLog;

					try {
						jsonLog = json::parse(data);
					} catch (const json::parse_error& e) {
						std::cerr << "Failed to parse JSON entry: " << e.what() << std::endl;
						return true;
					} catch (...) {
						std::cerr << "Unexpected parse exception of entry: \"" << data << "\"" << std::endl;
						return true;
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
					return true;
				}),
			cpr::VerifySsl(config.verifySsl)
		);

	if (res.error.code != cpr::ErrorCode::OK) {
		std::cerr << "Failed to connect to gateway: " << res.error.message << std::endl;
		return -4;
	}
	return 0;
}

