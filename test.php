<html>  
  <head>
    <title>
      CS143 Movie Database
    </title>
    <style>
      body {
          margin: 0;
      }
      ul {
          list-style-type: none;
          margin: 0;
          padding: 0;
          width: 25%;
          background-color: #A9A9A9;
          position: fixed;
          height: 100%;
          overflow: auto;
      }

      li a {
          display: block;
          color: #000;
          padding: 8px 16px;
          text-decoration: none;
          font-family: "Arial", sans-serif;
      }

      li a.home {
          padding: 8px 8px;
      }

      li a.active {
          background-color: #0080FF;
          color: black;
      }

      li a.header {
          background-color: #000000;
          color: white;
          padding: 8px 8px;
      }

      li a:hover:not(.active, .header) {
          background-color: #66B3FF;
          color: black;
      }
    </style>
  </head>
  <body>
    <ul>
      <li><a class="home" href="./index.php">Home</a></li>
      <li><a class="header">Add Content</a></li>
      <li><a class="active" href="./test.php">Add New Actor/Director</a></li>
      <li><a href="./movieInfo.php">Add New Movie</a></li>
      <li><a href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a href="./movieDiretor.php">Add Movie/Director Relationship</a></li>
      <li><a href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>
    <div style="margin-left:25%; padding:1px 16px; height:1000px;">

    <h1>Add New Actor/Director</h1>

    <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">     
      Actor <INPUT TYPE="radio" name="actor_director" VALUE="Actor" CHECKED>
      Director <INPUT TYPE="radio" name="actor_director" VALUE="Director">
      
      <br><br>

      <b>First Name</b> <br> <INPUT TYPE="text" name="firstName" placeholder="e.g., John">

      <br><br>
      
      <b>Last Name</b> <br> <input type="text" name="lastName" placeholder="e.g., Smith">

      <br><br>

      Male <INPUT TYPE="radio" name="gender" VALUE="Male" CHECKED>
      Female<INPUT TYPE="radio" name="gender" VALUE="Female">

      <br><br>
      
      <b>Date of Birth</b> <br> <input type="text" name="DOB" placeholder="YYYY-MM-DD">

      <br><br>

      <b>Date of Death (if applicable)</b> <br> <input type="text" name="DOD" placeholder="YYYY-MM-DD">
      
      <br><br>

      <input type="submit" value="Submit">
    </form>

    <?php
      if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {

        $quitError = 0;

        if (empty($_GET)) {
          $quitError = 1;
        }

        // Check validity of input
        if (!($firstName = $_GET['firstName'])) {
          echo "Need a first name!<br>";
          $quitError=1;
        }

        if (!($lastName = $_GET['lastName'])) {
          echo "Need a last name!<br>";
          $quitError=1;
        }

        $dateError = 0;

        if (!($dob = $_GET['DOB'])) {
          echo "Need a date of birth!<br>";
          $quitError=1;
          $dateError=1;
        }
        else if ((strlen($dob)!=10) OR (!preg_match("~[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]~", $dob, $matches))) {
          echo "Date formatting error for date of birth, please use YYYY-MM-DD format<br>";
          $quitError=1;
          $dateError=1;
        }

        $sex = $_GET['gender'];
        
        $dod = $_GET['DOD'];
        if($dod == ""){
          $dod = NULL;
        }
        if ($dod != NULL AND ((strlen($dod) != 10) OR (!preg_match("~[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]~", $dod, $matches)))) {
          echo "Date formatting error for date of death, please use YYYY-MM-DD<br>";
          $quitError = 1;
          $dateError = 1;
        }

        # TODO: Compare dob to dod
        if (($quitError == 0) AND (strcmp($dob, $dod) > 0) {
          echo "Date of birth cannot be after date of death.";
          $quitError = 1;
          $dateError = 1;
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

          echo "<br>Success! You have added the following " . $occupation . ":<br><br>";
          echo "<b>Name</b>: " . $firstName . " " . $lastName . "<br>";
          echo "<b>Gender</b>: " . $sex . "<br>";
          echo "<b>Date of Birth</b>: " . $dob . "<br>";
          echo "<b>Date of Death</b>: " . ($dod == NULL ? 'N/A' : $dod) . "<br>";
        }
      }
    ?>
    </div>
  </body>
</html>
