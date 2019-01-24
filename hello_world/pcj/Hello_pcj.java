/*
Copyright (c) 2018-2019 Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
SPDX-License-Identifier: BSD-3-Clause
*/

/* This is a simple Java program.
*  FileName : "Hello_pcj.java". 
*/

import lib.util.persistent.*;
import lib.util.persistent.types.*;
import java.util.Scanner;

class Hello_pcj
{
    // Your program begins with a call to main().
    // Prints "Hello, Persistent Memory" to the console.
    public static void main(String args[])
    {
		String option = "0"; 
		
		// Create string
		PersistentString msg = new PersistentString("Hello Persistent Memory!!!");
		
		//Create a scanner to read the command-line input
		Scanner scanner = new Scanner(System.in);
		
		while (!(option.equals("W") || option.equals("R")
				|| option.equals("Q"))) {
			//Prompt for user's input
		 	System.out.print("\nEnter <W> for Write to persistent-memory, <R> for Read from persistent-memory or <Q> to Quit: ");
			
			//Read the input as a String
			option = scanner.next();
		}
		switch(option)
		{	case "W":
				// Store string into persistent object directory with 'My_String'
				ObjectDirectory.put("My_String", msg);
				
				// Print string to console
				System.out.printf("\nWrite the (%s) string to persistent-memory.\n",msg);
				break;
			case "R":
				// Read string back out from persistent object directory
				PersistentString originalMsg = ObjectDirectory.get("My_String", PersistentString.class);
			
				// Validate the string originalMsg with msg
				//System.out.printf("\n %s %s \n", originalMsg, msg);
				if (!(originalMsg == null) && originalMsg.equals(msg)) 
					// Print string to console
					System.out.printf("\nRead the (%s) string from persistent-memory.\n",originalMsg);
				else
					System.out.printf("\nNo (Hello...) string in persistent-memory!!!\n");
				break;
			case "Q":
				//Quit the program
				break;
			default:
				System.out.printf("\nUnknown option (%s) string.\n", option);
				System.out.printf("\nOptions are <W> or <R>.\n");
				break;
		}
    }
}

