package me.vinceh121.journaldbot;

import java.util.List;

public class Config {
	private List<MatchCriteria> criterias;
	private long chatId;
	private String token, username;

	public List<MatchCriteria> getCriterias() {
		return criterias;
	}

	public void setCriterias(List<MatchCriteria> criterias) {
		this.criterias = criterias;
	}

	public long getChatId() {
		return chatId;
	}

	public void setChatId(long chatId) {
		this.chatId = chatId;
	}

	public String getToken() {
		return token;
	}

	public void setToken(String token) {
		this.token = token;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

}
