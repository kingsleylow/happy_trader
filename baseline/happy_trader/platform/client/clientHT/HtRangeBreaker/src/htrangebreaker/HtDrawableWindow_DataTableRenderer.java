/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker;

import java.awt.Color;
import java.awt.Component;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellRenderer;

/**
 *
 * @author DanilinS
 */
public class HtDrawableWindow_DataTableRenderer extends JTextArea implements TableCellRenderer {

	public static interface RenderInterface {
		public Color getBackgroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
		public Color getForegroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
	};

	public static interface RowSelector
	{
		public boolean isRowSelected(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
	}

	private RenderInterface rint_m = null;
	private RowSelector rsp_m;
	private int selectorColumn_m = -1;

	public HtDrawableWindow_DataTableRenderer(RenderInterface  rint, RowSelector rsp, int selectorColumn) {
		super.setOpaque(true);
		super.setLineWrap(true);
		super.setWrapStyleWord(true);
		selectorColumn_m = selectorColumn;
		
		rint_m = rint;
		rsp_m = rsp;
	}

	@Override
	public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {

		if (isSelected)	{
			super.setForeground(table.getSelectionForeground());
			super.setBackground(table.getSelectionBackground());
		} else 	{
			super.setForeground(table.getForeground());
			super.setBackground(table.getBackground());
		}

		Color c_bckg = rint_m.getBackgroundColor(table, value, isSelected, hasFocus, row, column);
		Color c_frgr = rint_m.getForegroundColor(table, value, isSelected, hasFocus, row, column);

		if (c_bckg != null)		{
			super.setBackground(c_bckg);
		}

		if (c_frgr != null) {
			super.setForeground(c_frgr);
		}

		if (rsp_m != null) {
			if (rsp_m.isRowSelected(table, value, isSelected, hasFocus, row, column) ) {
				if (selectorColumn_m >=0 && selectorColumn_m == column) {
					super.setForeground(table.getSelectionForeground());
					super.setBackground(table.getSelectionBackground());
				}
			}
		}
		
		setFont(table.getFont());
		setText(value !=null ? value.toString() : "");
		return this;
	}
}
