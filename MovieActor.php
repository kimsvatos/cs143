<html>
<body>
<?php
	$database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }
    $movieQuery = $database->query("SELECT id, title, year FROM Movie order by title;");
    $movieList = "";
    while($row = $movieQuery->fetch_array(MYSQLI_ASSOC)){
    	$id = row["id"];
    	$title = row["title"];
    	$year = row["year"];
    	//$movieList = $movieList . "<option value=\"id\"> $title ($year) </option>";

    }

?>
<h1> omg lets try it </h1>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
	<SELECT name="movie">
		<!--<?=$movieList?>-->
	</SELECT>

</form>


</body>
</html>