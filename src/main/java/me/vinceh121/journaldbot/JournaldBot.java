package me.vinceh121.journaldbot;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.Date;
import java.util.regex.Pattern;

import org.telegram.telegrambots.ApiContextInitializer;
import org.telegram.telegrambots.bots.TelegramLongPollingBot;
import org.telegram.telegrambots.meta.TelegramBotsApi;
import org.telegram.telegrambots.meta.api.methods.send.SendMessage;
import org.telegram.telegrambots.meta.api.objects.Update;
import org.telegram.telegrambots.meta.exceptions.TelegramApiException;
import org.telegram.telegrambots.meta.exceptions.TelegramApiRequestException;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.ObjectMapper;

import me.vinceh121.journaldgateway.JournalEntry;
import me.vinceh121.journaldgateway.parser.JsonParser;

public class JournaldBot extends TelegramLongPollingBot {
	private final ObjectMapper mapper = new ObjectMapper();
	private final Date start = new Date();
	private final Config config;

	public static void main(final String[] args) throws JsonParseException, JsonMappingException, IOException {
		final Config config = new ObjectMapper()
				.readValue(args.length == 0 ? new File("./config.json") : new File(args[0]), Config.class);

		ApiContextInitializer.init();
		final TelegramBotsApi api = new TelegramBotsApi();
		try {
			api.registerBot(new JournaldBot(config));
		} catch (final TelegramApiRequestException e) {
			e.printStackTrace();
		}
	}

	public JournaldBot(final Config config) {
		this.config = config;

		new Thread(() -> {
			try {
				this.start();
			} catch (final IOException e) {
				e.printStackTrace();
			}
		}, "JournalListener").start();
	}

	private void start() throws IOException {
		final URL url = new URL(config.getUrl());
		final HttpURLConnection con = (HttpURLConnection) url.openConnection();
		con.addRequestProperty("Accept", "application/json");
		final InputStream in = con.getInputStream();
		final JsonParser parse = new JsonParser(in, Charset.defaultCharset(), this.mapper);
		parse.forEachRemaining(e -> {
			if (this.start.before(e.getRealTimestamp())) { // ignore the entries before we started
				this.processEntry(e);
			}
		});
		parse.close();
	}

	private void processEntry(final JournalEntry e) {
		for (final MatchCriteria c : this.config.getCriterias()) {
			if (this.matches(c, e)) {
				this.sendEntry(c, e);
			}
		}
	}

	private boolean matches(final MatchCriteria crit, final JournalEntry entry) {
		final String value = String.valueOf(entry.get(crit.getField()));
		return Pattern.matches(crit.getRegex(), value);
	}

	private void sendEntry(final MatchCriteria c, final JournalEntry e) {
		final SendMessage msg = new SendMessage();
		msg.enableMarkdownV2(true);
		msg.setText(this.makeText(c, e));
		msg.setChatId(this.config.getChatId());
		try {
			this.execute(msg);
		} catch (final TelegramApiException e1) {
			e1.printStackTrace();
		}
	}

	private String makeText(final MatchCriteria c, final JournalEntry e) {
		final StringBuilder sb = new StringBuilder();
		sb.append("Match\n\n");

		sb.append("Field: `");
		sb.append(c.getField());
		sb.append("`\n");

		sb.append("Field value: `");
		sb.append(e.get(c.getField()));
		sb.append("`\n");

		sb.append("Regex: `");
		sb.append(c.getRegex());
		sb.append("`\n");

		sb.append("Message: `");
		sb.append(e.getMessage());
		sb.append("`\n");

		return sb.toString();
	}

	@Override
	public void onUpdateReceived(final Update update) {
	}

	@Override
	public String getBotUsername() {
		return this.config.getUsername();
	}

	@Override
	public String getBotToken() {
		return this.config.getToken();
	}

}
