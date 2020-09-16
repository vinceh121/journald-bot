package me.vinceh121.journaldgateway.parser;

import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;

import me.vinceh121.journaldgateway.JournalEntry;

public abstract class AbstractParser implements Iterator<JournalEntry>, AutoCloseable {
	private final InputStream input;

	public AbstractParser(final InputStream input) {
		this.input = input;
	}

	@Override
	public void close() throws IOException {
		this.input.close();
	}

	@Override
	public abstract JournalEntry next();

	@Override
	public boolean hasNext() {
		return true;
	}
}
