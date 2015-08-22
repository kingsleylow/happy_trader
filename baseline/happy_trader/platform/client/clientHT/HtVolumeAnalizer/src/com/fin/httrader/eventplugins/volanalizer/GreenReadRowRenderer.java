/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.volanalizer;

import java.awt.Color;
import java.awt.Component;
import java.text.DecimalFormat;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;

/**
 *
 * @author DanilinS
 */
public class GreenReadRowRenderer extends DefaultTableCellRenderer {

	DecimalFormat priceFormatter_m = null;
	private int open_idx_m = -1;
	private int close_idx_m = -1;

	public GreenReadRowRenderer(DecimalFormat priceFormatter, int open_idx, int close_idx) {
		super();
		priceFormatter_m = priceFormatter;
		open_idx_m = open_idx;
		close_idx_m = close_idx;
	}

	public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
		Component cell = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);

		double open = -1, close = -1;
		try {
			open = priceFormatter_m.parse((String) table.getValueAt(row, open_idx_m)).doubleValue();
			close = priceFormatter_m.parse((String) table.getValueAt(row, close_idx_m)).doubleValue();
		} catch (Throwable e) {
		}

		if (isSelected || hasFocus) {
			super.setForeground(table.getSelectionForeground());
		} else {
			super.setForeground(table.getForeground());
		}

		if (open > 0 && close > 0) {
			double diff = close - open;
			if (diff > 0) {
				cell.setBackground(Color.GREEN);
			} else {
				cell.setBackground(Color.RED);
			}
		} else {
			if (isSelected || hasFocus) {
				cell.setBackground(table.getSelectionBackground());
			} else {
				cell.setBackground(table.getBackground());
			}
		}


		return cell;
	}

	public static void signupForTable(JTable table, DecimalFormat priceFormatter, int open_idx, int close_idx)
	{
		GreenReadRowRenderer renderer  =new GreenReadRowRenderer(priceFormatter, open_idx, close_idx);
		for (int i = 0; i < table.getColumnCount(); i++) {
			table.getColumnModel().getColumn(i).setCellRenderer(renderer);
		}
	}

}
