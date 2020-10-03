package me.vinceh121.journaldbot;

public class MatchCriteria {
	private String field, regex;

	public String getField() {
		return this.field;
	}

	public void setField(final String field) {
		this.field = field;
	}

	public String getRegex() {
		return this.regex;
	}

	public void setRegex(final String regex) {
		this.regex = regex;
	}
}
