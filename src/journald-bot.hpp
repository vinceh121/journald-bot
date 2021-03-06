#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

using nlohmann::json;

namespace jdb {
	struct Criteria {
		std::string field, regex;
	};

	struct Config {
		std::string url,
			token;
		long int chatId;
		bool verifySsl;
		std::vector<std::vector<Criteria>> criterias;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Criteria, field, regex);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, url, token, chatId, verifySsl, criterias);
}; // namespace jdb

