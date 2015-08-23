/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker;

import java.awt.Color;
import java.awt.Component;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.table.TableCellRenderer;

/**
 *
 * @author DanilinS
 */
public class HtLoggingWindow_LogTableRenderer extends JTextArea implements TableCellRenderer {

	public static interface RenderInterface
		{
			public String onRenderValue(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
			public Color getBackgroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
			public Color getForegroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);


	};

	private RenderInterface rint_m;
	private Border border_m = null;

	public HtLoggingWindow_LogTableRenderer(RenderInterface rint)
	{
		super.setOpaque(true);
		super.setLineWrap(true);
		super.setWrapStyleWord(true);

		rint_m = rint;
		border_m = UIManager.getBorder("Table.focusCellHighlightBorder");
	}

	public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
		setText(rint_m.onRenderValue(table, value, isSelected, hasFocus, row, column));
		if (isSelected) {
				super.setForeground(table.getSelectionForeground());
				super.setBackground(table.getSelectionBackground());
			} else {
				super.setForeground(table.getForeground());
				super.setBackground(table.getBackground());
			}

		Color c_bckg = rint_m.getBackgroundColor(table, value, isSelected, hasFocus, row, column);
			Color c_frgr = rint_m.getForegroundColor(table, value, isSelected, hasFocus, row, column);

			if (c_bckg != null) {
				super.setBackground(c_bckg);
			}

			if (c_frgr != null) {
				super.setForeground(c_frgr);
			}

			setFont(table.getFont());
			if (hasFocus) {
				setBorder(border_m);
				//if (table.isCellEditable(row, column)) {
				//	setForeground(UIManager.getColor("Table.focusCellForeground"));
				//	setBackground(UIManager.getColor("Table.focusCellBackground"));
				//}
			} else {
				setBorder(new EmptyBorder(1, 2, 1, 2));
			}
			return this;
	}

}
