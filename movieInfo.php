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

      Rating:<SELECT NAME="rating">
      <OPTION SELECTED> G
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
       


        if(!$quitError){
          #where we would want to INSERT
          $database = new mysqli('localhost', 'cs143', '', 'CS143');
          if($database->connect_errno > 0){
            die('Unable to connect to database [' . $database->connect_error . ']');
          }
          //make sure to do query to get max person ID and then increment that too
          $id_query = "SELECT * FROM MaxMovieID;";
          $MaxMovieID = $database->query($id_query);
          $result = $MaxMovieID->fetch_array(MYSQLI_NUM);
          $id = $result[0] + 1;
          $year = $_GET['year'];
     	  $rating = $_GET['rating'];
     	  $genreArr = array();
     	  $k = 0;

          if(($genre = $_GET['action']) == "ON"){
          	$genreArr[$k]='Action';
          	$k++;
          }
          if(($genre =$_GET['adult']) == "ON"){
        	$genreArr[$k]='Adult';
          	$k++;
          	echo "$genreArr[$k - 1] <br>";
          }
          if($adventure = $_GET['adventure']){
        	//insert
          }
          if($animation = $_GET['animation']){
        	//inser
          }
          if($comedy = $_GET['comedy']){
        	//insert
          }
          if($crime = $_GET['crime']){
        	//insert
          }
          if($documentary = $_GET['documentary']){
        	//insert
          }
          if($drama= $_GET['drama']){
        	//insert
          }
          if($family = $_GET['family']){
        	//insert
          }
          if($fantasy = $_GET['fantasy']){
        	//insert
          }
          if($horror = $_GET['horror']){
        	//insert
          }
          if($musical = $_GET['musical']){
        	//insert
          }
          if($mystery = $_GET['mystery']){
        	//insert
          }
          if($romance= $_GET['romance']){
        	//insert
          }
          if($scifi = $_GET['scifi']){
        	//insert
          }
          if($short = $_GET['short']){
        	//insert
          }
          if($thriller = $_GET['thriller']){
        	//insert
          }
          if($war = $_GET['war']){
        	//insert
          }
          if($western = $_GET['western']){
        	//insert
          }




          #movie
       
          $movieStatement = $database->prepare("INSERT INTO Movie (id, title, year, rating, company) VALUES (?, ?, ?, ?, ?);");
          $movieStatement->bind_param("isiss", $id, $movieTitle, $year, $rating, $movieCo);
          $movieStatement->execute();
          //TODO: make sure INSERT succeeded before updating this value

          $id_statement = $database->prepare("UPDATE MaxMovieID SET id = $id;");
          $id_statement->execute();

          for($i=0; $i < count($genreArr); $i++){
          		 $genre = $genreArr[i];
          		 echo "$genre<br>";
         		 $g_statement = $database->prepare("INSERT INTO MovieGenre (mid, genre) VALUES (?, ?);");
          	   	 $g_statement->bind_param("is", $id, $genre);
          	     $g_statement->execute();
           }
          echo "submitted!";
        }
      }



   ?>






	</body>




</html>