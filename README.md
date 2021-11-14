# Calculator based on the recursive descent method
Consists of three modules:
<ul>
  <li> <code>main.c</code> — supports user input of the expression, asks for the variables if needed. If an error occurs, terminates with the corresponding exit code:
  <b>1</b> for the error in expression; <b>2</b> for the error in variable value; <b>3</b> for the error while calculating.</li>
  <li> <code>library.c</code> — implementation of the data structure ExpressionTree, that is used to store the expression. Functions <code>syntax_analyse</code> and 
  <code>calculate</code> are used to create the ExpressionTree and then calculate the expression in it respectively.</li>
  <li> <code>variables.c</code> is used to resolve the variables in the program. Stores the name and the value of the variable in the array.</li>
</ul>
Input the expression. If it is correct, the program will calculate it or ask for the values of the variables in it. If all the values are correct, program will calculate
the expression with the given values of the variables, and then if you want to reenter the variables' values for the same expression, input 'y'. Otherwise, input 'n',
and then enter new expression or end the input to stop the program.<br>
<code>Makefile</code> commands:
<ol>
  <li> To build only the library itself run <code>make build_library</code>. To build it with valgrind support, run <code>make build_library_val</code>.</li>
  <li> To compile the <code>main.c</code> just write <code>make</code> in your shell. Also <code>make run</code> will compile and then run the program.
  This will compile the program with valgrind support </li>
  <li> To run the <code>main.c</code> with valgrind run <code>make valcheck</code>. </li>
  <li> To test the <code>main.c</code> run <code>make test_main</code>. This will run tests, that are located at tests_main/tests, and compare
  the program output with the answers, that are located at tests_main/keys, and compare the exit code of the program with the correct one. </li>
  <li> To clean up run <code>make clean</code>. This will delete all the binary files and testing outputs if existed. </li>
</ol>
