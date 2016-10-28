<html>
  
  <head>
    <title>CS143 Movie Database</title>
  </head>

  <body>
    <h1></h1>

    

    <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">

      Director<INPUT TYPE="radio" name="actor_director" VALUE="1" CHECKED>
      <br>
      Actor<INPUT TYPE="radio" name="actor_director" VALUE="2">
      <br>
      First Name
      <br>
      <INPUT TYPE="text" name="firstName">
      <br><br>
      Last Name
      <br>
      <input type="text" name="lastName">

      <br><br>
      Date of Birth
      <br>
      <input type="text" name="DOB">
     

      <br><br>
      Date of Death?
      <br>
      <input type="text" name="DOD">
      <input type="submit" value="Submit">

    </form> <br>


    <?php

      # Retrieve the query the user wants to run
      if ($firstName = $_GET['firstName']) {
        echo "$firstName $lastName";
      }
    ?>

  </body>
</html>