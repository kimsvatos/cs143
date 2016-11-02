<html>
  <head>
    <title>
      CS143 Movie Database
    </title>
    <link rel=StyleSheet HREF="./style.css" TYPE="text/css" MEDIA="screen">
  </head>
  <body>
    <ul>
      <li><a class="home" href="./index.php">Home</a></li>
      <li><a class="header">Add Content</a></li>
      <li><a href="./test.php">Add New Actor/Director</a></li>
      <li><a href="./movieInfo.php">Add New Movie</a></li>
      <li><a href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a href="./movieDirector.php">Add Movie/Director Relationship</a></li>
      <li><a class="active" href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>

    <div style="margin-left:25%; padding:1px 16px; height:auto;">
    <div id="banner">
      <div id="banner-content"><h1>Add Review</h1>
      </div>
    </div>

    <div id="main-body">

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
     
      <b>Your Name</b>:
      <br>
      <INPUT TYPE="text" name="name" size="20" maxlength="20" placeholder="e.g., John Smith">
      <br><br>
      <b>Movie</b><br> 
      <SELECT name="movie">
            <?=$movieList?>
      </SELECT><br><br>



      <b>Rating</b><br><SELECT NAME="rating">
      <OPTION SELECTED>Select a Rating
      <OPTION> 0
      <OPTION> 1
      <OPTION> 2
      <OPTION> 3
      <OPTION> 4
      <OPTION> 5
      </SELECT>
      <br><br>
      <b>Comments</b>:<br>
      <TEXTAREA name="comment" ROWS=7 COLS=72 maxlength="500" placeholder="Max 500 characters"></TEXTAREA><br>

      <br>
      <input type="submit" value="Add Review!">
   </form>
<?php
      if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
      $quitError=0;
      if (!($name = $_GET['name'])) {
          echo "Need a name<br>";
          $quitError=1;
        }
        if (($movie = $_GET['movie']) == "Select Movie") {
          echo "Need a movie!<br>";
          $quitError=1;
        }
       if (($rating = $_GET['rating']) == "Select a Rating") {
          echo "Need a rating!<br>";
          $quitError=1;
        }
        if(!$quitError){
        
            $comment = $_GET['comment'];

            $rStatement = $database->prepare("INSERT INTO Review (name, `time`, mid, rating, comment) VALUES (?, ?, ?, ?, ?);");
            $timeS = date('Y-m-d H:i:s');
      
            $rStatement->bind_param("ssiis", $name, $timeS, $movie, $rating, $comment);
         
            $rStatement->execute();

            echo "Success!";
        }
      }
?>

 	  
      </div>
    </div>
	</body>
</html>