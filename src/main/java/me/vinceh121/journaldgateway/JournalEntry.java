package me.vinceh121.journaldgateway;

import java.math.BigInteger;
import java.util.Date;
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

	public Date getSourceTimestamp() {
		return new Date(((long) this.get("_SOURCE_REALTIME_TIMESTAMP")) / 1000);
	}

	public void setSourceTimestamp(final Date date) {
		this.put("_SOURCE_REALTIME_TIMESTAMP", date.getTime() * 1000);
	}

	public Date getRealTimestamp() {
		return new Date((Long.parseLong((String) this.get("__REALTIME_TIMESTAMP"))) / 1000);
	}

	public void setRealTimestamp(final Date date) {
		this.put("__REALTIME_TIMESTAMP", Long.toString(date.getTime() * 1000));
	}

	public Priority getPriority() {
		return Priority.values()[(int) this.get("PRIORITY")];
	}

	public void setPriority(final Priority priority) {
		this.put("PRIORITY", priority.ordinal());
	}

	@Override
	public synchronized String toString() {
		return "JournalEntry " + super.toString();
	}
}
