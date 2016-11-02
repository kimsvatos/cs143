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
      <li><a href="./test.php">Add New Actor/Director</a></li>
      <li><a class="active" href="./movieInfo.php">Add New Movie</a></li>
      <li><a href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a href="./movieDirector.php">Add Movie/Director Relationship</a></li>
      <li><a href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>
    <div style="margin-left:25%; padding:1px 16px; height:1000px;">

    <h1>Add New Movie</h1>

	  <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
     
      <b>Title</b> <br> <INPUT TYPE="text" name="title" placeholder="e.g., Titanic">
      
      <br><br>
      
      <b>Company</b> <br> <input type="text" name="company" placeholder="e.g., 20th Century Fox">
      
      <br><br>
      
      <b>Year</b> <br> <input type="text" name="year" placeholder="e.g., 1997">
      
      <br><br>

      <b>Rating</b> <br>
      <SELECT NAME="rating">
      <OPTION SELECTED> G
      <OPTION> PG
      <OPTION> PG-13
      <OPTION> R
      <OPTION> NC-17
      <OPTION> surrendere
      </SELECT>

      <br><br>

      <b>Genre</b> <br>

        <!--TODO: turn into table???-->

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
 	  
    <br><br>

      <input type="submit" value="Submit">
   </form>

   <?php
   		if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {

        $quitError=0;
        $yearError=0;

        # CHECK that we have valid input
        if (!($movieTitle = $_GET['title'])) {
          echo "Need a movie title!<br>";
          $quitError=1;
        }

        if (!($movieCo = $_GET['company'])) {
          echo "Need a movie company!<br>";
          $quitError=1;
        }

        if (!($year = $_GET['year'])) {
          echo "Need to specify a year!<br>";
          $quitError=1;
          $yearError=1;
        }       

        if (($yearError == 0) and (preg_match("~[^0-9]~", $year, $matches))) {
          echo "Date formatting error for year, please use only numeric input<br>";
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
     	    $rating = $_GET['rating'];
     	    $genreArr = array();
          $genrePrint = array();
     	    $k = 0;

          if(($genre = $_GET['action']) == "ON"){
          	array_push($genreArr, "Action");
            array_push($genrePrint, "Action");
          }
          if(($genre =$_GET['adult']) == "ON"){
        	array_push($genreArr, "Adult");
          array_push($genrePrint, "Adult");
          }
          if($adventure = $_GET['adventure']){
        	//insert
        	array_push($genreArr, "Adventure");
          array_push($genrePrint, "Adventure");
          }
          if($animation = $_GET['animation']){
        	//inser
        	array_push($genreArr, "Animation");
          array_push($genrePrint, "Animation");
          }
          if($comedy = $_GET['comedy']){
        	//insert
        	array_push($genreArr, "Comedy");
          array_push($genrePrint, "Comedy");
          }
          if($crime = $_GET['crime']){
        	//insert
        	array_push($genreArr, "Crime");
          array_push($genrePrint, "Crime");
          }
          if($documentary = $_GET['documentary']){
        	//insert
        	array_push($genreArr, "Documentary");
          array_push($genrePrint, "Documentary");
          }
          if($drama= $_GET['drama']){
        	//insert
        	array_push($genreArr, "Drama");
          array_push($genrePrint, "Drama");
          }
          if($family = $_GET['family']){
        	//insert
        	array_push($genreArr, "Family");
          array_push($genrePrint, "Family");
          }
          if($fantasy = $_GET['fantasy']){
        	//insert
        	array_push($genreArr, "Fantasy");
          array_push($genrePrint, "Fantasy");
          }
          if($horror = $_GET['horror']){
        	//insert
        	array_push($genreArr, "Horror");
          array_push($genrePrint, "Horror");
          }
          if($musical = $_GET['musical']){
        	//insert
        	array_push($genreArr, "Musical");
          array_push($genrePrint, "Musical");
          }
          if($mystery = $_GET['mystery']){
        	//insert
        	array_push($genreArr, "Mystery");
          array_push($genrePrint, "Mystery");
          }
          if($romance= $_GET['romance']){
        	//insert
        	array_push($genreArr, "Romance");
          array_push($genrePrint, "Romance");
          }
          if($scifi = $_GET['scifi']){
        	//insert
        	array_push($genreArr, "Scifi");
          array_push($genrePrint, "Scifi");
          }
          if($short = $_GET['short']){
        	//insert
        	array_push($genreArr, "Short");
          array_push($genrePrint, "Short");
          }
          if($thriller = $_GET['thriller']){
        	//insert
        	array_push($genreArr, "Thriller");
          array_push($genrePrint, "Thriller");
          }
          if($war = $_GET['war']){
        	//insert
        	array_push($genreArr, "War");
          array_push($genrePrint, "War");
          }
          if($western = $_GET['western']){
        	//insert
        	array_push($genreArr, "Western");
          array_push($genrePrint, "Western");
          }




          #movie
       
          $movieStatement = $database->prepare("INSERT INTO Movie (id, title, year, rating, company) VALUES (?, ?, ?, ?, ?);");
          $movieStatement->bind_param("isiss", $id, $movieTitle, $year, $rating, $movieCo);
          $movieStatement->execute();
          //TODO: make sure INSERT succeeded before updating this value

          $id_statement = $database->prepare("UPDATE MaxMovieID SET id = $id;");
          $id_statement->execute();

          
          $count = count($genreArr);
          for($i=0; $i < $count; $i++){
          		 $genre = array_pop($genreArr);
         		 $g_statement = $database->prepare("INSERT INTO MovieGenre (mid, genre) VALUES (?, ?);");
          	   	 $g_statement->bind_param("is", $id, $genre);
          	     $g_statement->execute();
           }

          echo "Success! You have added the following Movie:<br><br>";
          echo "<b>Title</b>: " . $movieTitle . "<br>";
          echo "<b>Company</b>: " . $movieCo . "<br>";
          echo "<b>Year</b>: " . $year . "<br>";
          echo "<b>Rating</b>: " . $rating . "<br>";
          echo "<b>Genre(s)</b>: ";

          $count = count($genrePrint);
          if ($count == 0) { echo "none"; }
          else {
            for($i=0; $i < $count; $i++){
               $genre = array_pop($genrePrint);
               echo $genre;
               if (count($genrePrint) > 0) {
                  echo ", ";
              }
            }    
           }      

        }
      }
   ?>
    </div>
	</body>
</html>