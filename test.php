<html>
  
  <head>
    <title>CS143 Movie Database</title>
  </head>

  <body>
    <h1></h1>

    <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
      <h2>Submit a New Person!</h2>
      Actor  <INPUT TYPE="radio" name="actor_director" VALUE="Actor" CHECKED>

      Director<INPUT TYPE="radio" name="actor_director" VALUE="Director">
      <br>
      First Name
      <br>
      <INPUT TYPE="text" name="firstName">
      <br><br>
      Last Name
      <br>
      <input type="text" name="lastName">
      <br>
      Male <INPUT TYPE="radio" name="gender" VALUE="Male" CHECKED>

      Better<INPUT TYPE="radio" name="gender" VALUE="Female">
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
 <!-- LINK TO PAGE HERE--> 
    <form method="post" action="infoPage.php">
      <input type = "submit">

    <?php
      if ($_SERVER["REQUEST_METHOD"] == "GET"){
        $quitError=0;
      # CHECK that we have valid input
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
        $sex = $_GET['gender'];
        $dod = $_GET['DOD'];
        if($dod == ""){
          $dod = NULL;
        }

        if(!$quitError){
          #where we would want to INSERT
          $database = new mysqli('localhost', 'cs143', '', 'CS143');
          if($database->connect_errno > 0){
            die('Unable to connect to database [' . $database->connect_error . ']');
          }
          //make sure to do query to get max person ID and then increment that too
          $id_query = "SELECT * FROM MaxPersonID;";
          $MaxPersonID = $database->query($id_query);
          $result = $MaxPersonID->fetch_array(MYSQLI_NUM);
          $id = $result[0] + 1;

          #director query
          $occupation = $_GET['actor_director'];
          if($occupation == "Director"){
            $personStatement = $database->prepare("INSERT INTO Director (id, last, first, dob, dod) VALUES (?, ?, ?, ?, ?);");
            $personStatement->bind_param("issss", $id, $lastName, $firstName, $dob, $dod);
          }
          #actor query
          else{
            $personStatement = $database->prepare("INSERT INTO Actor (id, last, first, sex, dob, dod) VALUES (?, ?, ?, ?, ?, ?);");
            $personStatement->bind_param("isssss", $id, $lastName, $firstName, $sex, $dob, $dod);
          }
         
          $personStatement->execute();
          //TODO: make sure INSERT succeeded before updating this value

          $id_statement = $database->prepare("UPDATE MaxPersonID SET id = $id;");
          $id_statement->execute();

         
          echo "submitted!";
        }
      }
    ?>

  </body>
</html>
