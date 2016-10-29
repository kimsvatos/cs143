<html>
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
<h1> omg lets try it </h1>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
	<SELECT name="movie">
		<?=$movieList?>
	</SELECT>
	<SELECT name="director">
		<?=$directorList?>
	</SELECT><br>
		<input type="submit" value="Add Movie/Director relationship!">

</form>
<?php
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
        echo "success!";
	}


?>

</body>
</html>
