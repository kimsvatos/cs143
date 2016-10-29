<html>
<title>i hate kyle</title>
	<h1 color="blue"> Add Movie Information</h1>
	<body>
	  <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
     
      <br>
      Title
      <br>
      <INPUT TYPE="text" name="title">
      <br><br>
      Company
      <br>
      <input type="text" name="company">
      <br>
      Year
      <br>
      <input type="text" name="year">
      <br>

      Rating:<SELECT NAME="Rating">
      <OPTION> G
      <OPTION> PG
      <OPTION> PG-13
      <OPTION> R
      <OPTION> NC-17
      <OPTION> surrendere
      </SELECT>

      <br>
      Action<input type="checkbox" name="action" value="ON">
 	  Adult<input type="checkbox" name="adult" value="ON">
      Adventure<input type="checkbox" name="adventure" value="ON">
 	  Animation<input type="checkbox" name="animation" value="ON">
 	  Comedy<input type="checkbox" name="comedy" value="ON"><br>
 	  Crime<input type="checkbox" name="crime" value="ON">
      Documentary<input type="checkbox" name="documentary" value="ON">
 	  Drama<input type="checkbox" name="drama" value="ON">
 	  Family<input type="checkbox" name="family" value="ON"><br>
 	  Fantasy<input type="checkbox" name="fantasy" value="ON">
      Horror<input type="checkbox" name="horror" value="ON">
 	  Musical<input type="checkbox" name="musical" value="ON">
 	  Mystery<input type="checkbox" name="mystery" value="ON"><br>
 	  Romance<input type="checkbox" name="romance" value="ON">
      Sci-Fi<input type="checkbox" name="scifi" value="ON">
 	  Short<input type="checkbox" name="short" value="ON">
 	  Thriller<input type="checkbox" name="thriller" value="ON"><br>
 	  War<input type="checkbox" name="war" value="ON">
      Western<input type="checkbox" name="western" value="ON">
 	  <br>
      <input type="submit" value="Submit">
   </form>
   <?php
   		if ($_SERVER["REQUEST_METHOD"] == "GET"){
        $quitError=0;
      # CHECK that we have valid input
        if (!($movieTitle = $_GET['title'])) {
          echo "Need a movie title!<br>";
          $quitError=1;
        }
        if (!($movieCo = $_GET['company'])) {
          echo "Need a movie company!<br>";
          $quitError=1;
        }
       
        $year = $_GET['year'];
        $horror = $_GET['horror'];
        $drama = $_GET['drama'];

        echo "$drama<br>";
        echo "$horror";



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