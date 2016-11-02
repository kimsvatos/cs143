<!DOCTYPE html>
<html>
<head>
<style>
body {
    margin: 0;
}

ul {
    list-style-type: none;
    margin: 0;
    padding: 0;
    width: 25%;
    background-color: #f1f1f1;
    position: fixed;
    height: 100%;
    overflow: auto;
}

li a {
    display: block;
    color: #000;
    padding: 8px 16px;
    text-decoration: none;
}

li a.active {
    background-color: #4CAF50;
    color: white;
}

li a:hover:not(.active) {
    background-color: #555;
    color: white;
}
</style>
</head>
<body>


<ul>
  <li><a href="./index.php">Home</a></li>
  <li><a href="./movieInfo.php">Insert a New Actor/Director</a></li>
  <li><a href="./movieInfo.php">Insert a New Movie</a></li>
  <li><a href="./movieActor.php">Add a Movie/Actor relationship!</a></li>
  <li><a href="./movieDirector.php">Add a Movie/Director relationship!</a></li>
  <li><a class="active" href="./review.php">Add a Review</a></li>
  <li><a href="./ActorInfo.php">Actor Lookup</a></li>
  <li><a href="./MovieSearchPage.php">Movie Lookup</a></li>
  <li><a href="./GenSearch.php">General Search</a></li>
 
</ul>
<div style="margin-left:25%;padding:1px 16px;height:1000px;">

	<h1>Review a Movie!</h1>

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
      </SELECT><br><br>



      Rating:<SELECT NAME="rating">
      <OPTION SELECTED>Select a Rating
      <OPTION> 0
      <OPTION> 1
      <OPTION> 2
      <OPTION> 3
      <OPTION> 4
      <OPTION> 5
      </SELECT>
      <br><br><br>
      <h4>Comments:</h4>
      <input type="textarea" name="comment" ROWS=10 COLUMNS=60><br>
      Must be 500 characters or less. 
      <br>
      <br>
      <input type="submit" value="Submit">
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

            echo "success!";
        }
       }
?>

 	  



	</body>




</html>