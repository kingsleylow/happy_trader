/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker;

import htrangebreaker.utils.AePlayWave;
import htrangebreaker.utils.HtFrameLogger;
import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.InputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.Timer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellRenderer;

/**
 *
 * @author DanilinS
 * must blink
 */
public class HtDrawableWindow_SignalTableRenderer extends JTextArea implements TableCellRenderer {

	private static final String RESOURCE_SOUND_PATH = "/htrangebreaker/resources/alert4.wav";
	private static final int DELAY_TIMER_MSEC = 500;
	private static int BLINK_COUNTS = 50;
	private Timer blinkTimer_m = null;
	private TimerActionListener timerListener_m = null;
	private JTable table_m = null;
	private HashMap<Integer, Integer> blinking_m = new HashMap<Integer, Integer>();
	private boolean sound_m = false;

	public static class TimerActionListener implements ActionListener {

		HtDrawableWindow_SignalTableRenderer main_m = null;

		public TimerActionListener(HtDrawableWindow_SignalTableRenderer mainClass) {
			super();
			main_m = mainClass;

		}

		public void actionPerformed(ActionEvent e) {
			// executed as EDT
			// determine which row to blink
			DefaultTableModel sigtabmodel = (DefaultTableModel) main_m.table_m.getModel();
			for (Iterator<Integer> it = main_m.blinking_m.keySet().iterator(); it.hasNext();) {
				int blinking_row = it.next();
				int blink_count = main_m.blinking_m.get(blinking_row);

				blink_count--;

				if (blink_count >= 0) {

					sigtabmodel.fireTableRowsUpdated(blinking_row, blinking_row);
					if (main_m.sound_m) {
						main_m.startPlayingSound();
					}

					if (blink_count == 0) {
						it.remove();
					} else {
						main_m.blinking_m.put(blinking_row, blink_count);
					}


				}

			}
		}
	}

	// must be done in EDT
	public void setUpBlinkingRow(int row_id) {
		blinking_m.put(row_id, BLINK_COUNTS);
	}

	public void enableSound(boolean is_enabled) {
		sound_m = is_enabled;
	}

	public void setSignalLengthSec(int signal_length) {
		BLINK_COUNTS = (int) (((double) signal_length) / ((double) DELAY_TIMER_MSEC / 1000.0));
	}

	public HtDrawableWindow_SignalTableRenderer(JTable table) {
		super.setOpaque(true);
		super.setLineWrap(true);
		super.setWrapStyleWord(true);
		table_m = table;

		timerListener_m = new TimerActionListener(this);
		blinkTimer_m = new Timer(DELAY_TIMER_MSEC, timerListener_m);
		blinkTimer_m.setRepeats(true);
		blinkTimer_m.start();

	}

	@Override
	public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {



		// determine which row to blink
		Integer blink_row = blinking_m.get(row);
		if (blink_row != null) {

			// deterimine blink colors
			Color c_bck = null;
			Color c_frg = null;

			Color c_bck_selected = null;
			Color c_frg_selected = null;

			int row_blink_counter = blink_row.intValue();
			if ((row_blink_counter % 2) == 0) {
				c_bck = Color.RED;
				c_frg = Color.YELLOW;

				c_bck_selected = Color.GREEN;
				c_frg_selected = Color.WHITE;
			} else {
				c_bck = Color.YELLOW;
				c_frg = Color.RED;

				c_bck_selected = Color.WHITE;
				c_frg_selected = Color.GREEN;
			}

			if (isSelected) {
				// visa virsa
				super.setForeground(c_frg_selected);
				super.setBackground(c_bck_selected);
			} else {
				super.setForeground(c_frg);
				super.setBackground(c_bck);
			}


		} else {

			// not blinking
			if (isSelected) {
				super.setForeground(table.getSelectionForeground());
				super.setBackground(table.getSelectionBackground());
			} else {
				super.setForeground(table.getForeground());
				super.setBackground(table.getBackground());
			}
		}


		setFont(table.getFont());
		setText(value != null ? value.toString() : "");
		return this;
	}

	/**
	 * Helpers
	 */
	private void startPlayingSound() {
		InputStream is = null;

		try {
			is = HtDrawableWindow_SignalTableRenderer.class.getResourceAsStream(RESOURCE_SOUND_PATH);
		} catch (Throwable e2) {
			HtFrameLogger.logError("Cannot load the sound file from the stream: " + RESOURCE_SOUND_PATH + " - " + e2.getMessage());
		}

		if (is != null) {
			new AePlayWave(is).start();
		}
	}
}
