<html>
  
  <head>
    <title>CS143 Movie Database</title>
  </head>

  <body>
    <h1></h1>

    <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
      <h2>Submit a New Person!</h2>
      Actor  <INPUT TYPE="radio" name="actor_director" VALUE="1" CHECKED>

      Director<INPUT TYPE="radio" name="actor_director" VALUE="2">
      <br>
      First Name
      <br>
      <INPUT TYPE="text" name="firstName">
      <br><br>
      Last Name
      <br>
      <input type="text" name="lastName">
      <br>
      Male <INPUT TYPE="radio" name="gender" VALUE="1" CHECKED>

      Better<INPUT TYPE="radio" name="gender" VALUE="2">
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
      if ($_SERVER["REQUEST_METHOD"] == "GET"){
        $quitError=0;
      # Retrieve the query the user wants to run
        if (!($firstName = $_GET['firstName'])) {
          echo "Need a first name!<br>";
          $quitError=1;
        }
        if (!($lastName = $_GET['lastName'])) {
          echo "Need a last name!<br>";
          $quitError=1;
        }
        if (!($dob = $_GET['DOB'])) {
          echo "Need a date of birth!<br>";
          $quitError=1;
        }
        if(!$quitError){
          #where we would want to INSERT
          $database = new mysqli('localhost', 'cs143', '', 'CS143');
          if($database->connect_errno > 0){
            die('Unable to connect to database [' . $database->connect_error . ']');
          }
          //make sure to do query to get max person ID and then increment that too
          echo "about to make statement <br>";
          $actorStatement = $database->prepare("INSERT INTO Actor (id, last, first, sex, dob, dod) VALUES (?, ?, ?, ?, ?, ?)");
          echo "about to bind param <br>";
          $t_id = 80000;
          $t_last = "haacker";
          $t_first = "kyle";
          $t_sex = "Male";
          $t_dob = "1000-10-10";
          $t_dod = "1900-10-10";
          $actorStatement->bind_param("isssss", $t_id, $t_last, $t_first, $t_sex, $t_dob, $t_dod);
          echo "binded dates <br>";
          //seems to insert, cant find when query tho so lets fix that bug 
          $statement->execute();
          echo "submitted!";
        }
      }
    ?>

  </body>
</html>
