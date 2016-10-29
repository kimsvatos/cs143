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

    $actorQuery = $database->query("SELECT id, last, first, dob FROM Actor order by last;");
    $actorList = "";
    $actorList = $actorList . "<option value=\"-1\"> Select Actor</option>";
    while($row = $actorQuery->fetch_array(MYSQLI_ASSOC)){
    	$last = $row["last"];
    	$id = $row["id"];
    	$first = $row["first"];
     	$dob = $row["dob"];
    	$actorList = $actorList . "<option value=\"$id\"> $last $first ($dob)</option>";

    }

?>
<h1> omg lets try it </h1>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
	<SELECT name="movie">
		<?=$movieList?>
	</SELECT>
	<SELECT name="actor">
		<?=$actorList?>
	</SELECT>
	<input type="text" name="role" size="50" maxlength="50"><br>
	<input type="submit" value="Add Movie/Actor relationship!">

</form>
<?php
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


?>





</body>
</html>