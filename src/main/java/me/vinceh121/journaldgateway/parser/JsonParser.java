package me.vinceh121.journaldgateway.parser;

import java.io.InputStream;
import java.nio.charset.Charset;
import java.util.Iterator;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;

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
			final ObjectNode obj = (ObjectNode) this.mapper.readTree(line);
			final Iterator<String> fields = obj.fieldNames();
			while (fields.hasNext()) {
				final String field = fields.next();
				final JsonNode node = obj.get(field);
				if (this.isByteEncoded(node)) {
					final JsonNode replNode = this.decodeArray((ArrayNode) node);
					obj.set(field, replNode);
				}
			}
			return this.mapper.treeToValue(obj, JournalEntry.class);
		} catch (final JsonProcessingException e) {
			throw new RuntimeException(e);
		}
	}

	private boolean isByteEncoded(final JsonNode node) {
		if (!node.isArray()) {
			return false;
		}

		for (int i = 0; i < node.size(); i++) {
			if (!node.get(i).isNumber()) {
				return false;
			}
		}
		return true;
	}

	private JsonNode decodeArray(final ArrayNode arr) {
		final byte[] barr = this.mapper.convertValue(arr, byte[].class);
		return new TextNode(new String(barr, Charset.defaultCharset()));
	}
}
