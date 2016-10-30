<html>
<title>CS 143 Movie Database</title>
	<h1>Review a Movie!</h1>
	<body>
      <?php
      $database = new mysqli('localhost', 'cs143', '', 'CS143');
            if($database->connect_errno > 0){
              die('Unable to connect to database [' . $database->connect_error . ']');
            }
      $movieQuery = $database->query("SELECT id, title, year FROM Movie order by title;");
      $movieList = "";
      $movieList = $movieList . "<option SELECTED value=\"-1\"> Select Movie </option>";
      while($row = $movieQuery->fetch_array(MYSQLI_ASSOC)){
            $id = $row["id"];
            $title = $row["title"];
            $year = $row["year"];
            $movieList = $movieList . "<option value=\"$id\"> $title ($year) </option>";
      }
?>
	  <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
     
      <br>
      Your Name:
      <br>
      <INPUT TYPE="text" name="name" size="20" maxlength="20">
      <br><br>
      Movie 
      <SELECT name="movie">
            <?=$movieList?>
      </SELECT><br>



      Rating:<SELECT NAME="rating">
      <OPTION SELECTED> Select a Rating
      <OPTION> 0
      <OPTION> 1
      <OPTION> 2
      <OPTION> 3
      <OPTION> 4
      <OPTION> 5
      </SELECT>
      <br>
      <input type="textarea" name="comment" ROWS=10 COLUMNS=60><br>
      Must be 500 characters or less. 
      <br>

      


 	  <br>
      <input type="submit" value="Submit">
   </form>



	</body>




</html>