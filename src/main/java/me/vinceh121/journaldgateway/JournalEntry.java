package me.vinceh121.journaldgateway;

import java.math.BigInteger;
import java.util.HashMap;

public class JournalEntry extends HashMap<String, Object> {
	private static final long serialVersionUID = -5180401000998316537L;

	public String getMessage() {
		return (String) this.get("MESSAGE");
	}

	public void setMessage(final String value) {
		this.put("MESSAGE", value);
	}

	public BigInteger getMessageId() {
		return (BigInteger) this.get("MESSAGE_ID");
	}

	public void setMessageId(final BigInteger value) {
		this.put("MESSAGE_ID", value);
	}

	public String getUnit() {
		return (String) this.get("_SYSTEMD_UNIT");
	}

	public void setUnit(final String unit) {
		this.put("_SYSTEMD_UNIT", unit);
	}

	@Override
	public synchronized String toString() {
		return "JournalEntry " + super.toString();
	}
}
