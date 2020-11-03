package me.vinceh121.journaldbot;

import java.util.List;

public class Config {
	private List<MatchGroup> criterias;
	private long chatId;
	private String token, username, url;

	public List<MatchGroup> getCriterias() {
		return this.criterias;
	}

	public void setCriterias(final List<MatchGroup> criterias) {
		this.criterias = criterias;
	}

	public long getChatId() {
		return this.chatId;
	}

	public void setChatId(final long chatId) {
		this.chatId = chatId;
	}

	public String getToken() {
		return this.token;
	}

	public void setToken(final String token) {
		this.token = token;
	}

	public String getUsername() {
		return this.username;
	}

	public void setUsername(final String username) {
		this.username = username;
	}

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

}
