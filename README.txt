READ-ME

Assignment:

	Homework #1 (PM/0-Machine)

Authors:

	Caleb Rivera
	Matthew Labrada

Instructions:

	Uploading to Eustis:

		(Note: Be sure to be on school network or on the university VPN)
		
		1. Open MobaXTerm

		2. Establish a new SSH session with the following settings:
			Remote Host: eustis3.eecs.ucf.edu
			Specify username: (enter your NID in this field)
			Port: 22

		3. Download and extract the submission into a folder

		4. Use the "cd" command in terminal to go to directory containing the folder you just created.
			(Ex: If you extracted the submission to "C:\Users\{CurrentUser}\Downloads\" then in terminal type: 
			cd Downloads)

		5. Once in that directory, type the following command:
			scp -r ./* YOUR_NID@eustis3.eecs.ucf.edu:~/
			(Note: Be sure to replace "YOUR_NID" with your actual UCF NID.)
		
	Compiling/Running on Eustis:

		1. In MobaXTerm, type the following command:
			gcc ./vm.c

		2. Now the program has been compiled. To run the program with a given input file use the following command:
			./a.out "./input.txt"
			(Note: You can replace "input.txt" with the filename of whatever input file you would like to use to run the program. It MUST be located in the root folder if you use this command though.)

		3. After running these commands, text should be printed onto the screen asking you to enter an integer,
		   and once you have done so and pressed enter, more text shall appear on the screen showing the execution
		   of the program. The text represents the Stack and PC, BP, and SP. The activation records are separated with a bar "|".
		
		4. An output text file containing this information will be created in the root directory and will be called "output.txt".
			
			