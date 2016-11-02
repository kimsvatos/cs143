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
  <li><a class="active" href="./movieActor.php">Add a Movie/Actor relationship!</a></li>
  <li><a href="./movieDirector.php">Add a Movie/Director relationship!</a></li>
  <li><a href="./review.php">Add a Review</a></li>
  <li><a href="./ActorInfo.php">Actor Lookup</a></li>
  <li><a href="./MovieSearchPage.php">Movie Lookup</a></li>
  <li><a href="./GenSearch.php">General Search</a></li>
 
</ul>
<div style="margin-left:25%;padding:1px 16px;height:1000px;">

<?php
if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
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

    $actorQuery = $database->query("SELECT id, last, first, dob FROM Actor order by last;");
    $actorList = "";
    $actorList = $actorList . "<option value=\"-1\"> Select Actor</option>";
    while($row = $actorQuery->fetch_array(MYSQLI_ASSOC)){
    	$last = $row["last"];
    	$id = $row["id"];
    	$first = $row["first"];
     	$dob = $row["dob"];
    	$actorList = $actorList . "<option value=\"$id\"> $first $last ($dob)</option>";

    }

?>
<h1> omg lets try it </h1>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
	<SELECT name="movie">
		<?=$movieList?>
	</SELECT>
	<SELECT name="actor">
		<?=$actorList?>
	</SELECT><br>
	<input type="text" name="role" size="50" maxlength="50"><br>
	<input type="submit" value="Add Movie/Actor relationship!">

</form>
<?php
	if ($_SERVER["REQUEST_METHOD"] == "GET"){
	$quitError = 0;
	if(($movieID = $_GET['movie'] ) == "-1"){
		echo "Must select a movie!<br>";
		$quitError = 1;
	}
	if(($actorID = $_GET['actor'] ) == "-1"){
		echo "Must select an actor!<br>";
		$quitError = 1;
	}
	if(!($role = $_GET['role'])){
		echo "Must specify a role!<br>";
		$quitError = 1;
	}

	if(!$quitError){
		//do stuff we like
		$statement = $database->prepare("INSERT INTO MovieActor (mid, aid, role) VALUES (?, ?, ?);");
        $statement->bind_param("iis", $movieID, $actorID, $role);
        $statement->execute();
        echo "success!";
	}
}
}
?>




</div>
</body>
</html>