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
      <li><a href="./movieInfo.php">Add New Movie</a></li>
      <li><a href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a class="active" href="./movieDirector.php">Add Movie/Director Relationship</a></li>
      <li><a href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>
    <div style="margin-left:25%; padding:1px 16px; height:1000px;">

    <h1>Add Movie/Director Relationship</h1>

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

    $directorQuery = $database->query("SELECT id, last, first, dob FROM Director order by last;");
    $directorList = "";
    $directorList = $directorList . "<option value=\"-1\"> Select Director</option>";
    while($row = $directorQuery->fetch_array(MYSQLI_ASSOC)){
    	$last = $row["last"];
    	$id = $row["id"];
    	$first = $row["first"];
     	$dob = $row["dob"];
    	$directorList = $directorList . "<option value=\"$id\"> $first $last ($dob)</option>";

    }

?>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
	<SELECT name="movie">
		<?=$movieList?>
	</SELECT>
	<SELECT name="director">
		<?=$directorList?>
	</SELECT><br><br>
		<input type="submit" value="Add Movie/Director relationship!">

</form>
<?php
    if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
	$quitError = 0;
	if(($movieID = $_GET['movie'] ) == "-1"){
		echo "Must select a movie!<br>";
		$quitError = 1;
	}
	if(($dID = $_GET['director'] ) == "-1"){
		echo "Must select a director!<br>";
		$quitError = 1;
	}


	if(!$quitError){
		//do stuff we like
		$statement = $database->prepare("INSERT INTO MovieDirector (mid, did) VALUES (?, ?);");
        $statement->bind_param("ii", $movieID, $dID);
        $statement->execute();
        echo "Success!";
	}
}

?>

</div>
</body>
</html>
