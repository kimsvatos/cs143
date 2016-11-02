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
  <li><a href="./review.php">Add a Review</a></li>
  <li><a href="./ActorInfo.php">Actor Lookup</a></li>
  <li><a href="./MovieSearchPage.php">Movie Lookup</a></li>
  <li><a href="./GenSearch.php">General Search</a></li>
 
</ul>
<div style="margin-left:25%;padding:1px 16px;height:1000px;">



<h1>Information</h1>
<!-- do queries and show table with DOB name DOD blah --> 
<?php
//if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
	$id = $_GET['id'];
	$database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }

  
    $id_query = "SELECT first, last, dob, sex, dod FROM Actor WHERE id=$id;";
    $queryRes = $database->query($id_query);
    $result = $queryRes->fetch_array(MYSQLI_ASSOC);
    echo $result['last'];
    echo " <h1> Actor Information is: </h1>";
    echo "<table border='1' bordercolor='black' cellpadding='2'>";
    echo "<tr>";
    echo "<th>Actor Name</th>";
    echo "<th>Sex</th>";
    echo "<th>Date of Birth</th>";
    echo "<th>Date of Death</th>";
    echo "</tr>";

    //add info below
    echo "<tr>";
    echo "<td>" . $result['first'] . " " . $result['last'] . "</td>";
	echo "<td>" . $result['sex'] . "</td>";
	echo "<td>" . $result['dob'] . "</td>";
	if($result['dod'] == NULL){
		echo "<td> N/A </td>";
	}
	else{
		echo "<td>" . $result['dod'] . "</td>";
	}
	echo "</tr>";
	echo "</table>";

	//movie info 
	$id_query = "SELECT role, id, title from MovieActor, Movie WHERE aid = $id AND mid=id";
    $queryRes = $database->query($id_query);
    
	///next table
    echo "<h1> Actor's Movies and Role:</h1>";
    echo "<table border='1' bordercolor='black' cellpadding='2'>";
    echo "<tr>";
    echo "<th>Movie Title</th>";
    echo "<th>Role</th>";
   
    echo "</tr>";

    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
    echo "<tr>";
    echo "<td> <a href=\"MovieLinkResult.php?id=".$result["id"]."\">".$result['title']."</a></td>";
    echo "<td> <a href=\"MovieLinkResult.php?id=".$result["id"]."\">".$result['role']."</a></td>";
	echo "</tr>";
	}	

	echo "</table>";

//}
?>
	<form method="post" action="ActorInfo.php">
        <input type="Submit" value="Go back to make another search!">
    </form>
</div>
</body>


</html>