/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.htraderhelper.utils;

import java.io.Reader;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author DanilinS
 * parses input stream beginning between begin token and end token
 */
public class TokenizerHelper {

	private Reader in_m = null;

	private List<Integer> chars_m = new ArrayList<Integer>();


	public TokenizerHelper(Reader in)
	{
		in_m = in;
	}

	// ------------
	// hepers

	public int readNext() throws Exception
	{
		if (chars_m.size()==0) {
			return in_m.read();
		}
		else {
		
			int c = chars_m.get( 0 );
			chars_m.remove(0);

			return c;
		}
	}

	public boolean checkIfNextIsToken(String token) throws Exception
	{
		
	
		int token_length = token.length();
		int[] read_array = new int[ token_length ];

		// create array with our read data
		for(int i = 0; i < token_length; i++) {
			if (i < chars_m.size() ) {
				read_array[i] = chars_m.get(i);
			}
			else {
				int c = in_m.read();
				if (c==-1) {
					return false;
				}

				read_array[i] = c;
				chars_m.add(c);
			}
		}

		boolean result = true;
		for(int i = 0; i < token_length; i++) {
			
			if (token.charAt(i)!=read_array[i]) {
				
				result = false;
				break;
			}
		}


		return result;
	}

	public boolean passSymbols(int cnt) throws Exception
	{
		for(int i = 0; i < cnt; i++) {
			int c=readNext();
			if (c ==-1)
				return false;
		}

		return true;
	}


	
}
