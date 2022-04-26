#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using nlohmann::json;

namespace jdb {
	struct Criteria {
		std::string field, regex;
		bool inverted;
	};
	// couldn't get _WITH_DEFAULT to work for some reason
	void to_json(json& j, const Criteria& value) {
		j = json{{"field", value.field},
			{"regex", value.regex},
			{"inverted", value.inverted}};
	}

	void from_json(const json& j, Criteria& value) {
		j.at("field").get_to(value.field);
		j.at("regex").get_to(value.regex);
		if (j.contains("inverted"))
			j.at("inverted").get_to(value.inverted);
		else
			value.inverted = false;
	}

	struct Config {
		std::string url,
			token;
		long int chatId;
		bool verifySsl;
		std::vector<std::vector<Criteria>> criterias;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, url, token, chatId, verifySsl, criterias)
}; // namespace jdb

