# Shell
Recreation of a Linux Bash with new commands implemented in c

Steps to run project:
1. Clone project
2. Open Linux shell inside the project folder
3. run ```gcc shell.c -o <name>```
4. run ```./<name>```

You can type ```help```, and a list of commands and their funcionality will be display, ```help <command name>``` will give you a specific description. The native commands are also available.  

Let's see some interesting prompts:

1. We have a dictionary that you can call using ```set``` and ```get```

  ```set <variable name> <value>``` will create a new variable in the dictionary  
  ```get <variable name>``` will return the value of the variable

  In ```set``` if value is between parentheses the shell will treat it as a command line, and ```get <that variable name>``` will run the command line  
    ```set a (cd folder)```  
    ```get a ```  
    this will replace your current path to previous_current_path/folder, in case folder exists, if not an error will be shown.  
    the same with other commands:  
    ```set b (cd folder && ls)```  
    ```get b```  
    this will replace your current path to previous_current_path/folder, and print the list of files inside folder.
    
    
 2. Create folder with spaces in the name, and placing the current path into folders with spaces in the name  
 
  ```mkdir (test 1)``` \\ name : test 1  
  ```cd (test 1)``` \\ current path: oldpath\test 1  
  
  if you want the name wrapped in parenthesis, not problem, inside the parenthesis you can put anything, the shell will only exclude the parenthesis more outside  
  ```mkdir ((test 1))``` \\ name : (test 1)  
  ```mkdir ((test 1)``` \\ name : (test 1  
  ```mkdir (test 1))``` \\ name : test 1)  
  ```mkdir ((test) 1)``` \\ name : (test) 1  
 
 3. Parenthesis in other occasions
 
  ```command 1 && command 2 || command 3``` in case command 1 fails no other command is executed  
  ```(commmand 1 && command 2) || command 3``` in case command 1 fails, command 3 will be executed  
  
  ```command 1 && command 2 &``` if command 1 success, command 2 will be executed in the background  
  ```(command 1 && command 2) &``` everything inside the parenthesis will be executed in the background  
  
  in the case of pipes, they have less priority than &, so:  
  ```command 1 | command 2 &``` in this case ```command 1 | command 2``` will go to the background  
  ```command 1 | (command 2 &)``` the standart out of command 1 will go to the standart in of command 2 that will be executed in the background  
  
 4. Those examples was just to highlight some interesting cases, there are a lot more commands, fill free to try all the commands combination you want, remember to put at least one space between commands, it can be more than one.
