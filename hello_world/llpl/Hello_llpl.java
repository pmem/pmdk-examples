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
 * FileName : "Hello_llpl.java". 
 */
 
import lib.llpl.*;
import java.util.Scanner;

class Hello_llpl
{
	
	/****************************
	 * This function writes the "Hello..." string to persistent-memory.
	 *****************************/
	public static void write_hello_string (byte[] input, Heap h, int size)	
	{
		// block allocation (transactional allocation)
		MemoryBlock newBlock = h.allocateMemoryBlock(size, true);
		
		//Attached the newBllock to the root address
		h.setRoot(newBlock.handle());

		// Write byte array (input) to newBlock @ offset 0 (on both) for 26 bytes
		newBlock.copyFromArray(input, 0, 0, size);

		//Ensure that the array (input) is in persistent memory
		newBlock.flush();

		//Convert byte array (input) to String format and write to console
		System.out.printf("\nWrite the (%s) string to persistent-memory.\n",new String(input));	
		return;
	}

	/****************************
	 * This function reads the "Hello..." string from persistent-memory.
	 *****************************/
	public static void read_hello_string(String msg, Heap h, int size)
	{
		// Allocate buffer for string
		// To retrieve byte array from persistent heap
		byte[] output = new byte[size];
		
		//Get the root block address
		long rootAddr = h.getRoot();
		if (rootAddr == 0) {
			System.out.println("Root Block NOT found!");
			System.exit(0);
		} 
		// Map the newBlock to the root of Flushable class
		MemoryBlock newBlock = h.memoryBlockFromHandle(rootAddr);
		
		// Read 26 bytes @ offset 0 from newBlock to byte array (output)
		newBlock.copyToArray(0L, output, 0, size);

		//Convert byte array (output) to String format and write to console
		System.out.printf("\nRead the (%s) string from persistent-memory.\n",new String(output));
		
		return;
	}
	
	
// Your program begins with a call to main().
// Prints "Hello Persistent Memory" to the console.
	public static void main (String[] args) {
		String option = "0"; 
		byte[] input;
		int size;  // String length
		
		// Define Heap
		Heap h = Heap.getHeap("./persistent_heap", 2147483648L);		
		
		//Initialize the msg string
		String msg = "Hello Persistent Memory!!!";
		
		// Convert String to byte array format
		// To store in persistent heap
		input = msg.getBytes();
		
		// Get the array size
		size = input.length;
		
		//Create a scanner to read the command-line input
		Scanner scanner = new Scanner(System.in);
		
		while (!(option.equals("W") || option.equals("R")
				|| option.equals("Q"))) {
			//Prompt for user's input
		 	System.out.print("\nEnter <W> for Write to persistent-memory, " +
							"<R> for Read from persistent-memory, or <Q> to Quit: ");
			
			//Read the input as a String
			option = scanner.next();
		}
		
		switch(option)
		{	case "W":
				write_hello_string(input, h, size);
				break;
			case "R":
				read_hello_string(msg,h, size);
				break;
			case "Q":
				//Quit the program
				break;
			default:
				System.out.printf("\nUnknown option (%s) string.\n", option);
				System.out.printf("\nOptions are <W> or <R>.\n");
				break;
		}	// End of Switch option
	} 	// End of main function
	
}	// End of Hello class 


