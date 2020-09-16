package me.vinceh121.journaldgateway.parser;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

import me.vinceh121.journaldgateway.JournalEntry;

public abstract class LineParser extends AbstractParser {
	private final BufferedReader br;

	public LineParser(final InputStream input, final Charset charset) {
		super(input);
		this.br = new BufferedReader(new InputStreamReader(input, charset));
	}

	@Override
	public JournalEntry next() {
		try {
			return this.parseLine(this.br.readLine());
		} catch (final IOException e) {
			throw new RuntimeException(e);
		}
	}

	public abstract JournalEntry parseLine(final String line);

	@Override
	public void close() throws IOException {
		this.br.close();
		super.close();
	}

}
