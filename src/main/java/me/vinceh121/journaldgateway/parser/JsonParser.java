package me.vinceh121.journaldgateway.parser;

import java.io.InputStream;
import java.nio.charset.Charset;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import me.vinceh121.journaldgateway.JournalEntry;

public class JsonParser extends LineParser {
	private final ObjectMapper mapper;

	public JsonParser(final InputStream input, final Charset charset) {
		this(input, charset, new ObjectMapper());
	}

	public JsonParser(final InputStream input, final Charset charset, final ObjectMapper mapper) {
		super(input, charset);
		this.mapper = mapper;
	}

	@Override
	public JournalEntry parseLine(final String line) {
		try {
			return this.mapper.readValue(line, JournalEntry.class);
		} catch (final JsonProcessingException e) {
			throw new RuntimeException(e);
		}
	}

}
