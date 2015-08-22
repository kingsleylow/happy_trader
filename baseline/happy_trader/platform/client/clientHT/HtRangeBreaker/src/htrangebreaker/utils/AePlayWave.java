/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.UnsupportedAudioFileException;

/**
 *
 * @author DanilinS
 */
public class AePlayWave extends Thread {

	private InputStream is_m = null;
	private Position curPosition;
	private final int EXTERNAL_BUFFER_SIZE = 524288; // 128Kb

	enum Position {

		LEFT, RIGHT, NORMAL
	};

	public AePlayWave(InputStream is) {
		is_m = is;
		curPosition = Position.NORMAL;
	}

	public AePlayWave(InputStream is, Position p) {
		is_m = is;
		curPosition = p;
	}

	public void run() {

		AudioInputStream audioInputStream = null;
		try {
			audioInputStream = AudioSystem.getAudioInputStream(is_m);
		} catch (UnsupportedAudioFileException e1) {
			closeIs();
			HtFrameLogger.logError(e1);
			return;
		} catch (IOException e1) {
			closeIs();
			HtFrameLogger.logError(e1);
			return;
		}

		AudioFormat format = audioInputStream.getFormat();
		SourceDataLine auline = null;
		DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);

		try {
			auline = (SourceDataLine) AudioSystem.getLine(info);
			auline.open(format);
		} catch (LineUnavailableException e) {
			closeIs();
			HtFrameLogger.logError(e);
			return;
		} catch (Exception e) {
			closeIs();
			HtFrameLogger.logError(e);
			return;
		}

		if (auline.isControlSupported(FloatControl.Type.PAN)) {
			FloatControl pan = (FloatControl) auline.getControl(FloatControl.Type.PAN);
			if (curPosition == Position.RIGHT) {
				pan.setValue(1.0f);
			} else if (curPosition == Position.LEFT) {
				pan.setValue(-1.0f);
			}
		}

		auline.start();
		int nBytesRead = 0;
		byte[] abData = new byte[EXTERNAL_BUFFER_SIZE];

		try {
			while (nBytesRead != -1) {
				nBytesRead = audioInputStream.read(abData, 0, abData.length);
				if (nBytesRead >= 0) {
					auline.write(abData, 0, nBytesRead);
				}
			}
		} catch (IOException e) {
			closeIs();
			HtFrameLogger.logError(e);
			return;
		} finally {
			closeIs();
			auline.drain();
			auline.close();
		}

	}
	/**
	 * Helprs
	 */

	private void closeIs()
	{
		try {
			if (is_m != null) {
				is_m.close();
				is_m = null;
			}
		}
		catch (Throwable e)
		{
		}
	}
}

