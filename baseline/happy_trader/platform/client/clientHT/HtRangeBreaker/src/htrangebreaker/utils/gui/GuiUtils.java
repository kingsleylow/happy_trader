/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils.gui;

import java.awt.Point;
import java.awt.Rectangle;
import javax.swing.JTable;
import javax.swing.JViewport;
import javax.swing.table.DefaultTableModel;

/**
 *
 * @author DanilinS
 */
public class GuiUtils {

	public static void scrollTableDown(JTable table) {
		int rowCount = table.getRowCount() - 1;

		Rectangle rect = table.getCellRect(rowCount, 0, true);
		table.scrollRectToVisible(rect);
		table.clearSelection();
		table.setRowSelectionInterval(rowCount, rowCount);

		((DefaultTableModel)table.getModel()).fireTableDataChanged(); // notify the model
	}

	public static void scrollToVisible(JTable table, int rowidx) {

		int vColIndex = 0;

    if (!(table.getParent() instanceof JViewport)) {
        return;
    }
    JViewport viewport = (JViewport)table.getParent();

    // This rectangle is relative to the table where the
    // northwest corner of cell (0,0) is always (0,0).
    Rectangle rect = table.getCellRect(rowidx, vColIndex, true);

    // The location of the viewport relative to the table
    Point pt = viewport.getViewPosition();

    // Translate the cell location so that it is relative
    // to the view, assuming the northwest corner of the
    // view is (0,0)
    rect.setLocation(rect.x-pt.x, rect.y-pt.y);

    // Scroll the area into view
    viewport.scrollRectToVisible(rect);
	}

	public static void scrollToVisible(JTable table) {

		scrollToVisible(table, table.getRowCount() - 1);
		
	}
}
